import * as React from 'react';
import { StyleSheet, css } from 'aphrodite';
import ProgressIndicator from './shared/ProgressIndicator';
import ContentShell from './Shell/ContentShell';
import LayoutShell from './Shell/LayoutShell';
import EditorToolbar from './EditorToolbar';
import EditorPanels from './EditorPanels';
import EditorFooter from './EditorFooter';
import AssetViewer from './AssetViewer';
import NoFileSelected from './NoFileSelected';
import FileList from './FileList/FileList';
import MonacoEditor from './Editor/MonacoEditor';
import openEntry from '../actions/openEntry';
import Banner from './shared/Banner';
import ModalDialog from './shared/ModalDialog';
import {
    isInsideFolder,
    changeParentPath,
} from '../utils/fileUtilities';
import withPreferences, { PreferencesContextType } from './Preferences/withPreferences';
import { FileSystemEntry, Annotation } from '../types';
import { EditorViewProps } from './EditorViewProps';
import ShareCode from './ShareCode';
import SplitterLayout from 'react-splitter-layout';
import './shared/Splitter.css';
import WebFrame from './WebFrame';


export type Props = PreferencesContextType &
    EditorViewProps;

type ModalName = 'shortcuts' | 'share' ;
type BannerName = 'no-entry' | 'entry-changed' | 'engine-changed';

type State = {
    currentModal: ModalName | null;
    currentBanner: BannerName | null;
    isDownloading: boolean;
    deviceLogsShown: boolean;
    shouldPreventRedirectWarning: boolean;
    previousEntry: FileSystemEntry | undefined;
    dragging: boolean;
};

const BANNER_TIMEOUT_SHORT = 3000;

// const BANNER_TIMEOUT_LONG = 15000;

class EditorView extends React.Component<Props, State> {
    state = {
        currentModal: null,
        currentBanner: null,
        isDownloading: false,
        deviceLogsShown: false,
        shouldPreventRedirectWarning: false,
        previousEntry: undefined,
        dragging: false,
    };

    componentDidMount() {
        this._EditorComponentRef = React.createRef();
        window.addEventListener('beforeunload', this._handleUnload);
    }

    componentDidUpdate(prevProps: Props, _: State) {
        if (this.props.entryPoint !== prevProps.entryPoint) {
            this._showBanner('entry-changed', BANNER_TIMEOUT_SHORT);
        }

        if (this.props.engine !== prevProps.engine) {
            this._showBanner('engine-changed', BANNER_TIMEOUT_SHORT);
        }
    }

    componentWillUnmount() {
        window.removeEventListener('beforeunload', this._handleUnload);
    }

    _handleUnload = (e: any) => {
        const isSaved =
            this.props.saveStatus === 'saved-draft' || this.props.saveStatus === 'published';

        if (isSaved || this.state.shouldPreventRedirectWarning) {
            this._allowRedirectWarning();
            return;
        }

        const message = 'You have unsaved changes. Are you sure you want to leave this page?';
        e.returnValue = message;
        return message;
    };


    _showBanner = (name: BannerName, duration: number) => {
        this.setState({ currentBanner: name });

        setTimeout(() => {
            // @ts-ignore
            this.setState(state => (state.currentBanner === name ? { currentBanner: null } : state));
        }, duration);
    };


    _handleOpenPath = (path: string): Promise<void> =>
        this.props.onFileEntriesChange(openEntry(this.props.fileEntries, path, true));

    _handlePreviewClick = async (path: string, line: number, column: number): Promise<void> => {
        await this.props.onFileEntriesChange(openEntry(this.props.fileEntries, path, true));
        this._EditorComponentRef.current && this._EditorComponentRef.current.setCursor(line, column);
        // console.log(line + ' ' + column);
    };

    _handleRemoveFile = (path: string) => {
        const entry = this.props.fileEntries.find(({ item }) => item.path === path);
        if (entry && entry.item.type === 'folder') {
            this.props.fileEntries.forEach(({ item }) => {
                if (isInsideFolder(item.path, path)) {
                    MonacoEditor.removePath(item.path);
                }
            });
        } else {
            MonacoEditor.removePath(path);
        }
    };

    _handleRenameFile = (oldPath: string, newPath: string) => {
        const entry = this.props.fileEntries.find(({ item }) => item.path === oldPath);
        if (entry && entry.item.type === 'folder') {
            this.props.fileEntries.forEach(({ item }) => {
                if (isInsideFolder(item.path, oldPath)) {
                    const renamedPath = changeParentPath(item.path, oldPath, newPath);
                    MonacoEditor.renamePath(item.path, renamedPath);
                }
            });
        } else {
            MonacoEditor.renamePath(oldPath, newPath);
        }
    };

    _EditorComponentRef: any;

    _showErrorPanel = () =>
        this.props.setPreferences({
            panelType: 'errors',
        });

    _showDeviceLogs = () =>
        this.props.setPreferences({
            panelType: 'logs',
        });

    _togglePanels = () =>
        this.props.setPreferences({
            panelsShown: !this.props.preferences.panelsShown,
        });

    _toggleFileTree = () =>
        this.props.setPreferences({
            fileTreeShown: !this.props.preferences.fileTreeShown,
        });

    _toggleDevicePreview = () => {
        this.props.setPreferences({
            devicePreviewShown: !this.props.preferences.devicePreviewShown,
        });
    };

    _toggleTheme = () =>
        this.props.setPreferences({
            theme: this.props.preferences.theme === 'light' ? 'dark' : 'light',
        });

    _preventRedirectWarning = () =>
        this.setState({
            shouldPreventRedirectWarning: true,
        });

    _allowRedirectWarning = () =>
        this.setState({
            shouldPreventRedirectWarning: false,
        });

    _handleShowShareCode = async () => {
        this.setState({ currentModal: 'share' });
    };

    _handleHideModal = () => {
        this.setState({ currentModal: null });
    };

    onDragStart = () => {
        this.setState({ dragging: true });
    };

    onDragEnd = () => {
        this.setState({ dragging: false });
    };


    render() {
        const { currentBanner, currentModal } = this.state;

        const {
            entry,
            saveStatus,
            sendCodeOnChangeEnabled,
            engineLogs,
            deviceErrors,
            onSendCode,
            // onReloadSnack,
            onClearDeviceLogs,
            onToggleSendCode,
            uploadFileAsync,
            preferences,
            name,
            previewRef,
            entryPoint,
            onSetEntryPoint,
            engine,
            onChangeEngineVersion,
        } = this.props;

        let annotations: Annotation[] = [];
        if (deviceErrors) {
            annotations = annotations.concat(deviceErrors);
        }
        let editorComponent;
        if (entry && entry.item.type === 'file') {
            if (entry.item.asset) {
                editorComponent = <AssetViewer entry={entry}/>;
            } else {
                const { content } = entry.item;
                editorComponent = <React.Fragment>
                    <MonacoEditor
                        entries={this.props.fileEntries}
                        autoFocus={!entry.state.isCreating}
                        annotations={annotations}
                        path={entry.item.path}
                        theme={this.props.preferences.theme}
                        value={content as string}
                        onValueChange={this.props.onChangeCode}
                        onCursorChange={this.props.onChangeCursor}
                        onOpenPath={this._handleOpenPath}
                        lineNumbers={'on'}
                        onTypeContent={this.props.onTypeContent}
                        ref={this._EditorComponentRef}
                    />
                </React.Fragment>;
            }
        } else {
            editorComponent = <NoFileSelected/>;
        }

        return (
            <ContentShell>
                {this.props.isSystemBusy ? <ProgressIndicator/> : null}
                <EditorToolbar
                    name={name}
                    saveStatus={saveStatus}
                    onSubmitTitle={this.props.onSubmitTitle}
                    onExportPDF={this.props.onExportPDF}
                    isSystemBusy={this.props.isSystemBusy}
                    onDownloadCode={this.props.onDownloadAsync}
                    onShowShareCode={this._handleShowShareCode}
                />
                <div className={css(styles.editorAreaOuterWrapper)}>
                    <div className={css(styles.editorAreaOuter)}>
                        <LayoutShell>
                            <FileList
                                visible={preferences.fileTreeShown}
                                entries={this.props.fileEntries}
                                onEntriesChange={this.props.onFileEntriesChange}
                                onRemoveFile={this._handleRemoveFile}
                                onRenameFile={this._handleRenameFile}
                                uploadFileAsync={uploadFileAsync}
                                preventRedirectWarning={this._preventRedirectWarning}
                                saveStatus={saveStatus}
                                entryPoint={entryPoint}
                                onSetEntryPoint={onSetEntryPoint}
                            />
                            <SplitterLayout onDragStart={this.onDragStart} onDragEnd={this.onDragEnd} percentage={true}
                                            secondaryInitialSize={45}>
                                <div className={css(styles.editorArea)}>{editorComponent}</div>
                                {preferences.devicePreviewShown && <div className={css(styles.previewerArea)}>
                                    {this.state.dragging && <div className="my-iframe-overlay"/>}
                                    <WebFrame engine={engine} previewRef={previewRef}
                                              onPreviewClick={this._handlePreviewClick}/></div>}
                            </SplitterLayout>


                        </LayoutShell>
                        {preferences.panelsShown ? (
                            <EditorPanels
                                annotations={annotations}
                                deviceLogs={engineLogs}
                                onShowErrorPanel={this._showErrorPanel}
                                onShowDeviceLogs={this._showDeviceLogs}
                                onTogglePanels={this._togglePanels}
                                onClearDeviceLogs={onClearDeviceLogs}
                                panelType={preferences.panelType}
                            />
                        ) : null}
                    </div>
                </div>
                <EditorFooter
                    annotations={annotations}
                    fileTreeShown={preferences.fileTreeShown}
                    devicePreviewShown={preferences.devicePreviewShown}
                    panelsShown={preferences.panelsShown}
                    sendCodeOnChangeEnabled={sendCodeOnChangeEnabled}
                    onSendCode={onSendCode}
                    // onReloadSnack={onReloadSnack}
                    engine={engine}
                    onChangeEngineVersion={onChangeEngineVersion}
                    onToggleTheme={this._toggleTheme}
                    onTogglePanels={this._togglePanels}
                    onToggleFileTree={this._toggleFileTree}
                    onToggleDevicePreview={this._toggleDevicePreview}
                    onToggleSendCode={onToggleSendCode}
                    // onShowShortcuts={this._handleShowShortcuts}
                    theme={this.props.preferences.theme}
                />
                <ModalDialog
                    className={css(styles.embedModal)}
                    autoSize={false}
                    visible={currentModal === 'share'}
                    onDismiss={this._handleHideModal}>
                    <ShareCode
                        theme={this.props.preferences.theme}
                    />
                </ModalDialog>

                <Banner type="success" visible={currentBanner === 'entry-changed'}>
                    Entry point is set to {this.props.entryPoint}
                </Banner>
                <Banner type="success" visible={currentBanner === 'engine-changed'}>
                    Switch to the {this.props.engine} typesetting
                    engine. {this.props.engine === 'PDFLaTeX' ? 'Note that PDFLaTeX engine has limited WYSIWYG functionalities' : ''}
                </Banner>
                <Banner type="error" visible={!this.props.entryPoint}>
                    No entry point detected, please choose a tex file as the entry point.
                </Banner>
            </ContentShell>
        );
    }
}

export default withPreferences(EditorView);

const styles = StyleSheet.create({
    editorAreaOuter: {
        display: 'flex',
        flex: 1,
        flexDirection: 'column',
        minWidth: 0,
        minHeight: 0,
    },

    editorAreaOuterWrapper: {
        display: 'flex',
        flex: 1,
        flexDirection: 'row',
        minHeight: 0,
        minWidth: 0,
    },

    embedModal: {
        minWidth: 0,
        minHeight: 0,
        maxWidth: 'calc(100% - 48px)',
        maxHeight: 'calc(100% - 48px)',
    },

    previewerArea: {
        display: 'flex',
        flex: 1,
        flexDirection: 'column',
    },

    editorArea: {
        display: 'flex',
        flex: 1,
        flexDirection: 'column',
        height: '100%',
        overflow: 'hidden',
    },

});
