
const DEFAULT_CODE = `% SwiftLaTeX sample document, (C) SwiftLabs Auckland Inc. free for you to use for your own texts.
\\documentclass[a4paper, 11pt]{article}
\\usepackage{enumitem}
\\usepackage{graphicx}
\\usepackage{xspace}
\\title{Sample Document Title}
\\author{your Name}
\\date{publication date}
\\begin{document}
% YOUR TEXT CAN START BELOW HERE

\\maketitle

\\begin{abstract}
A document to get you started. Delete any text you do not want.
\\end{abstract}

\\section{Introduction}
Click directly into the text if you want to place the cursor. For technical reasons the cursor cannot be positioned if you click far away from text.  To start a new paragraph, you have to press the enter key twice.

\\subsection{Commands}

For headers or \\emph{other} \\textbf{special} formatting then you need a  \\LaTeX\\xspace command. These commands must be entered in the footer window. Best is to copy/paste such formatting from examples that you can find on many forum websites in the internet. There are also plenty of reasonable  \\LaTeX\\xspace tutorials around.


%DO NOT DELETE BELOW
\\end{document}
`;

let isSystemBusy = true;

function toggleCreateProjectModal() {
    $('.modal').toggleClass('show-modal');
}

function _arrayBufferToJson(arrayBuf) {
    const decoder = new TextDecoder('utf8');
    const response_json = JSON.parse(decoder.decode(arrayBuf));
    return response_json;
}

function makeRandomID(length) {
    let result = '';
    const characters = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789';
    const charactersLength = characters.length;
    for (let i = 0; i < length; i++) {
        result += characters.charAt(Math.floor(Math.random() * charactersLength));
    }
    return result;
}

function logout() {
    localStorage.removeItem('provider');
    window.location.href = '/';
}

let storage = undefined;

async function prepareDefaultFileEntries() {
    const fid = makeRandomID(10) + 'main.tex';
    const url = await storage.put('asset', fid, new Blob([DEFAULT_CODE]));
    return [
        {
            'id': fid,
            'path': 'main.tex',
            'uri': url,
            'type': 'file',
            'asset': false,
        },
    ];
}

function getParentPath(path) {
    return path.includes('/') ? path.replace(/\/[^/]+$/, '') : undefined;
}

function recursiveCreateParent(path, parents) {
    const parentPath = getParentPath(path);
    if (!parentPath) {
        return;
    }
    if (parents.includes(parentPath)) {
        return;
    }
    parents.push(parentPath);
    recursiveCreateParent(parentPath, parents);
}

async function prepareFileEntries(user_zip_blob, remote_zip_url) {
    let zip_blob = user_zip_blob;
    if (remote_zip_url) {
        const response = await fetch(remote_zip_url);
        if (response.ok) {
            zip_blob = await response.blob();
        }
    }
    if (!zip_blob) {
        return prepareDefaultFileEntries();
    }

    try {
        let importedEntries = [];
        const allowed_extension = ['.tex', '.bib', '.bst', '.cls', '.pdf', '.jpg', '.png', '.txt', '.bbx', '.sty', '.ttf', '.otf'];
        const zip_handle = await JSZip.loadAsync(zip_blob);
        let queues = [];
        zip_handle.forEach(function(relativePath, zipEntry) {
            if (zipEntry.dir) {
                return;
            }
            if (zipEntry.size > 1024 * 1024 * 2) {
                return;
            }
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
            queues.push({ path: relativePath, entry: zipEntry });
        });

        let singleRoot = true;
        let parents = [];

        for (let item of queues) {
            recursiveCreateParent(item.path, parents);
        }

        if (parents.length === 0) {
            singleRoot = false;
        } else {
            for (let item of queues) {
                if (!item.path.startsWith(parents[0])) {
                    singleRoot = false;
                }
            }
        }

        if (singleRoot) {
            console.log('Single Root');
            const root = parents.pop();
            for (let j = 0; j < parents.length; j++) {
                parents[j] = parents[j].slice(root.length + 1);
            }
            for (let item of queues) {
                item.path = item.path.slice(root.length + 1);
            }
        }

        for (let item of queues) {
            const isTextEntry = /\.(cls|bib|txt|tex|bst|sty|bbx|md|js|html)$/.test(item.path);
            const randomID = makeRandomID(10) + encodeURIComponent(item.path).replace(/%/g, '_');
            const zipEntryBlob = await item.entry.async('blob');
            const uri = await storage.put('asset', randomID, zipEntryBlob);
            const entry = {
                'id': randomID,
                'path': item.path,
                'uri': uri,
                'type': 'file',
                'asset': !isTextEntry,
            };
            importedEntries.push(entry);
        }

        for (let j = 0; j < parents.length; j++) {
            const entry = {
                'id': makeRandomID(10),
                'path': parents[j],
                'uri': '',
                'type': 'folder',
                'asset': false,
            };
            importedEntries.push(entry);
        }

        console.log(importedEntries);
        return importedEntries;
    } catch (e) {
        console.error('Corrupted Zip ' + e.message);
        return prepareDefaultFileEntries();
    }

}

function onBeginCreateProject() {
    if (isSystemBusy) return;
    let title = $('#projectTitleInput').val();
    if (!title) {
        title = 'My fancy project';
    }
    const zipFile = $('#zipInput')[0].files[0];
    const templateInput = $('#templateInput').val();
    let templateUrl = '';
    if (templateInput === 'Beamer') {
        templateUrl = 'boilerplate/beamer.zip';
    } else if (templateInput === 'CV') {
        templateUrl = 'boilerplate/cv.zip';
    } else if (templateInput === 'IEEE') {
        templateUrl = 'boilerplate/ieee.zip';
    } else if (templateInput === 'ACM') {
        templateUrl = 'boilerplate/acm.zip';
    }

    toggleCreateProjectModal();
    NProgress.start();
    isSystemBusy = true;
    createProject(title, zipFile, templateUrl).then(_ => {
        return listProjects();
    }).catch(_ => {
        alert('Oops: Unable to create project, please refresh and try again?');
    }).finally(_ => {
        isSystemBusy = false;
        NProgress.done();
    });
}

async function createProject(project_name, user_zip_blob, remote_zip_url) {
    let fileEntries;
    if (user_zip_blob || remote_zip_url) {
        fileEntries = await prepareFileEntries(user_zip_blob, remote_zip_url);
    } else {
        fileEntries = await prepareDefaultFileEntries();
    }
    const userInfo = await storage.getUserInfo();
    let newProject = {
        'name': project_name,
        'username': userInfo.username,
        'modifiedTime': new Date().toString(),
        'fileEntries': fileEntries,
        'entryPoint': 'main.tex',
    };
    await storage.put('manifest', makeRandomID(10), new Blob([JSON.stringify(newProject)]));
}

async function deleteProject(project_id) {

    const repoArrayBuffer = await storage.get('manifest', project_id);
    const response_json = _arrayBufferToJson(repoArrayBuffer);
    response_json['deleted'] = true;
    await storage.put('manifest', project_id, new Blob([JSON.stringify(response_json)]));

}

function html_escape(input) {
    return encodeURIComponent(input).replace(/%20/g, ' ');
}

async function renameProject(project_id, new_project_name) {

    const repoArrayBuffer = await storage.get('manifest', project_id);
    const response_json = _arrayBufferToJson(repoArrayBuffer);
    response_json['name'] = html_escape(new_project_name);
    await storage.put('manifest', project_id, new Blob([JSON.stringify(response_json)]));

}

function openFunction() {
    const pid = $(this).data('pid');
    window.location.href = '/editor.html?p=' + pid;
}

function delFunction() {
    if (isSystemBusy) return;
    const pid = $(this).data('pid');
    if (confirm('Are you going to delete this project?')) {
        NProgress.start();
        isSystemBusy = true;
        deleteProject(pid).then(_ => {
            return listProjects();
        }).catch(_ => {
            alert('Oops: Unable to delete the project, please refresh and try again?');
        }).finally(_ => {
            isSystemBusy = false;
            NProgress.done();
        });
    }
}

function renameFunction() {
    if (isSystemBusy) return;
    const pid = $(this).data('pid');
    const name = prompt('Please input a new name for your project?');
    if (name != null && name.length > 0) {
        NProgress.start();
        isSystemBusy = true;
        renameProject(pid, name).then(_ => {
            return listProjects();
        }).catch(_ => {
            alert('Oops: Unable to rename the project, please refresh and try again?');
        }).finally(_ => {
            isSystemBusy = false;
            NProgress.done();
        });
    }
}


async function popupEachProject(repoID) {
    try {

        const repoArrayBuffer = await storage.get('manifest', repoID);
        const response_json = _arrayBufferToJson(repoArrayBuffer);
        if ('deleted' in response_json) {
            return;
        }
        const project_name = html_escape(response_json['name']);
        const author = response_json['username'];
        const modified_time = new Date(response_json['modifiedTime']).toDateString();
        const append_html = `<tr><td>${html_escape(project_name)}</td>
<td>${html_escape(author)}</td>
<td>${modified_time}</td>
<td>
<a class="button actionButton openButton" href="#" data-pid="${repoID}">Open</a>
<a class="button actionButton deleteButton" href="#" data-pid="${repoID}">Delete</a>
<a class="button actionButton renameButton" href="#" data-pid="${repoID}">Rename</a>
<a class="button actionButton downloadButton" href="#" data-pid="${repoID}">Archive</a>
</td></tr>`;
        return append_html;
    } catch (e) {
        console.error('Failed to load ' + repoID);
        return '';
    }
}

function compareFn(a, b) {
    const timestampA = Date.parse(a.modifiedTime);
    const timestampB = Date.parse(b.modifiedTime);
    return timestampA < timestampB ? 1 : -1;
}

function downloadFunction() {
    if (isSystemBusy) return;
    const pid = $(this).data('pid');
    NProgress.start();
    isSystemBusy = true;
    downloadProject(pid).catch(_ => {
        alert('Oops: Unable to download the project, please refresh and try again?');
    }).finally(_ => {
        isSystemBusy = false;
        NProgress.done();
    });
}

async function downloadProject(pid) {
    const zipobj = new JSZip();
    const repoArrayBuffer = await storage.get('manifest', pid);
    const response_json = _arrayBufferToJson(repoArrayBuffer);
    const fileEntries = response_json['fileEntries'];
    for (const entry of fileEntries) {
        if (entry.type === 'file') {
            const fileBuffer = await storage.get('asset', entry.id);
            zipobj.file(entry.path, fileBuffer);
        }
    }
    const blobFile = await zipobj.generateAsync({ type: 'blob' });
    const tempPDFURL = URL.createObjectURL(blobFile);
    setTimeout(_ => {
        URL.revokeObjectURL(tempPDFURL);
    }, 10000);
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
}

async function listProjects() {
    const projectListUI = $('#project-list');
    projectListUI.html('');
    const repos = await storage.list('manifest');
    repos.sort(compareFn);

    const promises = repos.map(r => popupEachProject(r.itemKey));
    while (promises.length) {
        // 10 at at time
        const htmls = await Promise.all(promises.splice(0, 10));
        for (let html of htmls) {
            projectListUI.append(html);
        }
    }

    $('.openButton').click(openFunction);
    $('.deleteButton').click(delFunction);
    $('.renameButton').click(renameFunction);
    $('.downloadButton').click(downloadFunction);
}

function init() {
    NProgress.start();
    const provider = localStorage.getItem('provider');
    const stored_token = window.localStorage.getItem('access_token');
    if (!stored_token || !provider) {
        alert('Not signed in yet');
        logout();
        return;
    }

    if (provider === 'google') {
        storage = new GoogleStorage(stored_token);
    } else if (provider === 'dropbox') {
        storage = new DropboxStorage(stored_token);
    } else if (provider === 'minio') {
        storage = new MinioStorage(stored_token);
    } else { /* Not expected */
        logout();
        return;
    }
    listProjects().then(_ => {
        NProgress.done();
        isSystemBusy = false;
    }).catch(e => {
        alert('Oops: Unable to fetch project list, please refresh and try again ' + e);
    });
}

isSystemBusy = false;
init();
