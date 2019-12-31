let latexWorker = undefined;
let latexWorkerStatus = "NA";
let start_compile_time = 0;
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
            console.log("worker compile finish " + (performance.now() - start_compile_time));
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
        bin = "swiftlatex.js";
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
    start_compile_time =  performance.now();

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
        latexWorker.postMessage({'cmd': 'setmainfile', 'url': filename});
    }
}

function writeMemFSFile(srccode, filename) {
    if (latexWorker !== undefined) {
        latexWorker.postMessage({'cmd': 'writefile', 'url': filename, 'src': srccode});
    }
}


function makeMemFSFolder(filename) {
    if (latexWorker !== undefined) {
        let tmpF = filename;
        if (tmpF === "" || tmpF === "/") return;
        latexWorker.postMessage({'cmd': 'mkdir', 'url': tmpF});
    }
}

function flushCache() {
    if (latexWorker !== undefined) {
        console.warn("Flushing");
        latexWorker.postMessage({'cmd': 'flushcache'});
    }

}

function closeWorker() {
    if (latexWorker !== undefined)
        latexWorker.postMessage({'cmd': 'grace'});
}