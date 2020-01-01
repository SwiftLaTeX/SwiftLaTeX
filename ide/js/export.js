

NProgress.configure({showSpinner: true});
const urlParams = new URLSearchParams(window.location.search);
if (!urlParams.has('pid')) {
    window.location.href = '/';
}
let gpid = urlParams.get('pid');
let main_file_id = urlParams.get('mainfileid');
let cachedFileList = undefined;
let networkClient = undefined;
let export_log = undefined;
let export_pdf = undefined;
const MAXRERUNTIME = 4;

function fatal_error(reason) {
    status = "error";
    $('#alert-modal-body').html("<p>" + reason + "</p>");
    $('#alert-modal').modal('show');
    closeWorker();
}

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


function updateStatusBar(item) {
    let logui = $("#logconsole");
    logui.prepend("<div>" + item + "</div>");
}

async function exportPDF() {

    NProgress.start();

    try {
        await fetchToken();
    } catch (e) {
        fatal_error("Unable to fetch token, please try again");
        return;
    }

    try {
        networkClient = new SwiftFilesystem();
        await networkClient.init();
        networkClient.chdir("/" + gpid);
    } catch (e) {
        fatal_error("Unable to connect to remote storage " + e);
        return;
    }

    await buildLaTeXWorker("js/exporter/pdflatexori.js");


    try {
        let resp = await networkClient.readdir("", true);
        cachedFileList = resp;
        let logui = $("#logconsole");
        for (let i = 0; i < resp.length; i++) {
            let entry = resp[i];
            logui.prepend("<div> Downloading " + entry.path + "</div>");
            if (entry.isfolder) {
                if (entry.path !== "") {
                    makeMemFSFolder(entry.path); //We expect top folder to exists
                }
            } else {
                let uint8array = await networkClient.getFile(entry.path, true);

                writeMemFSFile(uint8array, entry.path);
            }
        }
    } catch (e) {
        fatal_error("Unable to preload project file list" + e);
        return;
    }


    setEngineMainFile(main_file_id);

    try {
        for (let i = 0; i < MAXRERUNTIME; i++) {
            let resp = await compileDocument();
            export_log = resp.log;
            export_pdf = resp.pdf;
            if (resp.status !== 0) {
                fatal_error("error spotted in the document, please recheck");
                break;
            }
            let logui = $("#logconsole");
            // logui.prepend("<pre"> + export_log + "</pre>");
            try {
                let log_entries = LatexParserModule.parse(export_log, {ignoreDuplicates: true});

                for (let tmpid in log_entries.all) {
                    let msg = `[${tmpid}]${log_entries.all[tmpid]['level']} in file ${log_entries.all[tmpid]['file']}:${log_entries.all[tmpid]['message']}`;
                    logui.prepend("<div>" + msg + "</div>");
                }
            } catch (e) {
                logui.prepend("<div>Corrupted Log Detected</div>");
            }

        }

        if (export_pdf !== undefined) {
            let dummyfile = new Blob([export_pdf], {type: "application/octet-binary;charset=utf-8"});
            let dummyurl = URL.createObjectURL(dummyfile);
            let alink = $("#downloadlink");
            alink.attr("href", dummyurl);
            let current_datetime = new Date();
            let formatted_date = current_datetime.getFullYear() + "_" + (current_datetime.getMonth() + 1) + "_" + current_datetime.getDate() + "_" + current_datetime.getHours() + "_" + current_datetime.getMinutes() + "_" + current_datetime.getSeconds();
            alink.attr("download", "export" + formatted_date + ".pdf");
            alink.text("Download PDF");
            alink.show();
        }

    } catch (e) {
        fatal_error("Unexpected error detected " + e);
        return;
    }
    NProgress.done();
    closeWorker();

}

exportPDF();