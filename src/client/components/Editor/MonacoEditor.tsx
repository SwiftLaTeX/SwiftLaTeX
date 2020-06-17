import * as React from 'react';
import { StyleSheet, css } from 'aphrodite';
import classnames from 'classnames';
import debounce from 'lodash/debounce';
// import mapValues from 'lodash/mapValues';
import 'monaco-editor/esm/vs/editor/browser/controller/coreCommands.js';
// import 'monaco-editor/esm/vs/editor/browser/widget/codeEditorWidget.js';
import 'monaco-editor/esm/vs/editor/contrib/bracketMatching/bracketMatching.js';
import 'monaco-editor/esm/vs/editor/contrib/caretOperations/caretOperations.js';
// import 'monaco-editor/esm/vs/editor/contrib/caretOperations/transpose.js';
import 'monaco-editor/esm/vs/editor/contrib/clipboard/clipboard.js';
// import 'monaco-editor/esm/vs/editor/contrib/codelens/codelensController.js';
// import 'monaco-editor/esm/vs/editor/contrib/colorPicker/colorDetector.js';
import 'monaco-editor/esm/vs/editor/contrib/comment/comment.js';
import 'monaco-editor/esm/vs/editor/contrib/contextmenu/contextmenu.js';
import 'monaco-editor/esm/vs/editor/contrib/cursorUndo/cursorUndo.js';
// import 'monaco-editor/esm/vs/editor/contrib/dnd/dnd.js';
import 'monaco-editor/esm/vs/editor/contrib/find/findController.js';
// import 'monaco-editor/esm/vs/editor/contrib/folding/folding.js';
// import 'monaco-editor/esm/vs/editor/contrib/format/formatActions.js';
// import 'monaco-editor/esm/vs/editor/contrib/goToDeclaration/goToDeclarationCommands.js';
// import 'monaco-editor/esm/vs/editor/contrib/goToDeclaration/goToDeclarationMouse.js';
// import 'monaco-editor/esm/vs/editor/contrib/gotoError/gotoError.js';
// import 'monaco-editor/esm/vs/editor/contrib/hover/hover.js';
import 'monaco-editor/esm/vs/editor/contrib/inPlaceReplace/inPlaceReplace.js';
import 'monaco-editor/esm/vs/editor/contrib/linesOperations/linesOperations.js';
// import 'monaco-editor/esm/vs/editor/contrib/links/links.js';
// import 'monaco-editor/esm/vs/editor/contrib/multicursor/multicursor.js';
// import 'monaco-editor/esm/vs/editor/contrib/parameterHints/parameterHints.js';
// import 'monaco-editor/esm/vs/editor/contrib/quickFix/quickFixCommands.js';
// import 'monaco-editor/esm/vs/editor/contrib/referenceSearch/referenceSearch.js';
// import 'monaco-editor/esm/vs/editor/contrib/rename/rename.js';
// import 'monaco-editor/esm/vs/editor/contrib/smartSelect/smartSelect.js';
// import 'monaco-editor/esm/vs/editor/contrib/snippet/snippetController2.js';
// import 'monaco-editor/esm/vs/editor/contrib/suggest/suggestController.js';
// import 'monaco-editor/esm/vs/editor/contrib/toggleTabFocusMode/toggleTabFocusMode.js';
import 'monaco-editor/esm/vs/editor/contrib/wordHighlighter/wordHighlighter.js';
import 'monaco-editor/esm/vs/editor/contrib/wordOperations/wordOperations.js';
// import 'monaco-editor/esm/vs/editor/standalone/browser/accessibilityHelp/accessibilityHelp.js';
// import 'monaco-editor/esm/vs/editor/standalone/browser/inspectTokens/inspectTokens.js';
// import 'monaco-editor/esm/vs/editor/standalone/browser/iPadShowKeyboard/iPadShowKeyboard.js';
// import 'monaco-editor/esm/vs/editor/standalone/browser/quickOpen/quickOutline.js';
// import 'monaco-editor/esm/vs/editor/standalone/browser/quickOpen/gotoLine.js';
// import 'monaco-editor/esm/vs/editor/standalone/browser/quickOpen/quickCommand.js';
// import 'monaco-editor/esm/vs/editor/standalone/browser/toggleHighContrast/toggleHighContrast.js';
import * as monaco from 'monaco-editor/esm/vs/editor/editor.api';
// import { SimpleEditorModelResolverService } from 'monaco-editor/esm/vs/editor/standalone/browser/simpleServices';
import { StaticServices } from 'monaco-editor/esm/vs/editor/standalone/browser/standaloneServices';
import { light, dark } from './themes/monaco';
import overrides from './themes/monaco-overrides';
import { ThemeName } from '../Preferences/withThemeName';
import ResizeDetector from '../shared/ResizeDetector';
// import prettierCode from '../../utils/prettierCode';
// import getRelativePath from '../../utils/getRelativePath';
// import getFileLanguage from '../../utils/getFileLanguage';
// import { SDKVersion } from '../../configs/sdk';
import { Annotation } from '../../types';
import { EditorProps } from './EditorProps';
import { Registry } from 'monaco-textmate'; // peer dependency
import { wireTmGrammars } from 'monaco-editor-textmate';
import { MonacoYJSBinding } from './MonacoYjs';

/**
 * Monkeypatch to make 'Find All References' work across multiple files
 * https://github.com/Microsoft/monaco-editor/issues/779#issuecomment-374258435
 */
// SimpleEditorModelResolverService.prototype.findModel = function(_: any, resource: any) {
//   return monaco.editor.getModels().find(model => model.uri.toString() === resource.toString());
// };

// @ts-ignore
global.MonacoEnvironment = {
    getWorkerUrl(_: string, __: string) {
        return 'dist/editor.worker.bundle.js';
    },
};

monaco.editor.defineTheme('light', light as any);
monaco.editor.defineTheme('dark', dark as any);
monaco.languages.register({ id: 'bibtex', extensions: ['.bib'] });
monaco.languages.register({ id: 'latex', extensions: ['.tex'] });

type Props = EditorProps & {
    theme: ThemeName;
};

// Store editor states such as cursor position, selection and scroll position for each model
const editorStates = new Map<string, monaco.editor.ICodeEditorViewState | undefined | null>();

const codeEditorService = StaticServices.codeEditorService.get();

const findModel = (path: string) => {
    return monaco.editor.getModels().find(model => model.uri.path === `/${path}`);
}

class MonacoEditor extends React.Component<Props> {

    static defaultProps: Partial<Props> = {
        lineNumbers: 'on',
        wordWrap: 'on',
        scrollBeyondLastLine: false,
        minimap: {
            enabled: false,
        },
        fontFamily: 'var(--font-monospace)',
        fontLigatures: true,
    };

    static removePath(path: string) {
        // Remove editor states
        editorStates.delete(path);

        // Remove associated models
        const model = findModel(path);

        if (model) {
            // console.error('disposing ' + path);
            model.dispose();
        }
    }

    static renamePath(oldPath: string, newPath: string) {
        const selection = editorStates.get(oldPath);

        editorStates.delete(oldPath);
        editorStates.set(newPath, selection);

        this.removePath(oldPath);
    }

    setCursor(line: number, column: number) {
        if (this._editor) {
            this._editor.setPosition({ lineNumber: line, column: column });
            this._editor.revealLine(line);
            this._editor.focus();
        }
    }

    componentDidMount() {

        const { path, value, annotations, autoFocus, ...rest } = this.props;

        // The methods provided by the service are on it's prototype
        // So spreading this object doesn't work, we must mutate it
        codeEditorService.openCodeEditor = async (
            { resource, options }: any,
            editor: monaco.editor.IStandaloneCodeEditor,
        ) => {
            // Remove the leading slash added by the Uri
            await this.props.onOpenPath(resource.path.replace(/^\//, ''));

            editor.setSelection(options.selection);
            editor.revealLine(options.selection.startLineNumber);

            return {
                getControl: () => editor,
            };
        };

        const editor = monaco.editor.create(
            this._node.current as HTMLDivElement,
            rest,
            codeEditorService,
        );

        // @ts-ignore

        const registry = new Registry({
            getGrammarDefinition: async (scopeName: string) => {
                // console.log(scopeName);
                if (scopeName === 'text.tex.latex') {
                    return {
                        format: 'json',
                        content: await (await fetch('bin/LaTeX.tmLanguage.json')).text(),
                    };
                } else if (scopeName === 'text.bibtex') {
                    return {
                        format: 'json',
                        content: await (await fetch('bin/Bibtex.tmLanguage.json')).text(),
                    };
                } else if (scopeName === 'text.tex') {
                    return {
                        format: 'json',
                        content: await (await fetch('bin/TeX.tmLanguage.json')).text(),
                    };
                } else {
                    throw new Error('Unexpected grammar ' + scopeName);
                }
            },
        });
        const grammars = new Map();
        grammars.set('bibtex', 'text.bibtex');
        grammars.set('latex', 'text.tex.latex');
        wireTmGrammars(monaco, registry, grammars, editor).then(
            function() {
                console.log('Grammer loaded');
            },
            function(err) {
                console.log('Unabled to load grammer ' + err);
            },
        );

        this._contentSubscription = editor.onDidChangeModelContent((_) => {
            const model = editor.getModel();

            if (model) {
                const value = model.getValue();
                const path = model.uri.path.substr(1);
                if (value !== this.props.value) {
                    this.props.onValueChange(value, path);
                }
            }
        });

        this._cursorSubscription = editor.onDidChangeCursorPosition((e) => {
            const model = editor.getModel();

            if (model) {
                const path = model.uri.path.substr(1);
                const column = e.position.column;
                const line = e.position.lineNumber;
                this.props.onCursorChange(path, line, column);
                // console.log('Path ' + path + ' ' + column + ' ' + line);
            }
        });

        this._editor = editor;

        this._openFile(path, value, autoFocus);
        this._updateMarkers(annotations);
    }

    componentDidUpdate(prevProps: Props) {
        const { path, value, annotations, autoFocus, theme, ...rest } = this.props;

        if (this._editor) {
            this._editor.updateOptions(rest);

            const model = this._editor.getModel();

            if (path !== prevProps.path) {
                // Save the editor state for the previous file so we can restore it when it's re-opened
                editorStates.set(prevProps.path, this._editor.saveViewState());
                this._openFile(path, value, autoFocus);
            } else if (model && value !== model.getValue()) {
                // @ts-ignore
                this._editor.executeEdits(null, [
                    {
                        range: model.getFullModelRange(),
                        text: value,
                    },
                ]);
            }
        }

        if (annotations !== prevProps.annotations) {
            this._updateMarkers(annotations);
        }

        if (theme !== prevProps.theme) {
            // Update the global editor theme
            // Monaco doesn't have a way to change theme locally
            monaco.editor.setTheme(theme);
        }

    }

    componentWillUnmount() {
        this._cursorSubscription && this._cursorSubscription.dispose();
        this._contentSubscription && this._contentSubscription.dispose();
        this._editor && this._editor.dispose();
    }

    _initializeFile = (path: string, value: string) => {
        let model = findModel(path);

        if (model && !model.isDisposed()) {
            // If a model exists, we need to update it's value
            // This is needed because the content for the file might have been modified externally
            // Use `pushEditOperations` instead of `setValue` or `applyEdits` to preserve undo stack
            // @ts-ignore
            model.pushEditOperations(
                [],
                [
                    {
                        range: model.getFullModelRange(),
                        text: value,
                    },
                ],
            );
        } else {
            model = monaco.editor.createModel(
                value,
                undefined,
                monaco.Uri.from({ scheme: 'file', path }),
            );

            model.updateOptions({
                tabSize: 2,
                insertSpaces: true,
            });

            new MonacoYJSBinding(model, this.props.onTypeContent);
        }
    };


    _openFile = (path: string, value: string, focus?: boolean) => {
        this._initializeFile(path, value);

        const model = findModel(path);

        if (this._editor && model) {
            this._editor.setModel(model);

            // Restore the editor state for the file
            const editorState = editorStates.get(path);

            if (editorState) {
                this._editor.restoreViewState(editorState);
            }

            if (focus) {
                this._editor.focus();
            }
        }
    };

    _updateMarkers = (annotations: Annotation[]) =>
        // @ts-ignore
        monaco.editor.setModelMarkers(this._editor.getModel(), null, annotations);

    _handleResize = debounce(() => this._editor && this._editor.layout(), 50, {
        leading: true,
        trailing: true,
    });

    _contentSubscription: monaco.IDisposable | undefined;
    _cursorSubscription: monaco.IDisposable | undefined;
    // _suppress: boolean = false;
    _editor: monaco.editor.IStandaloneCodeEditor | null = null;
    _node = React.createRef<HTMLDivElement>();
    _statusbar = React.createRef<HTMLDivElement>();

    render() {
        return (
            <div className={css(styles.container)}>
                <style type="text/css" dangerouslySetInnerHTML={{ __html: overrides }}/>
                <ResizeDetector onResize={this._handleResize}>
                    <div
                        ref={this._node}
                        className={classnames(
                            css(styles.editor),
                            'snack-monaco-editor',
                            `theme-${this.props.theme}`,
                        )}
                    />
                </ResizeDetector>
            </div>
        );
    }
}

export default MonacoEditor;

const styles = StyleSheet.create({
    container: {
        display: 'flex',
        flexDirection: 'row',
        flex: '1',
        minWidth: 0,
    },
    editor: {
        height: '100%',
        width: '100%',
    },
});
