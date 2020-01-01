function LocalBackend() {
    this.db = new Dexie("localFS");
    this.db.version(1).stores({
        pool: '&id'
    });

    this.canCache = function () {
        return false;
    };

    this.init = async function () {

    };

    this.setItem = async function (path, meta, data) {
        console.log("setItem " + path);
        if (data === undefined) {
            return this.db.pool.update(path, {'meta': meta})
        } else {
            let tmpBlob = new Blob([data]);
            return this.db.pool.put({'id': path, 'meta': meta, 'data': tmpBlob})
        }

    };

    this.getItem = async function (path) {
        let tmp = await this.db.pool.get(path);
        if (tmp == null) return null;
        return tmp.data;
    };

    this.headItem = async function (path) {
        let tmp = await this.db.pool.get(path);
        if (tmp == null) return null;
        return tmp.meta;
    };

    this.listItems = async function (path, recursive) {
        console.log("listItem " + path);

        if (!path.endsWith("/")) path += "/";
        if (recursive) {
            return await this.db.pool.where('id').startsWith(path).toArray();
        } else {
            return await this.db.pool.where('id').startsWith(path).filter(e => {
                if (e.id.substr(path.length).indexOf("/") === -1) {
                    return true;
                }
                return false;
            }).toArray();
        }

    };

    this.renameItem = async function (oldPath, newPath) {
        let entry = await this.db.pool.get(oldPath);
        if (entry == null) return null;
        let taskList = [oldPath];
        if (entry.meta.isfolder) {
            let todo = await this.db.pool.where('id').startsWith(oldPath + "/").toArray();
            for (let j = 0; j < todo.length; j++) {
                taskList.push(todo[j].id);
            }
        }
        for (let i = 0; i < taskList.length; i++) {
            let task = taskList[i];
            let dest = task.replace(oldPath, newPath);
            let tmp = await this.db.pool.get(task);
            let meta = tmp.meta;
            let data = tmp.data;
            await this.db.pool.put({'id': dest, 'meta': meta, 'data': data});
            await this.db.pool.delete(task);
        }
    };

    this.removeItem = async function (path) {
        let tmp = await this.db.pool.get(path);
        if (tmp == null) return null;
        if (tmp.meta.isfolder) {
            await this.db.pool.where('id').startsWith(path + "/").delete();
        }
        await this.db.pool.delete(path);

    };

    this.getFileRevisionList = async function (normPath) {
        return [];
    };

    this.share = async function (normPath) {
        return false;
    };
}