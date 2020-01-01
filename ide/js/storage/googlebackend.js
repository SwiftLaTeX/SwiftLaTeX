function GoogleBackend() {


    this.canCache = function () {
        return true;
    };


    this.init = function () {
        return new Promise((resolve, reject) => {
            gapi.load('client:auth2', function () {
                gapi.client.setToken({access_token: window.access_token});
                gapi.client.load('drive', 'v3', function () {
                    console.log("Loaded gapi client");
                    return resolve();
                });

            });
        });
    };

    this._convertGoogleEntry2Meta = function (entry) {
        //console.log(entry);
        let meta = entry.appProperties;
        if (meta === undefined) {
            throw new Error("A file without properties detected.");
        }
        if (meta.isfolder !== undefined) {
            if (typeof (meta.isfolder) === "string") {
                meta.isfolder = (meta.isfolder === "true");
            }
        } else {
            meta.isfolder = true;
        }


        if (meta.mtime !== undefined) {
            if (typeof (meta.mtime) === "string") {
                meta.mtime = parseInt(meta.mtime);
            }
        } else {
            meta.mtime = Date.now();
        }

        if (meta.share !== undefined) {
            if (typeof (meta.share) === "string") {
                meta.share = (meta.share === "true");
            }
        } else {
            meta.share = false;
        }

        if (meta.uid === undefined || meta.uid === "") {
            meta.uid = entry.owners[0].displayName;
        }

        if(meta.fid === undefined) {
            meta.fid = entry.id;
        }

        return meta;
    };



    this.headItem = async function (path) {

        console.log("head item " + path);

        if(path === "/") {
            let tmp = new FileMeta();
            tmp.aid = "/";
            tmp.fid = "root";
            return tmp;
        }

        let resp = await gapi.client.drive.files.list({
            pageSize: 1000,
            corpus: 'user',
            spaces: 'drive',
            fields: "files(id, appProperties, owners)",
            q: ` appProperties has { key='aid' and value='${path}'} `,
        });
        let entries = resp.result.files;
        if (entries === undefined || entries.length === 0) {
            return null;
        }

        if(entries.length > 1) {
            //Solve conflict, hopefully it never happend.
            for (let i = 1; i < entries.length; i++) {
                let tmp = entries[i];
                let tmpmeta = this._convertGoogleEntry2Meta(tmp);
                let tmpgid = tmpmeta.fid;
                tmpmeta.aid = tmpmeta.aid + this._genID(3);
                await gapi.client.drive.files.update({
                    fileId: tmpgid,
                    name: basename(tmpmeta.aid),
                    appProperties: tmpmeta
                });
            }
        }
        let entry = entries[0];
        let meta = this._convertGoogleEntry2Meta(entry);
        return meta;

    };


    this.setItem = async function (path, meta, data) {

        console.log("SetItem " + path);
        let parent = dirname(path);
        let base = basename(path);
        let gid = null;
        if(meta.fid !== undefined && meta.fid !== '') {
            gid = meta.fid;
        } else {
            let tmpmeta = await this.headItem(path);
            if(tmpmeta != null) gid = tmpmeta.fid;
        }

        if (gid == null) { //Create
            let fileMetadata = {
                'name': base,
                'appProperties': meta,
            };

            let parent_meta = await this.headItem(parent);
            if (parent_meta != null) {
                fileMetadata['parents'] = [parent_meta.fid];
            }

            if (meta.isfolder) {
                fileMetadata['mimeType'] = 'application/vnd.google-apps.folder';
            }

            let resp = await gapi.client.drive.files.create({
                resource: fileMetadata,
                fields: 'id'
            });

            gid = resp.result.id;
        }


        meta.aid = path;
        meta.fid = gid;
        console.log(meta);
        if (data === undefined) {
            await gapi.client.drive.files.update({
                fileId: gid,
                name: base,
                appProperties: meta
            });
        } else {
            let tmpBlob = new Blob([data]);
            let b64str = await new Promise((resolve, reject) => {
                const fr = new FileReader();
                fr.onload = function () {
                    return resolve(fr.result);
                };
                fr.readAsBinaryString(tmpBlob);
            });
            await this._uploadBinaryString(gid, b64str, meta);
        }
    };


    this.getItem = async function (objectPath, rev) {

        console.log("GetItem " + objectPath);
        let meta = await this.headItem(objectPath);
        if (meta == null) {
            throw new Error("file not available");
        }
        let gid = meta.fid;
        let fileBlob = await new Promise((resolve, reject) => {
            let downloadxhr = new XMLHttpRequest();
            let url = "https://content.googleapis.com/drive/v3/files/" + gid;
            if (rev !== undefined) {
                url += "/revisions/" + rev;
            }
            url += '?alt=media';
            downloadxhr.open("GET", url);
            downloadxhr.setRequestHeader('Authorization', 'Bearer ' + window.access_token);
            downloadxhr.responseType = 'blob';
            downloadxhr.onload = function () {
                //console.log(downloadxhr.response);
                resolve(downloadxhr.response);
            };
            downloadxhr.onerror = function () {
                reject();
            };
            downloadxhr.send();
        });
        return fileBlob;
    };

    this.listItems = async function (parentPath, recursive) {

        console.log("ListItems " + parentPath);
        let meta = await this.headItem(parentPath);

        if (meta == null) {
            return [];
        }

        let gid = meta.fid;

        let tasks = [gid];
        //console.error(`${gid} in ${parentId}`);

        let res = [];
        while (tasks.length > 0) {
            let tid = tasks.pop();
            //Work around
            let query =  `('${tid}' in parents) and (appProperties has {key='isfolder' and value='true'} or appProperties has {key='isfolder' and value='false'} )   `;
            if(tid === 'root') {
                query =  ` ('${tid}' in parents or sharedWithMe = true) and (appProperties has {key='isfolder' and value='true'} or appProperties has {key='isfolder' and value='false'} ) `
            }
            let resp = await gapi.client.drive.files.list({
                pageSize: 1000,
                corpus: 'user',
                spaces: 'drive',
                fields: "files(id, name, parents, owners, modifiedTime, mimeType, appProperties)",
                q: query
            });
            for (let i = 0; i < resp.result.files.length; i++) {
                let entry = resp.result.files[i];
                let tmpMeta = this._convertGoogleEntry2Meta(entry);
                res.push({id: tmpMeta.aid, meta: tmpMeta});
                if (recursive && tmpMeta.isfolder) {
                    tasks.push(entry.id);
                }
            }
        }
        //console.log(res);
        return res;
    };


    this.renameItem = async function (oldpath, newpath) {
        let entry = await this.headItem(oldpath);
        if (entry == null) return;

        if (entry.isfolder) {
            let dirs = await this.listItems(oldpath, true);
            for (let j = 0; j < dirs.length; j++) {
                let tmp = dirs[j];
                let tmpmeta = tmp.meta;
                let tmpgid = tmpmeta.fid;
                tmpmeta.aid = tmpmeta.aid.replace(oldpath, newpath);
                await gapi.client.drive.files.update({
                    fileId: tmpgid,
                    appProperties: tmpmeta
                });
            }
        }

        let gid = entry.fid;
        entry.aid = newpath;
        await gapi.client.drive.files.update({
            fileId: gid,
            name: basename(newpath),
            appProperties: entry
        });

    };

    this.removeItem = async function (path) {

        console.log("Remove Item " + path);
        let meta = await this.headItem(path);
        if(meta == null) {
            return ;
        }
        let gid = meta.fid;

        await gapi.client.drive.files.delete({
                fileId: gid
        });


    };

    this._uploadBinaryString = async function (fid, data, fileMetadata, contentTypeSupplied) {
        const boundary = '-------314159265358979323846';
        const delimiter = "\r\n--" + boundary + "\r\n";
        const close_delim = "\r\n--" + boundary + "--";
        //console.log(fileMetadata);
        let gMeta = {'appProperties': fileMetadata};

        let contentType = contentTypeSupplied || 'application/octect-stream';
        var base64Data = btoa(data);
        var multipartRequestBody =
            delimiter +
            'Content-Type: application/json\r\n\r\n' +
            JSON.stringify(gMeta) +
            delimiter +
            'Content-Type: ' + contentType + '\r\n' +
            'Content-Transfer-Encoding: base64\r\n' +
            '\r\n' +
            base64Data +
            close_delim;
        let res = await gapi.client.request({
            'path': '/upload/drive/v3/files/' + fid,
            'method': 'PATCH',
            'params': {'uploadType': 'multipart', 'alt': 'json'},
            'headers': {
                'Content-Type': 'multipart/mixed; boundary="' + boundary + '"'
            },
            'body': multipartRequestBody
        });

        //console.log(res);
        return res.result;
    };

    this.getFileRevisionList = async function (objectId) {
        let meta = await this.headItem(objectId);
        if (meta == null) {
            return [];
        } else {
            let gid = meta.fid;
            return this._getFileRevisionListByID(gid);
        }
    };

    this._getFileRevisionListByID = async function (cachedFid) {

        let resp = await gapi.client.drive.revisions.list({
            fileId: cachedFid,
            fields: '*',
            pageSize: 200
        });
        let res = [];
        let revList = resp.result.revisions;
        revList.forEach(entry => {
            let tmp = {};
            tmp['fid'] = cachedFid;
            tmp['rid'] = entry.id;
            tmp['user'] = entry.lastModifyingUser.displayName;
            tmp['filename'] = entry.originalFilename;
            let mTime = entry.modifiedTime.replace("T", " ");
            let tmpIndex = mTime.indexOf(".");
            mTime = mTime.substr(0, tmpIndex);
            tmp['modifiedTime'] = mTime;
            tmp['size'] = entry.size;
            res.push(tmp);
        });
        return res;
    };

    this.share = async function (normPath, receiver) {
        let meta = await this.headItem(normPath);

        if (meta == null) return false;

        let gid = meta.fid;
        return gapi.client.request({
            'path': '/drive/v3/files/' + gid + '/permissions',
            'method': 'POST',
            'params': {
                'emailMessage': 'To work on this amazing project, please use the following link SwiftLaTeX (https://www.swiftlatex.com/oauth/login_oauth?type=google&permission=full).',
                'sendNotificationEmail': 'true'
            },
            'body': {
                'role': 'writer', // owner, writer, commenter
                'type': 'user',
                'emailAddress': receiver
            }
        });

    };
}