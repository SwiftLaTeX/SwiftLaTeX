import { FileSystemEntry, Annotation } from '../../types';

export type EditorProps = {
    entries: FileSystemEntry[];
    path: string;
    value: string;
    onOpenPath: (path: string) => void;
    onValueChange: (value: string, path: string) => void;
    onCursorChange: (path: string, line: number, column: number) => void;
    onTypeContent: (delta: string, isInsert: boolean) => void;
    annotations: Annotation[];
    lineNumbers?: 'on' | 'off' | 'relative' | 'interval';
    wordWrap?: 'off' | 'on' | 'wordWrapColumn' | 'bounded';
    scrollBeyondLastLine?: boolean;
    minimap?: {
        enabled?: boolean;
        maxColumn?: number;
        renderCharacters?: boolean;
        showSlider?: 'always' | 'mouseover';
        side?: 'right' | 'left';
    };
    autoFocus?: boolean;
    fontFamily?: string;
    fontLigatures?: boolean;
};
