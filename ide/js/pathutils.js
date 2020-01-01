


function normalizePath(path) {

    if (path.length === 0) {
        return "/";
    }
    path = path.trim();
    path = path.replace(/\/\//g, "/");
    let parts = splitPath(path);
    parts = parts.reduce(reducer, []);
    return joinPath(...parts);
}

function joinPath(...parts) {
    if (parts.length === 0) return "/";
    let path = parts.join("/");
    // Replace consecutive '/'
    path = path.replace(/\/{2,}/g, "/");
    return path;
}

function splitPath(path) {
    if (path.length === 0) return ["/"];
    if (path === "/") return ["/"];
    let parts = path.split("/");
    if (parts[parts.length - 1] === '') {
        parts.pop();
    }
    if (path[0] === "/") {
        // assert(parts[0] === '')
        parts[0] = "/";
    } else {
        if (parts[0] !== "/") {
            parts.unshift("/");
        }
    }
    return parts;
}

function dirname(path) {
    const last = path.lastIndexOf("/");
    if (last === -1) throw new Error(`Cannot get dirname of "${path}"`);
    if (last === 0) return "/";
    return path.slice(0, last);
}

function basename(path) {
    if (path === "/") throw new Error(`Cannot get basename of "${path}"`);
    const last = path.lastIndexOf("/");
    if (last === -1) return path;
    return path.slice(last + 1);
}

function reducer(ancestors, current) {
    // Initial condition
    if (ancestors.length === 0) {
        ancestors.push(current);
        return ancestors;
    }
    // assert(ancestors.length > 0)
    // assert(ancestors[0] === '.' || ancestors[0] === '/')

    // Collapse '.' references
    if (current === ".") return ancestors;

    // Collapse '..' references
    if (current === "..") {
        if (ancestors.length === 1) {
            if (ancestors[0] === "/") {
                throw new Error("Unable to normalize path - traverses above root directory");
            }
            // assert(ancestors[0] === '.')
            if (ancestors[0] === ".") {
                ancestors.push(current);
                return ancestors;
            }
        }
        // assert(ancestors.length > 1)
        if (ancestors[ancestors.length - 1] === "..") {
            ancestors.push("..");
            return ancestors;
        } else {
            ancestors.pop();
            return ancestors;
        }
    }

    ancestors.push(current);
    return ancestors;
}


function sanitizeFileName(fileName) {
    let sanitized = fileName.replace(/[^\ 0-9A-z.-]/g, '_');
    return sanitized;
}
