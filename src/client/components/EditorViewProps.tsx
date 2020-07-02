import {
  Annotation,
  FileSystemEntry,
  SaveStatus,
    EngineVersion
} from '../types';


export type EditorViewProps = {
  saveStatus: SaveStatus;
  fileEntries: FileSystemEntry[];
  entry: FileSystemEntry | undefined;
  name: string;
  isSystemBusy: boolean;
  deviceErrors: Annotation[] | undefined;
  engineLogs: string;
  sendCodeOnChangeEnabled: boolean;
  onSendCode: () => void;
  onExportPDF: () => Promise<void>;
  onSubmitTitle: (name: string) => Promise<void>;
  onToggleSendCode: () => void;
  onClearDeviceLogs: () => void;
  onFileEntriesChange: (entries: FileSystemEntry[]) => Promise<void>;
  onChangeCode: (code: string, path: string) => void;
  onChangeCursor: (path: string, line: number, column: number) => void;
  onDownloadAsync: () => Promise<void>;
  uploadFileAsync: (file: File, id: string) => Promise<string>;
  previewRef: React.MutableRefObject<Window | null>;
  entryPoint: string;
  onSetEntryPoint: (path: string) => void;
  onTypeContent: (delta: string, isInsert: boolean) => void;
  onShareProject: () => Promise<void>;
  engine: EngineVersion;
  onChangeEngineVersion: (engine: EngineVersion) => Promise<void>;
};
