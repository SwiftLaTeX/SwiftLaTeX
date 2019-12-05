
let status = "init";
NProgress.configure({showSpinner: false});

let editor_viewer_splitor_instance = Split(['#viewerrow', '#editorrow'], {
    sizes: [85, 15],
    direction: 'vertical',
    cursor: 'row-resize',
    gutterSize: 10,
    onDragEnd: function () {
        ed.resize();
    }
});


const urlParams = new URLSearchParams(window.location.search);
if (!urlParams.has('pid')) {
    window.location.href = '/';
}
window.gpid = urlParams.get('pid');
window.shareEnabled = urlParams.get('share') === "true";
window.shareId = urlParams.get('share_id');


//UI Stuff
let mainFileID = undefined;
let currentEditorFileID = undefined;
let last_log_string = undefined;
let last_compile_is_partial = false;
let last_pdf_document = undefined;
let total_page_number = 0;
let current_page = 0;
let canpartial = false;
let lastEditorState = undefined;
let globalFontCache = {};
let fileDependencyCache = {};
let checkpointPositionCache = {};
let cachedFileList = undefined;
let writeLock = false;

// let bgSyncStatus = "idle";


function fatal_error(reason) {
    status = "error";
    $('#alert-modal-body').html("<p>" + reason + "</p>");
    $('#alert-modal').modal('show');
}

let menuData = [
    {
        'nav': 'File', 'items': [
            {'item': 'New Source file', 'id': 'newsourcefilebtn'},
            {'item': 'Change Main File', 'id': 'changemainbtn'},
            {'item': 'divider', 'id': '#'},
            {'item': 'Upload Zip', 'id': 'uploadzipbtn'},
            {'item': 'Upload Files', 'id': 'uploadfilebtn'},
            {'item': 'divider', 'id': '#'},
            {'item': 'Export PDF', 'id': 'exportpdfbtn'},
            {'item': 'Export ZIP', 'id': 'exportzipbtn'},
            {'item': 'divider', 'id': '#'},
            {'item': 'Exit', 'id': 'exitbtn', 'href': 'index.html'}
        ]
    },
    {
        'nav': 'Edit', 'items': [
            {'item': 'Redo', 'id': 'redobtn'},
            {'item': 'Undo', 'id': 'undobtn'},
            {'item': 'divider', 'id': '#'},
            {'item': 'Search', 'id': 'searchbtn'},
            {'item': 'Replace', 'id': 'replacebtn'}
        ]
    },
    {
        'nav': 'View', 'items': [
            {'item': 'Zoom In', 'id': 'zoominbtn'},
            {'item': 'Zoom Out', 'id': 'zoomoutbtn'},
            {'item': 'divider', 'id': '#'},
            // {'item':'Increase Font Size', 'id':'editorfontsizebtn'},
            // {'item':'Change Editor Theme', 'id':'editorthemebtn'},
            {'item': 'Toggle File Tree', 'id': 'filetreebtn'}
        ]
    },
    {
        'nav': 'Build', 'items': [
            {'item': 'Compile', 'id': 'refreshbtn'},
            {'item': 'Check LaTeX Log', 'id': 'checklogbtn'},
            {'item': 'divider', 'id': '#'},
            {'item': 'File History', 'id': 'historyviewbtn'}
        ]
    },
    {
        'nav': 'Help', 'items': [
            {
                'item': 'Submit a bug report',
                'id': 'submitbugbtn',
                'href': 'https://github.com/SwiftLaTeX/SwiftLaTeX/issues'
            },
            {'item': 'About', 'id': 'aboutusbtn', 'href': '/contact.html'}
        ]
    }
];


function popupMenu() {
    for (let j = 0; j < menuData.length; j++) {
        let menu = menuData[j];
        let htmlData = '';
        htmlData += `<li class="nav-item dropdown">
            <a class="nav-link dropdown-toggle" href="#" role="button" data-toggle="dropdown" aria-haspopup="true" aria-expanded="false">${menu.nav}</a>
            <div class="dropdown-menu" aria-labelledby="navbarDropdown">`;
        for (let i = 0; i < menu.items.length; i++) {
            let item = menu.items[i];
            let href = "#";
            if ('href' in item) {
                href = item['href'];
            }
            if (item['id'] === '#') {
                htmlData += '<div class="dropdown-divider"></div>';
            } else {
                htmlData += `<a class="dropdown-item" href="${href}" id="${item.id}">${item.item}</a>`;
            }
        }
        htmlData += '</div></li>';
        $('#menubar').append(htmlData);
    }
    bindMenuBtnEvents();
}

function bindMenuBtnEvents() {
    $("#searchbtn").click(function () {
        if (status !== "idle") return;
        ed.execCommand("find");
    });

    $("#replacebtn").click(function () {
        if (status !== "idle") return;
        ed.execCommand("replace");
    });

    $("#redobtn").click(function () {
        if (status !== "idle") return;
        ed.execCommand("redo");
    });

    $("#historyviewbtn").click(function () {
        if (status !== "idle") return;
        if (window.oauth_type === "sandbox") {
            alert("This functionality is not available in offline mode");
            return;
        }
        showHistory();
    });


    $("#changemainbtn").click(function () {
        if (status !== "idle") return;
        //if (confirm("Are you going to change the main file?")) {
            window.localStorage.removeItem("mainfile" + gpid);
            window.location.reload();
        //}
    });

    $("#newsourcefilebtn").click(function () {
        if (status !== "idle") return;
        let selectedNode = {id: "/"}; //We make a node by ourselves
        fileManagerCreateFile(selectedNode);
    });

    $("#uploadzipbtn").click(function () {
        if (status !== "idle") return;
        let selectedNode = {id: "/"}; //We make a node by ourselves
        fileManagerUploadZip(selectedNode);
    });

    $("#uploadfilebtn").click(function () {
        if (status !== "idle") return;
        let selectedNode = {id: "/"}; //We make a node by ourselves
        fileManagerUploadFiles(selectedNode);
    });

    $("#undobtn").click(function () {
        if (status !== "idle") return;
        ed.execCommand("undo");
    });

    $("#filetreebtn").click(function () {
        if (status !== "idle") return;
        $("#fileTreeView").toggle();
    });


    $("#zoominbtn").click(zoomin_handler);

    $("#zoomoutbtn").click(zoomout_handler);

    $("#checklogbtn").click(function (e) {
        e.preventDefault();
        $('#logmodal').modal({
            keyboard: false
        });
        popupLogModal();

    });

    $("#exportpdfbtn").click(function (e) {
        e.preventDefault();
        if (status !== 'idle') return;
        if (mainFileID === undefined) return;
        window.open(`export.html?pid=${window.gpid}&mainfileid=${mainFileID}`);
    });

    $("#exportzipbtn").click(function (e) {
        e.preventDefault();
        fileManagerExportZip();
    });

    $("#refreshbtn").click(function (e) {
        e.preventDefault();
        if (status !== "idle") return;
        canpartial = false;
        newContentPending = true;
        
    });

}

function popupLogModal() {
    let logui = $("#logstatus");
    logui.html("");

    if (last_log_string !== undefined || last_log_string !== "") {
        let log_entries = LatexParserModule.parse(last_log_string, {ignoreDuplicates: true});
        for (let tmpid in log_entries.all) {
            let msg = `[${tmpid}]${log_entries.all[tmpid]['level']} in file ${log_entries.all[tmpid]['file']}:${log_entries.all[tmpid]['raw']}`;
            logui.prepend("<div>" + msg + "</div>");
        }
        if (logui.html() === "") {
            logui.html(`<pre>${last_log_string}</pre>`);
        }
    }
}

function displayPage() {
    if (last_compile_is_partial) {
        canpartial = false;
        newContentPending = true;
    } else {
        let htmldata = last_pdf_document.find('page').eq(current_page - 1).text();
        $("#pagecontrol").val(current_page + "/" + total_page_number);
        setViewerHtml(htmldata);
    }
}

$("#prevpagebtn").click(function (e) {
    canpartial = false;
    e.preventDefault();
    if (status === 'idle') {

        if (current_page > 1) {
            current_page = current_page - 1;
        } else {
            current_page = 1;
        }

        displayPage();
    }
});

$("#nextpagebtn").click(function (e) {
    canpartial = false;
    e.preventDefault();
    if (status === 'idle') {
        if (current_page + 1 <= total_page_number) {
            current_page += 1;
        }

        displayPage();
    }
});

$("#pagecontrol").change(function () {
    canpartial = false;
    let val = $(this).val();
    let isvaild = false;
    if (val.includes("/")) {
        val = val.split("/")[0];
    }


    let vv = parseInt(val);
    //console.log(vv);
    if (!isNaN(vv)) {
        if ((vv >= 1 || vv <= total_page_number) && vv !== current_page) {
            isvaild = true;
        }
    }

    if (status !== "idle") isvaild = false;

    if (isvaild) {
        displayPage();
    } else {
        $(this).val(current_page + "/" + total_page_number);
    }
});


function updateFileNameInStatusBar(name) {
    $("#fileStatus").html("&nbsp|&nbsp" + name.substr(1));
}

function updateStatusBar(statusStr) {
    $("#systemStatus").html(statusStr);
}

function showErrorInStatusBar(statusStr) {
    $("#errorStatus").html(statusStr);
}


//Network stuff
let localFS;


function fetchToken() {
    return new Promise((resolve, reject) => {
        $.getScript("/oauth/token", function (data, textStatus, jqxhr) {
            if (jqxhr.status !== 200) {
                return reject("Not able to obtain token");
            }
            return resolve();
        }).fail(function () {
            return reject("Not able to obtain token");
        });
    });
}

async function renewToken() {
    try {
        await fetchToken();
        //updateLaTeXWorkerCred();
        setTimeout(renewToken, 300000);
    } catch (err) {
        console.error("Unable to refresh token " + err);

    }

}


function pickMainFile() {
    return new Promise((resolve, reject) => {
        $('#mainfilemodal').on('hidden.bs.modal', function (e) {
            let chosenID = $("#mainfileoptions").val();
            window.localStorage.setItem("mainfile" + gpid, chosenID);


            if (status === "init") {
                return resolve(chosenID);
            } else {
                window.location.reload();
                reject(); //Not necessary
            }
        });
        $("#mainfilemodal").modal({
            keyboard: false
        });
    });
}

function selectMainFilefromSourceList() {
    let texlist = [];
    cachedFileList.forEach(function (entry) {
        //console.log(entry);
        if (entry.isfolder) return;

        if (entry.path.substr(1).indexOf("/") !== -1) return; //We only allow

        if (!entry.name.endsWith(".tex")) return;
        $("#mainfileoptions").append(`<option value="${entry.path}">${entry.name}</option>`);
        texlist.push(entry);
    });

    if (texlist.length === 0) {
        return undefined;
    } else if (texlist.length === 1) {
        return texlist[0].path;
    } else {
        let last_chosen_file_id = window.localStorage.getItem("mainfile" + gpid);
        if (last_chosen_file_id !== null && last_chosen_file_id !== undefined) {
            return last_chosen_file_id;
        } else {
            return pickMainFile();
        }
    }
}


function loadFonts($fonts) {
    let needRebuild = false;
    $fonts.each(function () {
        let fontname = $(this).attr("name");
        let fontid = $(this).attr("num");
        if ((fontname in globalFontCache) && globalFontCache[fontname] !== fontid) {
            needRebuild = true;
            return false;
        }
    });
    if (needRebuild) {
        console.warn("Rebuilding font cache");
        $("#fontpool").html("");
        globalFontCache = {};
    }

    $fonts.each(function () {
        let fontname = $(this).attr("name");
        let fontid = $(this).attr("num");
        if (!(fontname in globalFontCache)) {
            globalFontCache[fontname] = fontid;
            $("#fontpool").prepend(`<style>@font-face{font-family:${fontname};src:url(https://www.swiftlatex.com/dl/font/${fontname}.ttf);}.ff${fontid}{font-family:${fontname};}</style>`);
        }
    });

}

function buildFileDependencyCache($files) {

    fileDependencyCache = {}; //Clear
    if (!last_compile_is_partial) {
        checkpointPositionCache = {};
    }
    $files.each(function () {

        let fileidsupply = $(this).attr("src");
        let filenum = parseInt($(this).attr("no"));
        let checkpoint = parseInt($(this).attr("checkpoint"));


        if (filenum === 1) {
            fileidsupply = mainFileID;
        } else {
            fileidsupply = "/" + fileidsupply;
        }
        //console.log(fileidsupply  + " " +filenum + " " + checkpoint);
        //console.log(fileidsupply + " " + checkpoint);
        fileDependencyCache[fileidsupply] = filenum;
        if (checkpoint !== 0) {
            checkpointPositionCache[fileidsupply] = checkpoint;
        }

    });


    //console.log(fileDependencyCache);
}

function handleSuccessfulCompilation() {
    removeEditorErrorLineMarker();
    showErrorInStatusBar("");
    set_viewer_bg_color("#9c9c9c");
    if (!last_compile_is_partial) {
        canpartial = true;
        total_page_number = last_pdf_document.find('page').length;
        if (current_page === 0)
            current_page = 1;
        if (current_page > total_page_number)
            current_page = total_page_number;
        $("#pagecontrol").val(current_page + "/" + total_page_number);
    }
    if (current_page !== 0) {
        let htmldata = last_pdf_document.find('page').eq(current_page - 1).text();
        let patch = generateDiffPatch4Viewer();
        setViewerHtml(htmldata, lastEditorState.pos, patch);
        $("#pagecontrol").val(current_page + "/" + total_page_number);
    } else {
        //We should issue a warning.
        showErrorInStatusBar("This file does not generate any output.")
    }
}

function handleFailedCompilation() {
    if (!last_compile_is_partial) {

        set_viewer_bg_color("pink");
        if (last_log_string !== undefined || last_log_string !== "") {
            if (last_log_string.indexOf("No pages of output") !== -1) {
                addEditorErrorLineMarker(0);
                showErrorInStatusBar("No pages of output!");
            } else {
                try {
                    let log_entries = LatexParserModule.parse(last_log_string, {ignoreDuplicates: true});
                    let errorline = log_entries.errors[0].line;
                    addEditorErrorLineMarker(errorline - 1);
                    showErrorInStatusBar(log_entries.errors[0].message + " in line " + errorline);
                } catch (err) {
                    addEditorErrorLineMarker(0);
                    console.error("Unable to parse error log" + err);
                }
            }

        } else {
            console.error("No error log available");
            addEditorErrorLineMarker(0);
        }
        canpartial = false;

    } else {
        console.warn("Partial Compile Failed, Try Full Compile");
        canpartial = false;
        newContentPending = true;
    }
}

function digestCompileResult(compile_resp) {

    last_log_string = compile_resp.log;
    last_compile_is_partial = compile_resp.partial;
    if (compile_resp.status === 0) {
        try {

            let decoder = new TextDecoder();
            let rawMeat = decoder.decode(new Uint8Array(compile_resp.pdf));
            //console.log(rawMeat);
            let outputRawDoc = $.parseXML(rawMeat);
            last_pdf_document = $(outputRawDoc);
            let $fonts = last_pdf_document.find('fontdef');
            loadFonts($fonts);
            let $files = last_pdf_document.find('file');
            buildFileDependencyCache($files);

        } catch (err) {
            console.error("Unable to parse output result");
            compile_resp.status = 1;
        }
    }

    if (compile_resp.status === 0) {
        handleSuccessfulCompilation();
    } else {
        handleFailedCompilation();
    }
}

function generateDiffPatch4Viewer() {
    let editorstate = grabEditorCurrentState();

    if (editorstate.pos.row === lastEditorState.pos.row) {
        let dmp = new diff_match_patch();
        let diff = dmp.diff_main(lastEditorState.context, editorstate.context);
        let delcount = 0;
        let inscount = 0;
        let inscontent = "";
        let delcontent = "";
        for (let tindex in diff) {
            if (diff[tindex][0] === 1) {
                inscount += 1;
                inscontent += diff[tindex][1];
            } else if (diff[tindex][0] === -1) {
                delcount += 1;
                delcontent += diff[tindex][1];
            }
        }
        if (inscount === 1 && delcount === 0) {
            return "+" + inscontent;
        } else if (inscount === 0 && delcount === 1) {
            console.log("it is delete patch >>" + delcontent);
            return "-" + delcontent.split('').reverse().join('');
        }
    }
    return "";
}


async function compileWatchDog() {
    if (status === "idle" && newContentPending) {
        await compile();
        newContentPending = false;
    }
    setTimeout(compileWatchDog, 1000);
}

async function compile() {
    if (status === "busy") {
        console.warn("Rejected");
        return;
    }

    lastEditorState = grabEditorCurrentState();
    writeMemFSFile(lastEditorState.data, currentEditorFileID);
    if (!writeLock) {
        writeLock = true;
        localFS.updateFile(currentEditorFileID, lastEditorState.data).then(_ => {
            writeLock = false;
        }).catch(e => {
            writeLock = false;
            fatal_error("Unable to sync file to remote cloud, please refresh and try again");
        });
    }

    if (currentEditorFileID !== mainFileID && canpartial) {
        if (fileDependencyCache !== undefined) { //We could optimize the non-included file
            //console.log(fileDependencyCache);
            if (!(currentEditorFileID in fileDependencyCache)) {
                console.log("File not include ignored.");
                return;
            }
        }
    }


    status = "busy";
    NProgress.start();

    // partialStateCheck(lastEditorState.pos);
    let partialDecision = canpartial && total_page_number > 2;// We only turn it on when two many pages
    updateStatusBar(`Compilation Ongoing (Boost Mode:${partialDecision}).`);

    try {
        let start_time = performance.now();
        let res = await compileDocument(current_page, partialDecision);
        console.log("Compiler says fuzzbuzz " + (performance.now() - start_time) + " " + partialDecision);
        digestCompileResult(res);
        updateStatusBar('Compilation Finished.');
        status = "idle";
        NProgress.done();
    } catch (err) {
        console.error("Restarting engine" + err);
        updateStatusBar("Engine failure! Restarting");
        await buildLaTeXWorker();
        await preloadProjectFilesToMemFS();

        setEngineMainFile(mainFileID);
        status = "idle";
        NProgress.done();
        updateStatusBar("Engine Recovered!");
    }

}


async function loadSelectedSourceFile(fid, initpos) {
    if (status !== "idle") return;
    if (fid === currentEditorFileID) return;

    if (fid === undefined) return;
    if (!(fid.endsWith('.tex') || fid.endsWith('.txt') || fid.endsWith('.bib') || fid.endsWith(".cls") || fid.endsWith(".bst"))) return;
    status = "busy";
    NProgress.start();
    canpartial = false;
    try {
        const textFileSrc = await localFS.getFile(fid);
        currentEditorFileID = fid;
        updateFileNameInStatusBar(fid);
        //await setupSharePad(gpid, fid, window.username, textFileSrc);
        // ed.setValue(textFileSrc);
        // ed.resize();
        await setupEditor(textFileSrc, fid);
        if (initpos !== undefined) { //Reenter again.
            setEditorInitAnchor(initpos);
        } else {
            setEditorInitAnchor({'row': 0, 'column': 0});
        }
        status = "idle";
        NProgress.done();
    } catch (e) {
        fatal_error("Unable to load source file, please retry later " + e);
    }
}


async function showHistory() {
    if (status !== "idle") return;
    status = "busy";
    NProgress.start();
    try {
        let res = await localFS.getFileRevisionList(currentEditorFileID);
        $("#historyfileoptions").html("");
        res.forEach(entry => {
            $("#historyfileoptions").prepend(`<option value="${entry.rid}">${currentEditorFileID} at ${entry.modifiedTime}</option>`);
        });
        $('#history-modal').modal('show');
        $('#history-modal').off('hide.bs.modal');
        $('#history-modal').on('hide.bs.modal', function (e) {
            let tmpid = $(document.activeElement).attr('id');
            if (tmpid === "downloadhistorybtn") {
                let revid = $("#historyfileoptions").val();
                revertHistoryFile(revid);
            }
        });

    } catch (e) {
        alert("Unable to retrieve file revisions. " + e);
    }
    status = "idle";
    NProgress.done();
}


async function revertHistoryFile(revid) {
    if (status !== "idle") return;
    status = "busy";
    NProgress.start();
    try {
        let tmpRes = await localFS.getFile(currentEditorFileID, false, revid);
        //console.log(tmpRes);
        ed.setValue(tmpRes, 1);
        //await setupSharePad(gpid, currentEditorFileID, window.username, textFileSrc);
    } catch (e) {
        alert("Unable to download the file rev.");
    }
    status = "idle";
    NProgress.done();
}

// function _trySync() {
//     try {
//             localFS.sync("/", true);
//         } catch(err) {
//             console.error("Unable to sync file " + err);
//         }
// }

async function init() {
    status = "init";
    NProgress.start();

    updateStatusBar("Fetching Token");
    try {
        await fetchToken();
        setTimeout(renewToken, 300000);
    } catch (e) {
        fatal_error("Unable to fetch token, please try again");
        return;
    }

    await buildLaTeXWorker();


    updateStatusBar("Loading files from the remote storage");
    try {
        localFS = new SwiftFilesystem();
        await localFS.init();
        localFS.chdir("/" + gpid);
        cachedFileList = await localFS.readdir("/", true);
    } catch (e) {
        fatal_error("Unable to connect to storage " + e);
        return;
    }

    // await localFS.sync("/", true); //Deep sync

    setupFileTreeUI(cachedFileList);


    updateStatusBar("Loading source files");
    await preloadProjectFilesToMemFS();


    updateStatusBar("Resolving Main File");
    mainFileID = await selectMainFilefromSourceList();
    if (mainFileID === undefined) {
        fatal_error("Unable to retrieve the main latex file~");
        return;
    }


    setEngineMainFile(mainFileID);
    currentEditorFileID = mainFileID;
    updateFileNameInStatusBar(currentEditorFileID);
    updateStatusBar("Init the share pad");


    let mainFileText = undefined;
    updateStatusBar("Loading Main File");
    try {
        mainFileText = await localFS.getFile(mainFileID);
    } catch (e) {
        fatal_error("Unable to download main file " + e);
        return;
    }
    await setupEditor(mainFileText, mainFileID);


    setEditorInitAnchor({'row': 0, 'column': 0});
    status = "idle";
    updateStatusBar("Initialized! Ready to go");
    compileWatchDog().then(e => {
    });
    //setInterval(_trySync, 5000);
}

window.addEventListener("beforeunload", function (e) {
    if (newContentPending || writeLock || status === "busy") {
        let confirmationMessage = "\o/";
        e.returnValue = confirmationMessage; //Gecko + IE
        return confirmationMessage;  //Webkit, Safari, Chrome
    }
});

popupMenu();
init();

