

let suppressCursorReport = false;
let errorlinemarkerid = undefined;
let RangeClass = ace.require('ace/range').Range;
let newContentPending = false;
// Initialize Editor
const ed = ace.edit('editor');
ed.setFontSize("15px");

ed.setOption('printMargin', false);
ed.getSession().setMode("ace/mode/latex");
ed.getSession().setUseWrapMode(true);
ed.getSession().setUseSoftTabs(false);
ed.on('change', onChangeEventCB);
ed.getSelection().on('changeCursor', reportCursorToViewer);
ed.resize();

//We can optimize this
function getCurrentEditFileDependencyID() {
    //if(fileDependencyCache === undefined) return;
    if (currentEditorFileID === mainFileID) {
        return 1;
    } else {

        if (!(currentEditorFileID in fileDependencyCache)) {
            return 0;
        }
        return fileDependencyCache[currentEditorFileID];
    }
}

function getFileIDfromDependencyID(id) {
    if (id === 1) {
        return mainFileID;
    } else {
        for (let key in fileDependencyCache) {
            if (fileDependencyCache[key] === id) {
                return key;
            }
        }
    }
    return undefined;
}


function grabEditorCurrentState() {
    let anchorPos = ed.selection.getSelectionAnchor();
    anchorPos.fid = getCurrentEditFileDependencyID();
    let wholelinetxt = ed.session.getLine(anchorPos.row);
    return {'data': ed.session.getValue(), 'pos': anchorPos, 'context': wholelinetxt};
}


function partialStateCheck(e) {

    if (currentEditorFileID in checkpointPositionCache) {
        let checkpoint = checkpointPositionCache[currentEditorFileID];
        if (checkpoint !== 0) {
            const tmpPos = ed.session.doc.indexToPosition(checkpoint);
            if (e.start.row <= tmpPos.row) {
                canpartial = false;
            }
        }
        //console.log(checkpoint + " " + stindex);
    } else {

    }
}

function submitSharePadOp(e) {
    if (!window.shareEnabled) return;
    if (stopWatch) return;

    const op = [];
    const stindex = ed.session.doc.positionToIndex(e.start);
    //const edindex = ed.session.doc.positionToIndex(e.end);

    op.push(stindex);
    if (e.action === 'insert')
    {
        op.push(e.lines.join(ed.session.doc.getNewLineCharacter()))
    }
    else if (e.action === 'remove')
    {
        const delCt = e.lines.join(ed.session.doc.getNewLineCharacter()).length;
        op.push({d: delCt});
    }
    console.log("submiting op");
    sharedoc.submitOp(op);
}

function emitPreviewEvent(e) {
    let isLocalChange = !stopWatch;
    // if(isFireBaseInit) {
    //     isLocalChange = !firepad.editorAdapter_.ignoreChanges;
    // }
    if (isLocalChange) {
        if (e.lines.length === 1 && e.lines[0].length === 1) {
            //console.log(e.action);
            if (e.action === "insert") {
                // let event = {'cmd':'editorinsertchar', 'data': e.lines[0]};
                // //console.log("inserting " + e.lines[0]);
                // ide.viewer.postMessage(event);
                handle_editor_char_insert(e.lines[0]);
            } else if (e.action === "remove") {
                handle_editor_char_delete(e.lines[0]);
                // let event = {'cmd':'editordeletechar', 'data': e.lines[0]};
                // ide.viewer.postMessage(event);
            }
        } else {
            //console.error("Ignore");
        }
    }

}


function onChangeEventCB(e) {

    submitSharePadOp(e);

    partialStateCheck(e);

    emitPreviewEvent(e);


    newContentPending = true;
}

function reportCursorToViewer() {
    if (suppressCursorReport) {
        //console.log("suppress");
        suppressCursorReport = false;
        return;
    }
    let tmpid = getCurrentEditFileDependencyID();

    if (tmpid === 0) {
        return 0; //Dont report.
    }
    let anchorPos = ed.selection.getSelectionAnchor();
    anchorPos.fid = tmpid;
    handle_editor_move_anchor(anchorPos, false);
    //
    // ide.viewer.postMessage({'cmd':'editormoveanchor', 'data':anchorPos});
}


function removeEditorErrorLineMarker() {
    if (errorlinemarkerid !== undefined) {
        ed.session.removeMarker(errorlinemarkerid);
        errorlinemarkerid = undefined;
    }
}


function addEditorErrorLineMarker(errorline) {

    if (errorlinemarkerid !== undefined) {
        removeEditorErrorLineMarker();
    }
    errorlinemarkerid = ed.session.addMarker(new RangeClass(errorline, 0, errorline, 1), "errorLineMarker", "fullLine");
    ed.scrollToLine(errorline, true, true, function () {
    });
}

function handleViewerAnchorMoved(input) {

    let filedepID = input.file;
    let cuurentDepId = getCurrentEditFileDependencyID();
    // console.log(input );
    // console.log(cuurentDepId);
    if (filedepID === cuurentDepId) {
        suppressCursorReport = true;
        let chartype = input.type;

        ed.scrollToLine(input.row, true, true, function () {
        });
        if (chartype === "normal" || chartype === "command") {

            ed.gotoLine(input.row, input.column, true);
        } else if (chartype === "spacebefore") {
            ed.gotoLine(input.row, input.column - 1, true);
        } else if (chartype === "spaceafter") {
            ed.gotoLine(input.row, input.column - 1, true);
            ed.navigateWordRight();
        }
        ed.focus();
    } else {
        let toFetchFileID = getFileIDfromDependencyID(filedepID);
        if (toFetchFileID === undefined) {

        } else {
            loadSelectedSourceFile(toFetchFileID, input);
        }
    }

}


function setEditorInitAnchor(input) {
    ed.scrollToLine(input.row, true, true, function () {
    });
    ed.gotoLine(input.row, input.column, true);
    ed.focus();
}


let sharews = undefined;
let shareconn = undefined;
let sharedoc = undefined;
let stopWatch = false;

async function setupEditor(src, filename) {


    if (window.shareEnabled) {
        if (sharews === undefined) {
            try {
                await initShareDBConn();
                console.log("Connected to sharedb");
            } catch (e) {
                console.log("Unable to connect to sharedb " + e);
                window.shareEnabled = false;
            }
        }
        if (sharedoc !== undefined) {
            sharedoc.destroy();
            sharedoc = undefined;
        }
    }


    if (window.shareEnabled) {
        return new Promise((resolve, reject) => {
            sharedoc = shareconn.get(window.shareId, filename);
            //console.log(sharedoc);
            sharedoc.fetch(e => {
                if (e !== undefined) {
                    return reject(e);
                }
                if (sharedoc.version === 0) {
                    sharedoc.create(src, 'text');
                }
                sharedoc.subscribe(e => {
                    stopWatch = true;
                    ed.setValue(sharedoc.data);
                    ed.moveCursorTo(0, 0);
                    ed.focus();
                    ed.resize();
                    stopWatch = false;
                    resolve();
                });

                sharedoc.on('op', _handleOp);
            });
        });
    } else {
        ed.setValue(src, 1);
        ed.resize();
    }
}


function initShareDBConn() {
    sharews = new ReconnectingWebSocket(`wss://www.swiftlatex.com/pad/${window.username}/${window.access_token}/${window.expiry}/${window.jwt_token}`, null, {
        debug: false,
        automaticOpen: false
    });

    return new Promise((resolve, reject) => {
        sharews.onerror = function () {
            console.error("Unable to connect to share remote, disable");
            sharews.close();
            window.shareEnabled = false;
            return reject();
        };
        sharews.onopen = function () {
            sharews.onerror = function (e) {
            };
            sharews.onopen = function (e) {
            };
            shareconn = new ShareDB.Connection(sharews);
            return resolve();
        };
        sharews.open();
    });
}

function _handleOp(op, mime) {
    if (mime) return;
    let posIndex = 0;
    for(let i = 0; i < op.length; i ++) {
        let data = op[i];

        if((typeof data) === "number") {
            posIndex += data;
        }
        else if((typeof data) === "string") {
            const pos = ed.session.doc.indexToPosition(posIndex);
            stopWatch = true;
            ed.session.insert(pos, data);
            stopWatch = false;
            posIndex += data.length;
        } else {
            const delCt = data.d;
            const stPos = ed.session.doc.indexToPosition(posIndex);
            const edPos = ed.session.doc.indexToPosition(posIndex + delCt);
            const range = {start: stPos, end: edPos};
            stopWatch = true;
            ed.session.remove(range);
            stopWatch = false;
        }

    }



}