function DropboxBackend() {


    this.dbx = new Dropbox.Dropbox({accessToken: window.access_token, fetch: fetch});

    this.canCache = function () {
        return true;
    };

    this.init = async function () {

    };

    this.setItem = async function (path, meta, data) {
        console.log("SetItem " + path);
        if (meta.isfolder) {
            if (data === undefined) return;
            await this.dbx.filesCreateFolder({path: path, autorename: false});
        } else {
            await this.dbx.filesUpload({path: path, contents: data, mode: 'overwrite'});
        }
    };


    this.getItem = async function (path, rev) {
        if (rev !== undefined) {
            let response = await this.dbx.filesDownload({path: rev});
            return response.fileBlob;
        } else {
            let response = await this.dbx.filesDownload({path: path});
            return response.fileBlob;
        }
    };

    this._convertDropboxEntry2Meta = function (entry) {
        let meta = new FileMeta();
        let isfolder = entry['.tag'] === 'folder';
        meta.isfolder = isfolder;
        if (entry.server_modified !== undefined) {
            meta.mtime = Date.parse(entry.server_modified);
        }
        return meta;
    };

    this.headItem = async function (path) {
        try {
            let resp = await this.dbx.filesGetMetadata({path: path});
            return this._convertDropboxEntry2Meta(resp);
        } catch (e) {
            if (e.status === 409)
                return null;
            throw e;
        }
    };


    this.listItems = async function (path, recursive) {
        console.log("ListItems " + path);
        if (path === "/") path = "";
        let response = await this.dbx.filesListFolder({path: path, 'recursive': recursive});
        let entries = response.entries;
        let res = [];
        for (let i = 0; i < entries.length; i++) {
            let entry = entries[i];
            let id = entry.path_display;
            let meta = this._convertDropboxEntry2Meta(entry);
            res.push({'id': id, 'meta': meta});
        }
        return res;
    };


    this.removeItem = async function (path) {
        await this.dbx.filesDelete({path: path});
    };

    this.renameItem = async function (oldpath, newpath) {
        await this.dbx.filesMove({from_path: oldpath, to_path: newpath});
    };


    this.getFileRevisionList = async function (normPath) {

        let resp = await this.dbx.filesListRevisions({path: normPath});
        let res = [];
        let revList = resp.entries;
        revList.forEach(entry => {
            let tmp = {};
            tmp['fid'] = entry.id;
            tmp['rid'] = "rev:" + entry.rev;
            tmp['user'] = "Author";
            tmp['filename'] = entry.name;
            tmp['modifiedTime'] = entry.server_modified;
            tmp['size'] = entry.size;
            res.push(tmp);
        });

        return res;
    };

    this.share = async function (normPath) {
        return false;
    };
}