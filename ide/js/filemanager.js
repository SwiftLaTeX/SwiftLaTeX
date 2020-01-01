

const ROOTNODENAME = "Files/";
let isFileTreeInited = false;
const FILESIZELIMIT = 2 * 1024 * 1024;


function isDownloadFileAvailableforMenu($node) {
    if (status !== "idle") return true;


    if ($node.text.endsWith("/"))
        return true;

    return false;
}

function isSelectAsMainFileAvailableforMenu($node) {
    if (status !== "idle") return true;


    if ($node.parent !== "/")
        return true;

    if ($node.id === mainFileID)
        return true;

    if ($node.text.endsWith(".tex"))
        return false;

    return true;
}

function selectAsMainFile($node) {
    if (confirm("Are you going to change the main file?")) {
        window.localStorage.setItem("mainfile" + gpid, $node.id);
        window.location.reload();
    }
    return true;
}

function isUploadZipAvailableforMenu($node) {
    if (status !== "idle") return true;

    if ($node.text === ROOTNODENAME)
        return false;

    return true;
}


function isRenameFileAvailableforMenu($node) {
    if (status !== "idle") return true;

    if ($node.text === ROOTNODENAME)
        return true;

    if ($node.id === mainFileID || $node.id === currentEditorFileID)
        return true;

    if (currentEditorFileID.startsWith($node.id))
        return true;

    return false;
}

function isRemoveFileAvailableforMenu($node) {
    if (status !== "idle") return true;

    if ($node.text === ROOTNODENAME)
        return true;

    if ($node.id === mainFileID || $node.id === currentEditorFileID)
        return true;

    if (currentEditorFileID.startsWith($node.id))
        return true;


    return false;
}

function isCreateFileAvailableforMenu($node) {
    if (status !== "idle") return true;
    let nodeText = $node.text;
    return !(nodeText === ROOTNODENAME || nodeText.endsWith("/"));
}


async function fileManagerCreateFile($node) {
    if (status !== "idle") return false;
    let newfile = prompt("Create a new source file? Allowed Formats:[.tex, .bib, .txt, .bst., cls]");
    if (newfile === null || newfile === "") return false;
    if (!(newfile.endsWith(".tex") || newfile.endsWith(".txt") || newfile.endsWith(".bib") || newfile.endsWith(".bst") || newfile.endsWith(".cls"))) {
        alert("Source files must end with the following extension [.tex, .bib, .txt, .bst, .cls]");
        return false;
    }
    let filename = sanitizeFileName(newfile);

    let isthere = await localFS.exists($node.id + "/" + filename);
    if (isthere) {
        return;
    }
    status = "busy";
    NProgress.start();
    try {
        await localFS.updateFile($node.id + "/" + filename, "");
        await refreshTreeContent(); //A bit detour
    } catch (e) {
        alert("Unable to create a new file. " + e);
    }


    return true;

}

function simplifySingleRootZip(queues) {
    let singleRootStructure = true;
    //Let's check whether we need to
    if (queues.length > 1 && queues[0][2] === true) {
        let rootPath = queues[0][0];
        for (let i = 1; i < queues.length; i++) {
            if (!queues[i][0].startsWith(rootPath)) {
                singleRootStructure = false;
                break;
            }
        }
    } else {
        singleRootStructure = false;
    }

    if (singleRootStructure) {
        let rootentry = queues.shift();
        queues.forEach(entry => {
            //console.log("Before simplifySingleRootZip " + entry[0]);
            entry[0] = entry[0].substr(rootentry[0].length + 1);
            //console.log("After simplifySingleRootZip " + entry[0]);
        });
    }
}

async function _handleZip(zip) {
    const allowed_extension = ['.tex', '.bib', '.bst', '.cls', '.pdf', '.jpg', '.png', '.txt', '.bbx', 'sty'];
    let queues = [];
    zip.forEach(function (relativePath, zipEntry) {
        if (zipEntry.size > FILESIZELIMIT) {
            return;
        }
        if (zipEntry.dir) {
            queues.push([relativePath.slice(0, -1), zipEntry, true]);
        } else {
            let legalExtension = false;
            allowed_extension.some(ext => {
                if (zipEntry.name.endsWith(ext)) {
                    legalExtension = true;
                    return true;
                }
            });
            if (!legalExtension) {
                return;
            }
            queues.push([relativePath, zipEntry, false]);
        }
    });
    simplifySingleRootZip(queues);


    NProgress.start();
    status = "busy";

    for (let j = 0; j < queues.length; j++) {
        updateStatusBar("Uploading the file " + queues[j][0]);
        if (queues[j][2]) {
            await localFS.mkdir(queues[j][0]);
        } else {
            let binaryStr = await queues[j][1].async("uint8array");
            await localFS.updateFile(queues[j][0], binaryStr, true);
        }
    }
    //await localFS.uploadZipEntries(gpid, queues);
    await refreshTreeContent();
    await loadSelectedSourceFile(mainFileID);
    updateStatusBar("Upload task finished");
}

function fileManagerUploadZip($node) {
    if (status !== "idle") return false;
    $("#zipuploader").off("change");
    $("#zipuploader").change(function (e) {
        $("#zipuploader").off("change");
        if (this.files.length > 0) {
            let fileHandle = this.files[0];
            if (fileHandle.size > FILESIZELIMIT * 3) {
                alert("This zip file size is too big.");
                return;
            }

            JSZip.loadAsync(fileHandle).then(zip => {
                _handleZip(zip)
            }).catch(e => {
                alert("Corrupted Zip");
            });
        }
    });
    $("#zipuploader").trigger("click");
}

function _blobDownload(tmpblob, name) {
    let tmpurl = window.URL.createObjectURL(tmpblob);
    let link = document.createElement('a');
    link.href = tmpurl;
    link.setAttribute('download', name);
    document.body.appendChild(link);
    link.click();
    document.body.removeChild(link);
    setTimeout(function () {
        window.URL.revokeObjectURL(tmpurl)
    }, 10000);
}

async function fileManagerDownloadFile($node) {
    if (status !== "idle") return false;
    let uint8array = await localFS.getFile($node.id, true);
    let tmpblob = new Blob([uint8array]);
    _blobDownload(tmpblob, $node.text);
}

async function refreshTreeContent() {
    canpartial = false;
    try {
        cachedFileList = await localFS.readdir("", true);
        flushCache();
        await preloadProjectFilesToMemFS();
        setupFileTreeUI(cachedFileList);
        NProgress.done();
        status = "idle";
    } catch (e) {
        alert("Error refresh the file tree " + e);
        NProgress.done();
        status = "idle";

    }
    //_trySync();
}

function fileManagerUploadFiles($node) {
    if (status !== "idle") return false;
    $("#fileuploader").off("change");
    $("#fileuploader").change(async function (e) {
        $("#fileuploader").off("change");

        status = "busy";
        NProgress.start();

        for (let j = 0; j < this.files.length; j++) {

            let fileHandle = this.files[j];
            if (fileHandle.size > FILESIZELIMIT) {
                console.log("File too big, ignore");
                alert("Ignored an oversize file " + fileHandle.name);
                continue;
            }


            try {
                console.log("Uploading " + fileHandle.name);
                let ab = await new Response(fileHandle).arrayBuffer();
                let uint8array = new Uint8Array(ab);
                await localFS.updateFile($node.id + "/" + fileHandle.name, uint8array, true);
            } catch (error) {
                console.error("Unable to upload " + fileHandle.name + error);
            }
        }
        await refreshTreeContent();
        await loadSelectedSourceFile(mainFileID);
    });
    $("#fileuploader").trigger("click");


}


async function fileManageRenameFile($node) {
    if (status !== "idle") return false;
    let newfile = prompt("Rename this file?");
    if (newfile === null || newfile === "") return false;

    let filename = sanitizeFileName(newfile);
    let isthere = await localFS.exists($node.id + "/" + filename);
    if (isthere) {
        return;
    }
    status = "busy";
    NProgress.start();
    try {
        await localFS.rename($node.id, $node.parent + "/" + filename);
    } catch (e) {
        alert("Error detected when renaming files " + e);
    }

    await refreshTreeContent();

    return true;
}


async function fileManagerCreateFolder($node) {
    if (status !== "idle") return false;
    let newfile = prompt("Create a new folder");
    if (newfile === null) return false;
    let filename = sanitizeFileName(newfile);
    let isthere = await localFS.exists($node.id + "/" + filename);
    if (isthere) {
        return;
    }
    status = "busy";
    NProgress.start();


    try {
        await localFS.mkdir($node.id + "/" + filename);
    } catch (e) {
        alert("Unable to create a new folder. " + e);
    }

    await refreshTreeContent();

    return true;
}

async function fileManagerRemoveFile($node) {
    if (status !== "idle") return false;
    let yesorno = confirm("Delete this file?");
    if (!yesorno) return false;
    status = "busy";
    NProgress.start();
    try {
        await localFS.delete($node.id);
    } catch (e) {
        alert("Unable to delete this file. " + e);
    }

    await refreshTreeContent();

    return true;
}

async function fileManagerExportZip() {
    if (status !== "idle") return false;
    status = "busy";
    NProgress.start();
    let zip = new JSZip();
    for (let i = 0; i < cachedFileList.length; i++) {
        let entry = cachedFileList[i];
        if (entry.isfolder) {
            continue;
        } else {
            let uint8blob = await localFS.getFile(entry.path, true);
            zip.file(entry.path, uint8blob);
        }
    }
    let zipblob = await zip.generateAsync({type: "blob"});
    _blobDownload(zipblob, "export.zip");
    status = "idle";
    NProgress.done();
    return true;
}


function prepareJsonData4Tree(tree) {
    let tmpData = [];

    tree.forEach(function (entry) {
        let nodename = entry.name;
        if (entry.isfolder) {
            nodename += "/";
        }

        tmpData.push({'id': entry.path, 'parent': entry.parent, 'text': nodename, "icon": entry.isfolder});
    });

    tmpData.push({
        'id': "/", 'parent': "#", 'text': ROOTNODENAME, "icon": true,
        state: {
            opened: true
        }
    });
    //console.log(tmpData);

    return tmpData;
}


function setupFileTreeUI(treedata) {

    let tmpData = prepareJsonData4Tree(treedata);

    if (isFileTreeInited) {
        $('#fileTreeView').jstree(true).settings.core.data = tmpData;
        $('#fileTreeView').jstree(true).refresh();
        return;
    }

    isFileTreeInited = true;

    let contextMenu = {
        "items": function ($node) {
            // var tree = $("#tree").jstree(true);
            return {

                "Create File": {
                    "separator_before": false,
                    "separator_after": false,
                    "label": "Create Source File",
                    "_disabled": function (obj) {
                        return isCreateFileAvailableforMenu($node)
                    },
                    "action": function (obj) {
                        return fileManagerCreateFile($node)
                    }
                },
                "Upload Files": {
                    "separator_before": false,
                    "separator_after": false,
                    "label": "Upload Files",
                    "_disabled": function (obj) {
                        return isCreateFileAvailableforMenu($node)
                    },
                    "action": function (obj) {
                        return fileManagerUploadFiles($node)
                    }
                },
                "Upload Zip File": {
                    "separator_before": false,
                    "separator_after": false,
                    "label": "Upload a Zip",
                    "_disabled": function (obj) {
                        return isUploadZipAvailableforMenu($node)
                    },
                    "action": function (obj) {
                        return fileManagerUploadZip($node)
                    }
                },
                "New Folder": {
                    "separator_before": false,
                    "separator_after": false,
                    "label": "New Folder",
                    "_disabled": function (obj) {
                        return isCreateFileAvailableforMenu($node)
                    },
                    "action": function (obj) {
                        return fileManagerCreateFolder($node)
                    }
                },
                "Rename": {
                    "separator_before": true,
                    "separator_after": false,
                    "label": "Rename",
                    "_disabled": function (obj) {
                        return isRenameFileAvailableforMenu($node)
                    },
                    "action": function (obj) {
                        return fileManageRenameFile($node);
                    }
                },
                "Remove": {
                    "separator_before": false,
                    "separator_after": false,
                    "label": "Remove",
                    "_disabled": function (obj) {
                        return isRemoveFileAvailableforMenu($node)
                    },
                    "action": function (obj) {
                        return fileManagerRemoveFile($node);
                    }
                },
                "Download File": {
                    "separator_before": false,
                    "separator_after": false,
                    "label": "Download",
                    "_disabled": function (obj) {
                        return isDownloadFileAvailableforMenu($node)
                    },
                    "action": function (obj) {
                        return fileManagerDownloadFile($node);
                    }
                },
                "Select as Main File": {
                    "separator_before": true,
                    "separator_after": false,
                    "label": "Select as Main File",
                    "_disabled": function (obj) {
                        return isSelectAsMainFileAvailableforMenu($node)
                    },
                    "action": function (obj) {
                        return selectAsMainFile($node);
                    }
                }
            };
        }
    };

    $('#fileTreeView').on("select_node.jstree", function (node, data) {
        if (data !== undefined && data.event !== undefined && data.event.type === "click") {
            if (!data.node.text.endsWith("/")) {
                loadSelectedSourceFile(data.node.id);
            }
        }

    });

    $('#fileTreeView').jstree({
        "plugins": ["contextmenu"],
        "contextmenu": contextMenu,
        multiple: false,
        "core": {
            "data": tmpData
        }
    });
}
