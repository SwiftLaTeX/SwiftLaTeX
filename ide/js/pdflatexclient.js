/*

SwiftLaTeX REFERENCE SOURCE LICENSE (SwiftLaTeX-RSL)

This license governs use of the accompanying software. If you use the software, you accept this license. If you do not accept the license, do not use the software.

1. Definitions
The terms "reproduce," "reproduction" and "distribution" have the same meaning here as under U.S. copyright law.

"You" means the licensee of the software.

"Your company" means the company you worked for when you downloaded the software.

"Reference use" means use of the software within your company as a reference, in read only form, for the sole purposes of debugging your products, maintaining your products, or enhancing the interoperability of your products with the software, and specifically excludes the right to distribute the software outside of your company.

"Licensed patents" means any Licensor patent claims which read directly on the software as distributed by the Licensor under this license.

2. Grant of Rights
(A) Copyright Grant- Subject to the terms of this license, the Licensor grants you a non-transferable, non-exclusive, worldwide, royalty-free copyright license to reproduce the software for reference use.

(B) Patent Grant- Subject to the terms of this license, the Licensor grants you a non-transferable, non-exclusive, worldwide, royalty-free patent license under licensed patents for reference use.

3. Limitations
(A) No Trademark License- This license does not grant you any rights to use the Licensor's name, logo, or trademarks.

(B) If you begin patent litigation against the Licensor over patents that you think may apply to the software (including a cross-claim or counterclaim in a lawsuit), your license to the software ends automatically.

(C) The software is licensed "as-is." You bear the risk of using it. The Licensor gives no express warranties, guarantees or conditions. You may have additional consumer rights under your local laws which this license cannot change. To the extent permitted under your local laws, the Licensor excludes the implied warranties of merchantability, fitness for a particular purpose and non-infringement.


 */


let latexWorker = undefined;
let latexWorkerStatus = "NA";

let _promiseResolve = undefined;
let _promiseReject = undefined;

function _internalMessageHandler(ev) {
    let data = ev['data'];
    let cmd = data['cmd'];
    let result = data['result'];
    //console.log(data);
    if (cmd === "postRun") {
        latexWorkerStatus = "ready";
        _promiseResolve();
    } else if (cmd === "compile") {
        latexWorkerStatus = "ready";
        if (result === "failed") {
            console.error("Something goes really wrong, restarting " + data['log']);
            latexWorker = undefined; //It should be closed by the worker
            latexWorkerStatus = "NA";
            _promiseReject(data['log']);

        } else {
            //console.log("worker compile finish " + (performance.now() - start_compile_time));
            _promiseResolve(data);
        }

    } else if (cmd === "texlivefetch") {
        updateStatusBar("Fetching file " + data['data']);
    } else {
        console.log(data);
    }
}

function buildLaTeXWorker(bin) {
    if (bin === undefined) {
        bin = "js/fastlatex/cpdfetex.js";
    }
    if (latexWorker !== undefined) {
        console.error("Other instance is running, abort()");
        latexWorker.postMessage({"cmd": "grace"});
        latexWorker = undefined;
    }
    latexWorkerStatus = "init";
    return new Promise((resolve, reject) => {
        _promiseReject = reject;
        _promiseResolve = resolve;
        latexWorker = new Worker(bin);
        latexWorker.onmessage = _internalMessageHandler;
    });

}


function compileDocument(page, partial) {

    if (latexWorkerStatus !== "ready") {
        console.log("Instance not ready");
        return;
    }
    latexWorkerStatus = 'busy';
    //start_compile_time =  performance.now();

    return new Promise((resolve, reject) => {
        _promiseReject = reject;
        _promiseResolve = resolve;

        if (partial) {
            latexWorker.postMessage({"cmd": "compile", "page": page});
        } else {
            latexWorker.postMessage({"cmd": "compile"});
        }
    });

}


function setEngineMainFile(filename) {
    if (latexWorker !== undefined) {
        //console.log(filelist);

        latexWorker.postMessage({'cmd': 'setmainfile', 'url': filename});
    }
}

function writeMemFSFile(srccode, filename) {
    if (latexWorker !== undefined) {
        //console.log(filelist);
        latexWorker.postMessage({'cmd': 'writefile', 'url': filename, 'src': srccode});
    }
}

// function delete(filename)
// {
//     if(latexWorker !== undefined) {
//         //console.log(filelist);
//          latexWorker.postMessage({'cmd':'deletefile', 'url':filename});
//     }
// }
//
// function moveFile(oldfilename, newfilename)
// {
//     if(latexWorker !== undefined) {
//         //console.log(filelist);
//          latexWorker.postMessage({'cmd':'movefile', 'oldurl':oldfilename, 'newurl':newfilename});
//     }
// }
//
// function deleteFolder(filename)
// {
//     if(latexWorker !== undefined) {
//         //console.log(filelist);
//          latexWorker.postMessage({'cmd':'rmdir', 'url':filename});
//     }
// }

function makeMemFSFolder(filename) {
    if (latexWorker !== undefined) {
        //console.log(filelist)
        let tmpF = filename;
        if (tmpF === "" || tmpF === "/") return;
        latexWorker.postMessage({'cmd': 'mkdir', 'url': tmpF});
    }
}

function flushCache() {
    if (latexWorker !== undefined) {
        //console.log(filelist);
        console.warn("Flushing");
        latexWorker.postMessage({'cmd': 'flushcache'});
    }

}

function closeWorker() {
    if (latexWorker !== undefined)
        latexWorker.postMessage({'cmd': 'grace'});
}