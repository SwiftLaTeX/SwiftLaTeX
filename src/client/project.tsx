import * as React from 'react';
import ReactDOM from 'react-dom';
import ProgressIndicator from './components/shared/ProgressIndicator';
import { initializeStorage, BackendStorage } from './storage';
import {
    arrayBufferToJson,
    genRandomString,
    triggerDownloadBlob,
    getParentPath,
    genFileID,
} from './utils/fileUtilities';
import JSZip from 'jszip';
import { EventReporter } from './utils/eventReport';
import { ProjectEntry } from './types';

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
% YOUR TEXT CAN START BELOW HERE

\\begin{abstract}
A sample document to get you started. Delete any text you do not want.
\\end{abstract}

\\section{Introduction}
You can click directly into the text in the PDF preview panel to place the cursor. Enjoy the WYSIWYG capabilities of the PDF view.
\\begin{itemize}
\\item For technical reasons the cursor cannot be positioned if you click far away from text. 
\\item To start a new paragraph, you have to press the enter key twice.
\\end{itemize}

\\noindent You can export the final document as a PDF from the menu above. You can also export the source code of the document at any time.


\\subsection{Commands}

For headers or \\emph{other} \\textbf{special} formatting you need a  \\LaTeX\\xspace command. These commands must be entered into the source code view. Best is to copy/paste such formatting from examples that you can find on many forum websites in the internet. There are also plenty of reasonable  \\LaTeX\\xspace tutorials around.


%DO NOT DELETE BELOW
\\end{document}
`;


type Props = {};

type State = {
    projects: ProjectEntry[];
    isBusy: boolean;
    currentModal: 'create' | undefined;
    sessionState: 'panel' | 'import' | 'error' | 'init';
    errorMessage: string
};

class Project extends React.PureComponent<Props, State> {
    constructor(props: Props) {
        super(props);
        this.state = {
            projects: [],
            isBusy: true,
            currentModal: undefined,
            sessionState: 'init',
            errorMessage: '',
        };
    }

    projectNameRef = React.createRef<HTMLInputElement>();
    fileRef = React.createRef<HTMLInputElement>();
    selectRef = React.createRef<HTMLSelectElement>();
    state: State;
    storage: BackendStorage = undefined as any;

    componentDidMount() {
        try {
            this.storage = initializeStorage();
        } catch {
            this.setState({ sessionState: 'error', errorMessage: 'Please login in first.' });
            return;
        }

        this.storage
            .getUserInfo()
            .then((info) => {
                EventReporter.reportEvent('project', 'login', info);
            })
            .catch((_) => {
            });

        const urlParams = new URLSearchParams(window.location.search);
        const shareID = urlParams.get('share') || '';
        if (!shareID) {
            this.setState({ sessionState: 'panel' });
        } else {
            this.setState({ sessionState: 'import' });
        }
    }

    componentDidUpdate(_: Props, prevState: State) {
        if (this.state.sessionState !== prevState.sessionState) {
            if (this.state.sessionState === 'panel') {
                this.setState({ isBusy: true });
                this._listProjects()
                    .then()
                    .catch((_) => {
                        this.setState({
                            sessionState: 'error',
                            errorMessage: 'Unable to fetch projects, please  try again',
                        });
                    })
                    .finally(() => {
                        this.setState({ isBusy: false });
                    });
            } else if (this.state.sessionState === 'import') {
                this.setState({ isBusy: true });
                this._handleImportProject()
                    .then(_ => {
                        this.setState({ 'sessionState': 'panel' });
                    })
                    .catch((_) => {
                        this.setState({
                            sessionState: 'error',
                            errorMessage: 'Error detected when receiving a shared project, please  try again',
                        });
                    }).finally(() => {
                    this.setState({ isBusy: false });
                });
            }
        }

    }

    _handleImportProject = async () => {
        const urlParams = new URLSearchParams(window.location.search);
        const shareID = urlParams.get('share') || '';

        const result = await this.storage.get('manifest', shareID);
        if (result) {
            return;
        }

        const queryUrlRequest = await fetch(`/s/query?p=${shareID}`);
        if (!queryUrlRequest.ok) {
            throw 'Shared Project not found.';
        }

        const queryUrl = await queryUrlRequest.text();
        /* Cors Fetch */
        const manifestRequest = await fetch(`/s/fetch?uri=${encodeURIComponent(queryUrl)}`);
        if (!manifestRequest.ok) {
            throw 'Cannot copy the shared project.';
        }

        /* Fetch each asset file */
        const manifest = (await manifestRequest.json()) as ProjectEntry;
        for (let item of manifest.fileEntries) {
            if (item.type === 'folder') continue;

            const uri = item.uri;
            const assetRequest = await fetch(`/s/fetch?uri=${encodeURIComponent(uri)}`);
            if (!assetRequest.ok) {
                throw 'Cannot download the shared project.';
            }
            const arrayBlob = await assetRequest.blob();
            // console.log('Length ' + arrayBlob.size);
            const newUrl = await this.storage.put('asset', item.id, arrayBlob);
            item.uri = newUrl;
        }
        manifest.shareEnabled = true;
        await this.storage.put('manifest', shareID, new Blob([JSON.stringify(manifest)]));
        await this.setState({ sessionState: 'panel' });
    };

    _handleLogout = () => {
        localStorage.removeItem('provider');
        window.location.href = '/';
    };

    _handleOpenProject = (pid: string) => {
        if (this.state.isBusy) return;
        EventReporter.reportEvent('project', 'open');
        window.location.href = '/editor.html?p=' + pid;
    };

    _handleDeleteProject = async (pid: string) => {
        if (this.state.isBusy) return;
        EventReporter.reportEvent('project', 'delete');
        this.setState({ isBusy: true });
        try {
            const repoArrayBuffer = await this.storage.get('manifest', pid);
            if (!repoArrayBuffer) {
                throw `manifest not found ${pid}`;
            }
            const response_json = arrayBufferToJson(repoArrayBuffer);
            response_json.deleted = true;
            await this.storage.put('manifest', pid, new Blob([JSON.stringify(response_json)]));
            await this._listProjects();
        } catch {
            this.setState({
                sessionState: 'error',
                errorMessage: 'Unable to delete this project, please  try again',
            });
        }
        this.setState({ isBusy: false });
    };

    _handleRenameProject = async (pid: string) => {
        if (this.state.isBusy) return;
        EventReporter.reportEvent('project', 'rename', pid);
        const new_project_name = prompt('Please input a new name for your project?');
        if (new_project_name != null && new_project_name.length > 0) {
            this.setState({ isBusy: true });
            try {
                const repoArrayBuffer = await this.storage.get('manifest', pid);
                if (!repoArrayBuffer) {
                    throw `Manifest File not found ${pid}`;
                }
                const response_json = arrayBufferToJson(repoArrayBuffer);
                response_json.name = new_project_name;
                await this.storage.put('manifest', pid, new Blob([JSON.stringify(response_json)]));
                await this._listProjects();
            } catch {
                this.setState({
                    sessionState: 'error',
                    errorMessage: 'Unable to rename this project, please  try again',
                });
            }
            this.setState({ isBusy: false });
        }
    };

    _handleDownloadProject = async (pid: string) => {
        if (this.state.isBusy) return;
        EventReporter.reportEvent('project', 'download');
        this.setState({ isBusy: true });
        try {
            const zipobj = new JSZip();
            const repoArrayBuffer = await this.storage.get('manifest', pid);
            if (!repoArrayBuffer) {
                throw `Manifest not found ${pid}`;
            }
            const response_json = arrayBufferToJson(repoArrayBuffer);
            const fileEntries = response_json.fileEntries;
            for (const entry of fileEntries) {
                if (entry.type === 'file') {
                    const fileBuffer = await this.storage.get('asset', entry.id);
                    if (!fileBuffer) {
                        throw `Asset not found ${entry.id}`;
                    }
                    zipobj.file(entry.path, fileBuffer);
                }
            }
            const blobFile = await zipobj.generateAsync({ type: 'blob' });
            triggerDownloadBlob(blobFile, pid + '.zip');
        } catch {
            this.setState({
                sessionState: 'error',
                errorMessage: 'Unable to download this project, please  try again',
            });
        }
        this.setState({ isBusy: false });
    };

    _popupEachProject = async (repoID: string) => {
        const repoArrayBuffer = await this.storage.get('manifest', repoID);
        if (!repoArrayBuffer) {
            throw 'Manifest not found';
        }
        const entry = arrayBufferToJson(repoArrayBuffer) as ProjectEntry;
        entry.pid = repoID;
        return entry;
    };

    _listProjects = async () => {
        EventReporter.reportEvent('project', 'list');
        this.setState({ projects: [] });
        const repos = await this.storage.list('manifest');
        repos.sort((a, b) => {
            const timestampA = Date.parse(a.modifiedTime);
            const timestampB = Date.parse(b.modifiedTime);
            return timestampA < timestampB ? 1 : -1;
        });

        const promises = repos.map((r) => this._popupEachProject(r.itemKey));
        while (promises.length) {
            // 10 at at time
            const batchResults = await Promise.allSettled(promises.splice(0, 10));
            const projects: ProjectEntry[] = [];
            for (let item of batchResults) {
                if (item.status === 'fulfilled') {
                    projects.push(item.value);
                }
            }
            this.setState((state: State) => ({
                projects: state.projects.concat(projects),
            }));
        }
    };

    _handleHideModal = () => {
        this.setState({ currentModal: undefined });
    };

    _handleOpenModal = () => {
        if (this.state.isBusy) return;
        this.setState({ currentModal: 'create' });
    };

    prepareDefaultFileEntries = async () => {
        const fid = genFileID('main.tex');
        const url = await this.storage.put('asset', fid, new Blob([DEFAULT_CODE]));
        return [
            {
                id: fid,
                path: 'main.tex',
                uri: url,
                type: 'file',
                asset: false,
            },
        ];
    };

    recursiveCreateParent = (path: string, parents: string[]) => {
        const parentPath = getParentPath(path);
        if (!parentPath) {
            return;
        }
        if (parents.includes(parentPath)) {
            return;
        }
        parents.push(parentPath);
        this.recursiveCreateParent(parentPath, parents);
    };

    prepareFileEntries = async (user_zip_blob: Blob | undefined, remote_zip_url: string) => {
        let zip_blob = user_zip_blob;
        if (remote_zip_url) {
            const response = await fetch(remote_zip_url);
            if (response.ok) {
                zip_blob = await response.blob();
            }
        }
        if (!zip_blob) {
            return this.prepareDefaultFileEntries();
        }

        try {
            const importedEntries = [];
            const allowed_extension = [
                '.tex',
                '.bib',
                '.bst',
                '.cls',
                '.pdf',
                '.jpg',
                '.png',
                '.txt',
                '.bbx',
                '.sty',
                '.ttf',
                '.otf',
            ];
            const zip_handle = await JSZip.loadAsync(zip_blob);
            const queues: any = [];
            zip_handle.forEach(function(relativePath, zipEntry) {
                if (zipEntry.dir) {
                    return;
                }
                if ((zipEntry as any).size > 1024 * 1024 * 2) {
                    return;
                }
                let legalExtension = false;
                allowed_extension.some((ext: string) => {
                    if (zipEntry.name.endsWith(ext)) {
                        legalExtension = true;
                        return true;
                    }
                    return false;
                });
                if (!legalExtension) {
                    return;
                }
                queues.push({ path: relativePath, entry: zipEntry });
            });

            let singleRoot = true;
            const parents: any = [];

            for (const item of queues) {
                this.recursiveCreateParent(item.path, parents);
            }

            if (parents.length === 0) {
                singleRoot = false;
            } else {
                for (const item of queues) {
                    if (!item.path.startsWith(parents[0])) {
                        singleRoot = false;
                    }
                }
            }

            if (singleRoot) {
                // console.log('Single Root');
                const root = parents.pop();
                for (let j = 0; j < parents.length; j++) {
                    parents[j] = parents[j].slice(root.length + 1);
                }
                for (const item of queues) {
                    item.path = item.path.slice(root.length + 1);
                }
            }

            for (const item of queues) {
                const isTextEntry = /\.(cls|bib|txt|tex|bst|sty|bbx|md|js|html)$/.test(item.path);
                const randomID = genFileID(item.path);
                const zipEntryBlob = await item.entry.async('blob');
                const uri = await this.storage.put('asset', randomID, zipEntryBlob);
                const entry = {
                    id: randomID,
                    path: item.path,
                    uri: uri,
                    type: 'file',
                    asset: !isTextEntry,
                };
                importedEntries.push(entry);
            }

            for (let j = 0; j < parents.length; j++) {
                const entry = {
                    id: genRandomString(10),
                    path: parents[j],
                    uri: '',
                    type: 'folder',
                    asset: false,
                };
                importedEntries.push(entry);
            }

            // console.log(importedEntries);
            return importedEntries;
        } catch (e) {
            console.error('Corrupted Zip ' + e.message);
            return this.prepareDefaultFileEntries();
        }
    };

    _handleCreateProj = async () => {
        if (this.state.isBusy) return false;
        if (!this.projectNameRef.current) return false;
        EventReporter.reportEvent('project', 'create');
        this.setState({ isBusy: true, currentModal: undefined });
        try {
            let title = this.projectNameRef.current.value;
            if (!title) {
                title = 'My fancy project';
            }
            const selectedFiles = this.fileRef.current!.files;
            let zipFile;
            if (selectedFiles && selectedFiles.length > 0) {
                zipFile = selectedFiles[0];
            }
            const templateInput = this.selectRef.current!.value;
            let templateUrl = '';
            if (templateInput === 'Beamer') {
                templateUrl = 'boilerplate/beamer.zip';
            } else if (templateInput === 'CV') {
                templateUrl = 'boilerplate/cv.zip';
            } else if (templateInput === 'IEEE') {
                templateUrl = 'boilerplate/ieee.zip';
            } else if (templateInput === 'ACM') {
                templateUrl = 'boilerplate/acm.zip';
            } else if (templateInput === 'Tkiz') {
                templateUrl = 'boilerplate/tkiz.zip';
            }
            const entries = await this.prepareFileEntries(zipFile, templateUrl);

            let mainEntry = 'main.tex';
            for (let ent of entries) {
                if (ent.path.endsWith('.tex') && !ent.path.includes('/')) {
                    mainEntry = ent.path;
                    /* Prefer the following main entry */
                    if (ent.path === 'main.tex' || ent.path === 'manuscript.tex' || ent.path === 'paper.tex') {
                        break;
                    }
                }
            }

            const userInfo = await this.storage.getUserInfo();

            const pid = genFileID(title);
            const newProject = {
                name: title,
                username: userInfo.username,
                modifiedTime: new Date().toString(),
                fileEntries: entries,
                entryPoint: mainEntry,
                pid: pid,
            };
            await this.storage.put('manifest', pid, new Blob([JSON.stringify(newProject)]));
            await this._listProjects();
        } catch {
            this.setState({
                sessionState: 'error',
                errorMessage: 'Unable to create the project. Please  try again',
            });
        }
        this.setState({ isBusy: false });
        return false;
    };

    _renderErrorPage() {
        return <div className="container">
            <section className="header">
                <h2 className="title">Oops: {this.state.errorMessage}</h2>
            </section>
            <div className="navbar-spacer"/>
            <nav className="navbar">
                <div className="container">
                    <ul className="navbar-list">
                        <li className="navbar-item navbar-action ">
                            <a className="navbar-link" href="#" onClick={() => {
                                window.location.reload();
                            }}>
                                Try again?
                            </a>
                        </li>
                        <li className="navbar-item navbar-action ">
                            <a className="navbar-link" href="#" onClick={this._handleLogout}>
                                Go back to home page.
                            </a>
                        </li>
                    </ul>
                </div>
            </nav>
        </div>;
    }

    _renderImportPage() {
        return <div className="container">
            <section className="header">
                <h2 className="title">Importing a project...</h2>
            </section>
            {this.state.isBusy ? <ProgressIndicator/> : null}
        </div>;
    }

    _renderProjectPage() {
        const items = [];

        for (const project of this.state.projects) {
            if (project.deleted) {
                continue;
            }
            items.push(
                <tr key={project.pid}>
                    <td>{project.name}</td>
                    <td>{project.username}</td>
                    <td>{new Date(project.modifiedTime).toDateString()}</td>
                    <td>
                        <a
                            className="button actionButton openButton"
                            href="#"
                            onClick={() => this._handleOpenProject(project.pid)}>
                            {' '}
                            Open
                        </a>
                        <a
                            className="button actionButton deleteButton"
                            href="#"
                            onClick={() => this._handleDeleteProject(project.pid)}>
                            {' '}
                            Delete
                        </a>
                        <a
                            className="button actionButton renameButton"
                            href="#"
                            onClick={() => this._handleRenameProject(project.pid)}>
                            {' '}
                            Rename
                        </a>
                        <a
                            className="button actionButton downloadButton"
                            href="#"
                            onClick={() => this._handleDownloadProject(project.pid)}>
                            {' '}
                            Archive
                        </a>
                    </td>
                </tr>,
            );
        }

        let createDialog = null;
        if (this.state.currentModal === 'create') {
            createDialog = (
                <div className="modal">
                    <div className="modal-content">
                        <span className="close-button" onClick={this._handleHideModal}>
                            &times;
                        </span>
                        <form className="createProjectForm">
                            <div className="row">
                                <div className="u-full-width">
                                    <label htmlFor="projectTitleInput">Project Title:</label>
                                    <input
                                        ref={this.projectNameRef}
                                        className="u-full-width"
                                        type="text"
                                        placeholder="My Fancy Project"
                                        id="projectTitleInput"
                                    />
                                </div>
                            </div>
                            <div className="row">
                                <div className="u-full-width">
                                    <label htmlFor="zipInput">Upload a zip:</label>
                                    <input
                                        ref={this.fileRef}
                                        className="button-primary"
                                        type="file"
                                        accept=".zip"
                                        id="zipInput"
                                    />
                                </div>
                            </div>
                            <div className="u-full-width">
                                <label htmlFor="templateInput">Or select a boilerplate</label>
                                <select
                                    className="u-full-width"
                                    id="templateInput"
                                    ref={this.selectRef}>
                                    <option value="Article">Article</option>
                                    <option value="IEEE">IEEE Conference Proceedings</option>
                                    <option value="ACM">ACM Master Article Template</option>
                                    <option value="Beamer">Beamer</option>
                                    <option value="CV">CV</option>
                                    <option value="Tkiz">Tkiz/PGF</option>
                                </select>
                            </div>
                            <input
                                className="button-primary"
                                type="submit"
                                value="Submit"
                                onClick={this._handleCreateProj}
                            />
                        </form>
                    </div>
                </div>
            );
        }

        return (
            <div className="container">
                <section className="header">
                    <h2 className="title">Your LaTeX Projects:</h2>
                </section>
                {this.state.isBusy ? <ProgressIndicator/> : null}
                <div className="navbar-spacer"/>
                <nav className="navbar">
                    <div className="container">
                        <ul className="navbar-list">
                            <li className="navbar-item navbar-action ">
                                <a className="navbar-link" href="#" onClick={this._handleOpenModal}>
                                    Create New Project
                                </a>
                            </li>
                            <li className="navbar-item navbar-action ">
                                <a className="navbar-link" href="#" onClick={this._handleLogout}>
                                    Logout
                                </a>
                            </li>
                        </ul>
                    </div>
                </nav>

                <table className="u-full-width">
                    <thead>
                    <tr>
                        <th>Name</th>
                        <th>Author</th>
                        <th>Modified Time</th>
                        <th>Actions</th>
                    </tr>
                    </thead>
                    <tbody id="project-list">{items}</tbody>
                </table>
                {createDialog}
            </div>
        );
    }

    render() {
        if (this.state.sessionState === 'error') {
            return this._renderErrorPage();
        } else if (this.state.sessionState === 'import') {
            return this._renderImportPage();
        } else {
            return this._renderProjectPage();
        }
    }
}

ReactDOM.render(<Project/>, document.getElementById('root'));
