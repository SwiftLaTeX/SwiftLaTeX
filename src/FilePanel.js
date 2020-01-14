import React from 'react'
import Moment from 'moment'
import RemoteStorage from 'remotestoragejs'
import FileBrowser, {Icons}  from 'react-keyed-file-browser'
import 'react-keyed-file-browser/dist/react-keyed-file-browser.css';
import 'font-awesome/css/font-awesome.min.css';
export default class FilePanel extends React.Component {
    remoteStorageHandle = undefined;
    remoteStorageBaseClient = undefined;
    state = {
        files: [
        ]
    };


    componentDidMount() {
        this.remoteStorageHandle = new RemoteStorage({logging: false});
        this.remoteStorageHandle.access.claim('*', 'rw');
        this.remoteStorageHandle.caching.enable('/');
        this.remoteStorageBaseClient = this.remoteStorageHandle.scope('/foo/');
        this.showFiles().then(_=>{});
    }

    async showFiles() {
        let res = await this._listFiles("");
        let files = res.map((k)=>{
           return {key: k, size: 0, modified: 0};
        });
        this.setState({files: files});
    }

    async _listFiles(url){
        let res = [];
        let files = await this.remoteStorageBaseClient.getListing(url);
        for(let file in files) {
            if(file.endsWith("/")) {
                let subRes = await this._listFiles(file);
                res = res.concat(subRes);
            } else {
                res.push(url + file);
            }
        }
        return res;
    }

    handleCreateFolder = (key) => {
        this.remoteStorageBaseClient.storeFile('text/plain', key + 'Thumbs.db', key)
            .then(() => {
                this.showFiles().then(_=>{});
            });
    };

    handleCreateFiles = (files, prefix) => {
        this.setState(state => {
            const newFiles = files.map((file) => {
                let newKey = prefix;
                if (prefix !== '' && prefix.substring(prefix.length - 1, prefix.length) !== '/') {
                    newKey += '/'
                }
                newKey += file.name;
                return {
                    key: newKey,
                    size: file.size,
                    modified: +Moment(),
                }
            });

            const uniqueNewFiles = [];
            newFiles.map((newFile) => {
                let exists = false;
                state.files.map((existingFile) => {
                    if (existingFile.key === newFile.key) {
                        exists = true
                    }
                    return true;
                });
                if (!exists) {
                    uniqueNewFiles.push(newFile)
                }
                return true;
            });
            state.files = state.files.concat(uniqueNewFiles);
            return state
        })
    };
    handleRenameFolder = (oldKey, newKey) => {
        this.setState(state => {
            const newFiles = [];
            state.files.map((file) => {
                if (file.key.substr(0, oldKey.length) === oldKey) {
                    newFiles.push({
                        ...file,
                        key: file.key.replace(oldKey, newKey),
                        modified: +Moment(),
                    })
                } else {
                    newFiles.push(file)
                }
                return true;
            });
            state.files = newFiles;
            return state
        })
    };
    handleRenameFile = (oldKey, newKey) => {
        this.setState(state => {
            const newFiles = [];
            state.files.map((file) => {
                if (file.key === oldKey) {
                    newFiles.push({
                        ...file,
                        key: newKey,
                        modified: +Moment(),
                    })
                } else {
                    newFiles.push(file)
                }
                return true;
            });
            state.files = newFiles;
            return state
        })
    };
    handleDeleteFolder = (folderKey) => {
        this.setState(state => {
            const newFiles = [];
            state.files.map((file) => {
                if (file.key.substr(0, folderKey.length) !== folderKey) {
                    newFiles.push(file)
                }
                return true;
            });
            state.files = newFiles;
            return state
        })
    };
    handleDeleteFile = (fileKey) => {
        this.setState(state => {
            const newFiles = [];
            state.files.map((file) => {
                if (file.key !== fileKey) {
                    newFiles.push(file)
                }
                return true;
            });
            state.files = newFiles;
            return state
        })
    };

    render() {
        return (
            <FileBrowser
                files={this.state.files}
                icons={Icons.FontAwesome(4)}
                onCreateFolder={this.handleCreateFolder}
                onCreateFiles={this.handleCreateFiles}
                onMoveFolder={this.handleRenameFolder}
                onMoveFile={this.handleRenameFile}
                onRenameFolder={this.handleRenameFolder}
                onRenameFile={this.handleRenameFile}
                onDeleteFolder={this.handleDeleteFolder}
                onDeleteFile={this.handleDeleteFile}
                canFilter={false}
            />
        )
    }
}