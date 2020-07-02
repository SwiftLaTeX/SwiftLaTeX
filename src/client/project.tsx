import * as React from 'react';
import ReactDOM from 'react-dom';
import ProgressIndicator from './components/shared/ProgressIndicator';
import { initializeStorage, BackendStorage } from './storage';
import {
    arrayBufferToJson,
    genRandomString,
    triggerDownloadBlob,
    getParentPath,
    genRandomFileID,
} from './utils/fileUtilities';
import JSZip from 'jszip';
import { EventReporter } from './utils/eventReport';

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


type FileEntry = {
    'id': string;
    'path': string;
    'uri': string;
    'type': 'folder' | 'file';
    'asset': string;

}
type ProjectEntry = {
    'name': string;
    'username': string;
    'modifiedTime': string;
    'fileEntries': FileEntry[];
    'entryPoint': string;
    'pid': string;
    'shared'?: boolean;
    'deleted'?: boolean;
}

type Props = {};

type State = {
    projects: ProjectEntry[];
    isBusy: boolean
    currentModal: 'create' | undefined;
};

class Project extends React.PureComponent<Props, State> {
    constructor(props: Props) {
        super(props);
        this.state = {
            projects: [],
            isBusy: true,
            currentModal: undefined,
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
            alert('Not signed in yet');
            this._handleLogout();
            return;
        }

        this._listProjects().then(_ => {

        }).catch(_ => {
            alert('Unable to fetch projects, please refresh and try again');
        }).finally(() => {
            this.setState({ isBusy: false });
        });

    }

    componentDidUpdate(_: Props, __: State) {

    }

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
            const response_json = arrayBufferToJson(repoArrayBuffer);
            response_json['deleted'] = true;
            await this.storage.put('manifest', pid, new Blob([JSON.stringify(response_json)]));
            await this._listProjects();
        } catch {
            alert('Unable to delete this project, please refresh and try again');
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
                const response_json = arrayBufferToJson(repoArrayBuffer);
                response_json['name'] = new_project_name;
                await this.storage.put('manifest', pid, new Blob([JSON.stringify(response_json)]));
                await this._listProjects();
            } catch {
                alert('Unable to rename this project, please refresh and try again');
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
            const response_json = arrayBufferToJson(repoArrayBuffer);
            const fileEntries = response_json['fileEntries'];
            for (const entry of fileEntries) {
                if (entry.type === 'file') {
                    const fileBuffer = await this.storage.get('asset', entry.id);
                    zipobj.file(entry.path, fileBuffer);
                }
            }
            const blobFile = await zipobj.generateAsync({ type: 'blob' });
            triggerDownloadBlob(blobFile, pid + '.zip');
        } catch {
            alert('Unable to rename this project, please refresh and try again');
        }
        this.setState({ isBusy: false });

    };

    _popupEachProject = async (repoID: string) => {
        const repoArrayBuffer = await this.storage.get('manifest', repoID);
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

        const promises = repos.map(r => this._popupEachProject(r.itemKey));
        while (promises.length) {
            // 10 at at time
            const projects: ProjectEntry[] = await Promise.all(promises.splice(0, 10));
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
        const fid = genRandomFileID('main.tex');
        const url = await this.storage.put('asset', fid, new Blob([DEFAULT_CODE]));
        return [
            {
                'id': fid,
                'path': 'main.tex',
                'uri': url,
                'type': 'file',
                'asset': false,
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
            let importedEntries = [];
            const allowed_extension = ['.tex', '.bib', '.bst', '.cls', '.pdf', '.jpg', '.png', '.txt', '.bbx', '.sty', '.ttf', '.otf'];
            const zip_handle = await JSZip.loadAsync(zip_blob);
            let queues: any = [];
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
            let parents: any = [];

            for (let item of queues) {
                this.recursiveCreateParent(item.path, parents);
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
                // console.log('Single Root');
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
                const randomID = genRandomFileID(item.path);
                const zipEntryBlob = await item.entry.async('blob');
                const uri = await this.storage.put('asset', randomID, zipEntryBlob);
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
                    'id': genRandomString(10),
                    'path': parents[j],
                    'uri': '',
                    'type': 'folder',
                    'asset': false,
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
            let title = this.projectNameRef.current!.value;
            if (!title) {
                title = 'My fancy project';
            }
            const selectedFiles = this.fileRef.current!.files;
            let zipFile = undefined;
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
            const userInfo = await this.storage.getUserInfo();

            let pid = genRandomFileID(title);
            let newProject = {
                'name': title,
                'username': userInfo.username,
                'modifiedTime': new Date().toString(),
                'fileEntries': entries,
                'entryPoint': 'main.tex',
                'pid': pid,
            };
            await this.storage.put('manifest', pid, new Blob([JSON.stringify(newProject)]));
            await this._listProjects();
        } catch {
            alert('Unable to create the project. Please refresh and try again');
        }
        this.setState({ isBusy: false });
        return false;
    };

    render() {

        const items = [];

        for (const project of this.state.projects) {
            if (project['deleted']) {
                continue;
            }
            items.push(<tr key={project.pid}>
                <td>{project.name}</td>
                <td>{project.username}</td>
                <td>{new Date(project.modifiedTime).toDateString()}</td>
                <td>
                    <a className="button actionButton openButton" href="#"
                       onClick={() => this._handleOpenProject(project.pid)}> Open</a>
                    <a className="button actionButton deleteButton" href="#"
                       onClick={() => this._handleDeleteProject(project.pid)}> Delete</a>
                    <a className="button actionButton renameButton" href="#"
                       onClick={() => this._handleRenameProject(project.pid)}> Rename</a>
                    <a className="button actionButton downloadButton" href="#"
                       onClick={() => this._handleDownloadProject(project.pid)}> Archive</a>
                </td>
            </tr>);
        }

        let createDialog = null;
        if (this.state.currentModal === 'create') {
            createDialog = <div className="modal">
                <div className="modal-content">
                    <span className="close-button" onClick={this._handleHideModal}>&times;</span>
                    <form className="createProjectForm">
                        <div className="row">
                            <div className="u-full-width">
                                <label htmlFor="projectTitleInput">Project Title:</label>
                                <input ref={this.projectNameRef} className="u-full-width" type="text"
                                       placeholder="My Fancy Project"
                                       id="projectTitleInput"/>
                            </div>
                        </div>
                        <div className="row">
                            <div className="u-full-width">
                                <label htmlFor="zipInput">Upload a zip:</label>
                                <input ref={this.fileRef} className="button-primary" type="file" accept=".zip"
                                       id="zipInput"/>
                            </div>
                        </div>
                        <div className="u-full-width">
                            <label htmlFor="templateInput">Or select a boilerplate</label>
                            <select className="u-full-width" id="templateInput" ref={this.selectRef}>
                                <option value="Article">Article</option>
                                <option value="IEEE">IEEE Conference Proceedings</option>
                                <option value="ACM">ACM Master Article Template</option>
                                <option value="Beamer">Beamer</option>
                                <option value="CV">CV</option>
                                <option value="Tkiz">Tkiz/PGF</option>
                            </select>
                        </div>
                        <input className="button-primary" type="submit" value="Submit"
                               onClick={this._handleCreateProj}/>
                    </form>
                </div>
            </div>;
        }

        return (
            <div className='container'>

                <section className="header">
                    <h2 className="title">Your LaTeX Projects:</h2>
                </section>
                {this.state.isBusy ? <ProgressIndicator/> : null}
                <div className="navbar-spacer"/>
                <nav className="navbar">
                    <div className="container">
                        <ul className="navbar-list">
                            <li className="navbar-item navbar-action "><a className="navbar-link" href="#"
                                                                          onClick={this._handleOpenModal}>Create
                                New
                                Project</a></li>
                            <li className="navbar-item navbar-action "><a className="navbar-link" href="#"
                                                                          onClick={this._handleLogout}>Logout</a>
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
                    <tbody id="project-list">
                    {items}
                    </tbody>
                </table>

                {createDialog}

            </div>
        );
    }
}

ReactDOM.render(<Project/>, document.getElementById('root'));
