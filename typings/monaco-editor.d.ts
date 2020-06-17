declare module 'monaco-editor/esm/vs/editor/editor.main' {
  export * from 'monaco-editor';
}

declare module 'monaco-editor/esm/vs/editor/standalone/browser/simpleServices' {
  export const SimpleEditorModelResolverService: any;
}

declare module 'monaco-editor/esm/vs/editor/standalone/browser/standaloneServices' {
  export const StaticServices: {
    codeEditorService: {
      get(): any,
    },
  };
}
