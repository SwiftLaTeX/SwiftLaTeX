import { Annotation, FileManagerEntry, SaveStatus, EngineVersion } from '../types';
import * as monaco from 'monaco-editor';

export type EditorViewProps = {
    saveStatus: SaveStatus;
    fileEntries: FileManagerEntry[];
    entry: FileManagerEntry | undefined;
    name: string;
    isSystemBusy: boolean;
    deviceErrors: Annotation[] | undefined;
    engineLogs: string;
    sendCodeOnChangeEnabled: boolean;
    onSendCode: () => void;
    onExportPDF: () => Promise<void>;
    onSubmitTitle: (name: string) => Promise<void>;
    onToggleSendCode: () => void;
    onFileEntriesChange: (entries: FileManagerEntry[]) => Promise<void>;
    onChangeCode: (code: string, path: string) => void;
    onDownloadAsync: () => Promise<void>;
    uploadFileAsync: (file: File, id: string) => Promise<string>;
    previewRef: React.MutableRefObject<Window | null>;
    editorRef: React.MutableRefObject<any>;
    entryPoint: string;
    onSetEntryPoint: (path: string) => void;
    onShareProject: () => Promise<boolean>;
    engine: EngineVersion;
    onChangeEngineVersion: (engine: EngineVersion) => Promise<void>;
    onYJSEditingEvent: (e: monaco.editor.IModelContentChangedEvent) => void;
};
