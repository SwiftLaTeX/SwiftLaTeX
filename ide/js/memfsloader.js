


let pdfjsLib = window['pdfjs-dist/build/pdf'];

function isImageType(filename) {
    if (filename === undefined)
        return undefined;
    else if (filename.endsWith(".pdf"))
        return "pdf";
    else if (filename.endsWith(".jpg"))
        return "jpg";
    else if (filename.endsWith(".png"))
        return "png";
    return undefined;
}

async function generateImageMeta(entrypath, uint8array) {

    return new Promise((resolve, reject) => {
        let objectURL = URL.createObjectURL(new Blob([uint8array.buffer], {type: 'application/octet-stream'}));
        let tmpimg = new Image();
        tmpimg.onload = (_ => {
            //URL.revokeObjectURL(objectURL);
            resolve(`${tmpimg.width}\n${tmpimg.height}\n${entrypath}`);
        });
        tmpimg.onerror = (_ => {
            console.error("Unable to open the image document, return a dummy dimension ");
            //URL.revokeObjectURL(objectURL);
            resolve(`${300}\n${400}\n${entrypath}`);
        });
        tmpimg.src = objectURL;
    });

}

async function generatePDFMeta(entrypath, uint8array) {

    try {
        let doc = await pdfjsLib.getDocument(uint8array).promise;
        let page = await doc.getPage(1);
        let viewport = page.getViewport({scale: 1});
        let tmpWidth = viewport.width;
        let tmpHeight = viewport.height;
        doc.destroy();
        return `${tmpWidth}\n${tmpHeight}\n${entrypath}`;
    } catch (e) {
        return `${300}\n${400}\n${entrypath}`
    }
}


async function preloadProjectFilesToMemFS() {

    if (cachedFileList !== undefined) {
        for (let i = 0; i < cachedFileList.length; i++) {
            let entry = cachedFileList[i];
            if (entry.isfolder) {

                makeMemFSFolder(entry.path); //We expect top folder to exists

                continue;
            }
            try {
                //console.log(entry);
                let uint8blob = await localFS.getFile(entry.path, true);
                let imgType = isImageType(entry.name);
                if (imgType === "pdf") {
                    // const hashA = await crypto.subtle.digest('SHA-1', uint8blob);
                    // let decoder =  new TextDecoder();
                    // let hash = decoder.decode(new Uint8Array(hashA));
                    // let stat = localFS.stat(entry.path);

                    let meta = localStorage.getItem(entry.name + entry.mtime);
                    if (meta == null) {
                        meta = await generatePDFMeta(entry.path, uint8blob);
                        localStorage.setItem(entry.name + entry.mtime, meta);
                    }
                    writeMemFSFile(meta, entry.path);
                } else if (imgType === "png" || imgType === "jpg") {
                    meta = await generateImageMeta(entry.path, uint8blob);
                    writeMemFSFile(meta, entry.path);
                } else {
                    writeMemFSFile(uint8blob, entry.path);
                }
            } catch (e) {
                fatal_error("Error detected when loading projects files " + e)
            }
        }
    }
}

