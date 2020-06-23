declare module 'monaco-vim' {
  import { editor } from 'monaco-editor';

  export function initVimMode(
    editor: editor.IStandaloneCodeEditor,
    statusbar: HTMLElement
  ): { dispose: () => void };
}
