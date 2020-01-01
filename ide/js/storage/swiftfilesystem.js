function PathException(message) {
    this.message = "Path not found " + message;
    this.name = 'PathException';
}

function _genID(length) {
    let result           = '';
    let characters       = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789';
    let charactersLength = characters.length;
    for ( let i = 0; i < length; i++ ) {
          result += characters.charAt(Math.floor(Math.random() * charactersLength));
    }
    return result;
}

function FileMeta() {
    this.mtime = Date.now();
    this.isfolder = true;
    this.uid = '';
    this.share = false;
    this.fid = '';
}


function SwiftFilesystem() {

    this.backend = undefined;

    this.metaCache = {};

    this.fileCache = new Dexie("SwiftFileCache");

    this.fileCache.version(1).stores({
        file: '&id'
    });

    this.workingDir = "";

    this.init = async function () {
        if (window.oauth_type === "dropbox") {
            this.backend = new DropboxBackend();
            console.log("Using Dropbox backend");
        } else if (window.oauth_type === "google") {
            this.backend = new GoogleBackend();
            console.log("Using GDrive backend");
        } else {
            this.backend = new LocalBackend();
            console.log("Using Local backend");
        }
        await this.backend.init();
    };

    //Convert Path into InternalPath with the help of workingDir
    this.prepPath = function (path) {
        let tmp = this.workingDir + "/" + path;
        tmp = normalizePath(tmp);
        return tmp;
    };

    //This method receives a path and return the underlying object meta.
    this._lookup = async function (normPath) {
        if (!('/' in this.metaCache)) {
            this.metaCache['/'] = new FileMeta();
        }

        //If the meta exists in the cache, return it directly.
        if (normPath in this.metaCache) {
            //console.log("Head item use " + normPath);
            return this.metaCache[normPath];
        } else {
            let item = await this.backend.headItem(normPath);
            //console.log("Head item " + normPath);
            this.metaCache[normPath] = item;
            return item;
        }
    };


    this.readdir = async function (path, recursive) {

        let normPath = this.prepPath(path);
        let entries = await this.backend.listItems(normPath, recursive);
        let results = [];
        for (let i = 0; i < entries.length; i++) {
            let entry = entries[i];
            let entryid = entry.id;
            let entrymeta = entry.meta;
            this.metaCache[entryid] = entrymeta;

            //Copy one.
            let tmp = {};
            let exposedPath = entryid.substr(this.workingDir.length);
            if (exposedPath === "") continue;
            let exposedParentPath = dirname(exposedPath);
            let exposedName = basename(exposedPath);
            tmp.path = exposedPath;
            tmp.parent = exposedParentPath;
            tmp.mtime = entrymeta.mtime;
            tmp.name = exposedName;
            tmp.uid = entrymeta.uid;
            tmp.isfolder = entrymeta.isfolder;
            tmp.share = entrymeta.share;
            tmp.fid = entrymeta.fid;
            results.push(tmp);

        }
        //console.log(results);
        return results;
    };


    this.mkdir = async function (path) {
        let normPath = this.prepPath(path);
        let metaTry = await this._lookup(normPath);
        if (metaTry == null) {
            let parentPath = dirname(normPath);
            let parent_meta = await this._lookup(parentPath);
            if (parent_meta == null) throw new PathException(parentPath);
            let meta = new FileMeta();
            meta.isfolder = true;
            await this.backend.setItem(normPath, meta, ''); //This may change meta info
            //Cache 
            this.metaCache[normPath] = meta;
        }
    };

    this._cacheFile = async function (path, mtime, data) {
        if (!this.backend.canCache()) return;
        let cacheBlob = data;
        if (!(data instanceof Blob)) {
            cacheBlob = new Blob([data]);
        }
        let c = await this.fileCache.file.count();
        if (c > 256) {
            await this.fileCache.file.clear();
        }
        this.fileCache.file.put({'id': path, 'mtime': mtime, 'data': cacheBlob});
        console.log("Caching " + path + " mtime: " + mtime);
    };

    this._retrieveCacheFile = async function (path, mtime) {
        if (!this.backend.canCache()) return;
        let result = await this.fileCache.file.get(path);
        if (result === undefined) {
            return null;
        }
        if (result.mtime !== mtime) {
            console.log("Cache expire " + path + " " + mtime + " original " + result.mtime);
            await this.fileCache.file.delete(path);
            return null;
        }
        console.log("Using cache for " + path);
        return result.data;
    };

    this.updateFile = async function (path, data, isRaw) {
        let normPath = this.prepPath(path);

        //This is only for update mtime, other wise we could optimize it
        let parentPath = dirname(normPath);
        let parent_meta = await this._lookup(parentPath);
        if (parent_meta == null) throw new PathException(parentPath);

        let meta = await this._lookup(normPath);
        if (meta == null) {
            meta = new FileMeta();
            meta.isfolder = false;
        } else {
            if (meta.isfolder) {
                throw new PathException(normPath);
            }
            meta.mtime = Date.now();
        }

        await this._cacheFile(normPath, meta.mtime, data);

        await this.backend.setItem(normPath, meta, data); //This may change meta info

        if (parentPath !== "/") {
            parent_meta.mtime = Date.now();
            await this.backend.setItem(parentPath, parent_meta, undefined);
        }

        this.metaCache[normPath] = meta;
    };

    this.getFile = async function (path, isRaw, rev) {
        let normPath = this.prepPath(path);
        let meta = await this._lookup(normPath);
        if (meta == null || meta.isfolder) {
            throw new PathException(normPath);
        }

        let fileBlob = null;
        if (rev === undefined) {
            fileBlob = await this._retrieveCacheFile(normPath, meta.mtime);
            if (fileBlob == null) {
                fileBlob = await this.backend.getItem(normPath);
                if (!(fileBlob == null)) {
                    await this._cacheFile(normPath, meta.mtime, fileBlob);
                }
            }
        } else {
            fileBlob = await this.backend.getItem(normPath, rev);
        }

        if (fileBlob == null) {
            throw new PathException(normPath);
        }
        if (isRaw) {
            let untypedArray = await new Response(fileBlob).arrayBuffer();
            return new Uint8Array(untypedArray);
        } else {
            let textRes = await new Response(fileBlob).text();
            return textRes;
        }
    };

    this.rename = async function (oldpath, newpath) {

        let normOldPath = this.prepPath(oldpath);
        let normNewPath = this.prepPath(newpath);
        if (dirname(normOldPath) !== dirname(normNewPath)) {
            console.error("Parent path do not match");
            return;
        }
        let oldmeta = await this._lookup(normOldPath);
        if (oldmeta == null) {
            throw new PathException(normOldPath);
        }

        let newmeta = await this._lookup(normNewPath);
        if (!(newmeta == null)) {
            return;
        }

        await this.backend.renameItem(normOldPath, normNewPath);
        this.metaCache = {};
    };


    this.delete = async function (path) {
        let normPath = this.prepPath(path);
        if (normPath === "/") return;
        let meta = await this._lookup(normPath);
        if (meta == null) {
            throw new PathException(normPath);
        }
        await this.backend.removeItem(normPath);
        this.metaCache = {};
    };

    this.exists = async function (path) {
        let normPath = this.prepPath(path);
        let meta = await this._lookup(normPath);
        return meta != null;
    };


    this.getFileRevisionList = async function (path) {
        let normPath = this.prepPath(path);
        let meta = await this._lookup(normPath);
        if (meta == null || meta.isfolder) {
            throw new PathException(normPath);
        } else {
            return this.backend.getFileRevisionList(normPath);
        }
    };

    this.share = async function (path, receiver) {
        let normPath = this.prepPath(path);
        if (normPath === "/") return;
        let meta = await this._lookup(normPath);

        if (meta == null || !meta.isfolder) {
            throw new PathException(normPath);
        } else {
            let res = await this.backend.share(normPath, receiver);
            if(res) {
                meta.share = true;
                await this.backend.setItem(normPath, meta, undefined);
            }
        }


    };

    this.cwd = function () {
        return this.workingDir;
    };

    this.chdir = function (dir) {
        this.workingDir = dir;
    };
}