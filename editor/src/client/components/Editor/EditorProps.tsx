import { FileManagerEntry, Annotation } from '../../types';
import * as monaco from 'monaco-editor';

export type EditorProps = {
    entries: FileManagerEntry[];
    path: string;
    value: string;
    onValueChange: (value: string, path: string) => void;
    onCursorChange: (path: string, line: number, column: number) => void;
    onKeyStroke: (delta: string, isInsert: boolean) => void;
    onSubmitYJSEditingEvent: (e: monaco.editor.IModelContentChangedEvent) => void;
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
