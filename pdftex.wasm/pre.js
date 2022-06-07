const {parentPort} = require('node:worker_threads');
const XMLHttpRequest = require("xmlhttprequest").XMLHttpRequest;

const TEXCACHEROOT = "/tex";
const WORKROOT = "/work";
var Module = {};
const global_ctx = {};
global_ctx.memlog = "";
global_ctx.initmem = undefined;
global_ctx.mainfile = "main.tex"
global_ctx.texlive_endpoint = "https://texlive2.swiftlatex.com/";

function dumpHeapMemory() {
    var src = wasmMemory.buffer;
    var dst = new Uint8Array(src.byteLength);
    dst.set(new Uint8Array(src));
    // console.log("Dumping " + src.byteLength);
    return dst;
}

function restoreHeapMemory() {
    if (global_ctx.initmem) {
        var dst = new Uint8Array(wasmMemory.buffer);
        dst.set(global_ctx.initmem);
    }
}

function closeFSStreams() {
    for (var i = 0; i < FS.streams.length; i++) {
        var stream = FS.streams[i];
        if (!stream || stream.fd <= 2) {
            continue;
        }
        FS.close(stream);
    }
}

function prepareExecutionContext() {
    global_ctx.memlog = '';
    restoreHeapMemory();
    closeFSStreams();
    FS.chdir(WORKROOT);
}

Module['print'] = function(a) {
    global_ctx.memlog += (a + "\n");
};

Module['printErr'] = function(a) {
    global_ctx.memlog += (a + "\n");
    console.log(a);
};

Module['preRun'] = function() {
    FS.mkdir(TEXCACHEROOT);
    FS.mkdir(WORKROOT);
};

Module['postRun'] = function() {
    parentPort.postMessage({
        'result': 'ok',
    });
    global_ctx.initmem = dumpHeapMemory();
};

function cleanDir(dir) {
    let l = FS.readdir(dir);
    for (let i in l) {
        let item = l[i];
        if (item === "." || item === "..") {
            continue;
        }
        item = dir + "/" + item;
        let fsStat = undefined;
        try {
            fsStat = FS.stat(item);
        } catch (err) {
            console.log("Not able to fsstat " + item);
            continue;
        }
        if (FS.isDir(fsStat.mode)) {
            cleanDir(item);
        } else {
            try {
                FS.unlink(item);
            } catch (err) {
                console.log("Not able to unlink " + item);
            }
        }
    }

    if (dir !== WORKROOT) {
        try {
            FS.rmdir(dir);
        } catch (err) {
            console.log("Not able to top level " + dir);
        }
    }
}



Module['onAbort'] = function() {
    global_ctx.memlog += 'Engine crashed';
    parentPort.postMessage({
        'result': 'failed',
        'status': -254,
        'log': global_ctx.memlog,
        'cmd': 'compile'
    });
    return;
};

function compileLaTeXRoutine() {
    prepareExecutionContext();
    const setMainFunction = cwrap('setMainEntry', 'number', ['string']);
    setMainFunction(global_ctx.mainfile);
    let status = _compileLaTeX();
    if (status === 0) {
        let pdfArrayBuffer = null;
        _compileBibtex();
        try {
            let pdfurl = WORKROOT + "/" + global_ctx.mainfile.substr(0, global_ctx.mainfile.length - 4) + ".pdf";
            pdfArrayBuffer = FS.readFile(pdfurl, {
                encoding: 'binary'
            });
        } catch (err) {
            console.log("Fetch content failed.");
            status = -253;
            parentPort.postMessage({
                'result': 'failed',
                'status': status,
                'log': global_ctx.memlog,
                'cmd': 'compile'
            });
            return;
        }
        parentPort.postMessage({
            'result': 'ok',
            'status': status,
            'log': global_ctx.memlog,
            'pdf': pdfArrayBuffer.buffer,
            'cmd': 'compile'
        }, [pdfArrayBuffer.buffer]);
    } else {
        console.log("Compilation failed, with status code " + status);
        parentPort.postMessage({
            'result': 'failed',
            'status': status,
            'log': global_ctx.memlog,
            'cmd': 'compile'
        });
    }
}

function compileFormatRoutine() {
    prepareExecutionContext();
    let status = _compileFormat();
    if (status === 0) {
        let pdfArrayBuffer = null;
        try {
            let pdfurl = WORKROOT + "/pdflatex.fmt";
            pdfArrayBuffer = FS.readFile(pdfurl, {
                encoding: 'binary'
            });
        } catch (err) {
            console.log("Fetch content failed.");
            status = -253;
            parentPort.postMessage({
                'result': 'failed',
                'status': status,
                'log': global_ctx.memlog,
                'cmd': 'compile'
            });
            return;
        }
        parentPort.postMessage({
            'result': 'ok',
            'status': status,
            'log': global_ctx.memlog,
            'pdf': pdfArrayBuffer.buffer,
            'cmd': 'compile'
        }, [pdfArrayBuffer.buffer]);
    } else {
        console.log("Compilation format failed, with status code " + status);
        parentPort.postMessage({
            'result': 'failed',
            'status': status,
            'log': global_ctx.memlog,
            'cmd': 'compile'
        });
    }
}

function mkdirRoutine(dirname) {
    try {
        //console.log("removing " + item);
        FS.mkdir(WORKROOT + "/" + dirname);
        parentPort.postMessage({
            'result': 'ok',
            'cmd': 'mkdir'
        });
    } catch (err) {
        console.log("Not able to mkdir " + dirname);
        parentPort.postMessage({
            'result': 'failed',
            'cmd': 'mkdir'
        });
    }
}

function writeFileRoutine(filename, content) {
    try {
        FS.writeFile(WORKROOT + "/" + filename, content);
        parentPort.postMessage({
            'result': 'ok',
            'cmd': 'writefile'
        });
    } catch (err) {
        console.log("Unable to write mem file");
        parentPort.postMessage({
            'result': 'failed',
            'cmd': 'writefile'
        });
    }
}

function setTexliveEndpoint(url) {
    if(url) {
        if (!url.endsWith("/")) {
            url += '/';
        }
        global_ctx.texlive_endpoint = url;
    }
}

parentPort.on('message', data => {
    let cmd = data['cmd'];
    if (cmd === 'compilelatex') {
        compileLaTeXRoutine();
    } else if (cmd === 'compileformat') {
        compileFormatRoutine();
    } else if (cmd === "settexliveurl") {
        setTexliveEndpoint(data['url']);
    } else if (cmd === "mkdir") {
        mkdirRoutine(data['url']);
    } else if (cmd === "writefile") {
        writeFileRoutine(data['url'], data['src']);
    } else if (cmd === "setmainfile") {
        global_ctx.mainfile = data['url'];
    } else if (cmd === "flushcache") {
        cleanDir(WORKROOT);
    } else {
        console.error("Unknown command " + cmd);
    }
});


function kpse_find_file_impl(nameptr, format, _mustexist) {

    const reqname = UTF8ToString(nameptr);

    if (reqname.includes("/")) {
        return 0;
    }

    const cacheKey = format + "/" + reqname ;
    
    const remote_url = global_ctx.texlive_endpoint + 'pdftex/' + cacheKey;
    let xhr = new XMLHttpRequest();
    xhr.open("GET", remote_url, false);
    xhr.timeout = 150000;
    xhr.responseType = "arraybuffer";
    console.log("Start downloading texlive file " + remote_url);
    try {
        xhr.send();
    } catch (err) {
        console.log("TexLive Download Failed " + remote_url);
        return 0;
    }

    if (xhr.status === 200) {
        console.log("TexLive File downloaded " + remote_url);

        let arraybuffer = xhr.response;
        const fileid = xhr.getResponseHeader('fileid');
        const savepath = TEXCACHEROOT + "/" + fileid;
        FS.writeFile(savepath, new Uint8Array(arraybuffer));
        return allocate(intArrayFromString(savepath), 'i8', ALLOC_NORMAL);

    } else if (xhr.status === 301) {
        console.log("TexLive File not exists " + remote_url);
        return 0;
    } 
    return 0;
}


function kpse_find_pk_impl(nameptr, dpi) {
    const reqname = UTF8ToString(nameptr);

    if (reqname.includes("/")) {
        return 0;
    }

    const cacheKey = dpi + "/" + reqname ;

    const remote_url = global_ctx.texlive_endpoint + 'pdftex/pk/' + cacheKey;
    let xhr = new XMLHttpRequest();
    xhr.open("GET", remote_url, false);
    xhr.timeout = 150000;
    xhr.responseType = "arraybuffer";
    console.log("Start downloading texlive file " + remote_url);
    try {
        xhr.send();
    } catch (err) {
        console.log("TexLive Download Failed " + remote_url);
        return 0;
    }

    if (xhr.status === 200) {
        let arraybuffer = xhr.response;
        const pkid = xhr.getResponseHeader('pkid');
        const savepath = TEXCACHEROOT + "/" + pkid;
        FS.writeFile(savepath, new Uint8Array(arraybuffer));
        return allocate(intArrayFromString(savepath), 'i8', ALLOC_NORMAL);

    } else if (xhr.status === 301) {
        console.log("TexLive File not exists " + remote_url);
        return 0;
    } 
    return 0;

}