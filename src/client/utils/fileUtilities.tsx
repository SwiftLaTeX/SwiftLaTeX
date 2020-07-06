import escapeRegExp from 'escape-string-regexp';


export function isInsideFolder(path: string, folderPath: string) {
    return path.startsWith(`${folderPath}/`);
}

export function getParentPath(path: string): string | undefined {
    return path.includes('/') ? path.replace(/\/[^/]+$/, '') : undefined;
}

export function changeParentPath(path: string, oldParentPath: string, newParentPath: string) {
    return path.replace(new RegExp('^' + escapeRegExp(oldParentPath + '/')), newParentPath + '/');
}

export const genRandomString = (length: number = 32) => {
    let result = '';
    const characters = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789';
    const charactersLength = characters.length;
    for (let i = 0; i < length; i++) {
        result += characters.charAt(Math.floor(Math.random() * charactersLength));
    }
    return result;
};

export const triggerDownloadBlob = (blob: Blob, name: string) => {
    const tempPDFURL = URL.createObjectURL(blob);
    setTimeout(_ => {
        URL.revokeObjectURL(tempPDFURL);
    }, 30000);
    // Simulate link click to download file
    const element = document.createElement('a');
    if (element && document.body) {
        document.body.appendChild(element);
        element.setAttribute('href', tempPDFURL);
        element.setAttribute('download', name);
        element.style.display = '';
        element.click();
        document.body.removeChild(element);
    }
};

export function genRandomFileID(path: string) {
    // return encodeURIComponent(path).replace(/%/g, '_') + genRandomString(5);
    const preCoded = encodeURIComponent(path).replace(/%/g, '_');
    const splits = preCoded.split('.');
    const extension = splits.pop();
    if (extension === preCoded) {
        /* Without extension */
        return preCoded + '.' + genRandomString(8);
    }

    return splits + '.' + genRandomString(5) + '.' + extension;
}

export function getBaseName(path: string) {
    return path.split('/').reverse()[0];
}

export function removeExtension(filename: string) {
    const lastDotPosition = filename.lastIndexOf('.');
    if (lastDotPosition === -1) return filename;
    else return filename.substr(0, lastDotPosition);
}


export function getUniquePath(allPaths: string[], suggestedPath: string, initialSuffix?: string) {
    const parts = suggestedPath.includes('.') ? suggestedPath.split('.') : undefined;
    const ext = parts ? parts.pop() : '';
    const initialPath = parts ? parts.join('.') : suggestedPath;

    let path = suggestedPath;
    let counter = initialSuffix ? 0 : 1;

    while (allPaths.some(p => p.toLowerCase() === path.toLowerCase())) {
        const suffix = `${initialSuffix || ''} ${counter || ''}`.trim();

        if (ext) {
            path = `${initialPath}${suffix}.${ext}`;
        } else {
            path = `${initialPath}${suffix}`;
        }

        counter++;
    }

    return path;
}

export function supportedFileTypes(): string {
    return '.jpg,.png,.pdf,.cls,.bib,.txt,.tex,.bst,.sty,.bbx,.bmp,.md';
}

export function isTextFile(name: string): boolean {
    return /\.(cls|bib|txt|tex|bst|sty|bbx|md|js|html)$/.test(name);
}

export function isImageFile(name: string): boolean {
    return /\.(bmp|jpg|jpeg|png|gif|pdf)$/.test(name);
}

export function isPDFImage(name: string): boolean {
    return /\.(pdf)$/.test(name);
}

export function isOpenTypeFontFile(name: string): boolean {
    return /\.(ttf|otf)$/.test(name);
}

export function arrayBufferToJson(arrayBuf: ArrayBuffer) {
    const jsonStr = arrayBufferToString(arrayBuf);
    return JSON.parse(jsonStr);
}

export function arrayBufferToString(arrayBuf: ArrayBuffer) {
    const decoder = new TextDecoder('utf8');
    return decoder.decode(arrayBuf);
}

