import * as React from 'react';
import { StyleSheet, css } from 'aphrodite';
import classnames from 'classnames';
import debounce from 'lodash/debounce';
import 'monaco-editor/esm/vs/editor/browser/controller/coreCommands.js';
import 'monaco-editor/esm/vs/editor/contrib/bracketMatching/bracketMatching.js';
import 'monaco-editor/esm/vs/editor/contrib/caretOperations/caretOperations.js';
import 'monaco-editor/esm/vs/editor/contrib/clipboard/clipboard.js';
import 'monaco-editor/esm/vs/editor/contrib/contextmenu/contextmenu.js';
import 'monaco-editor/esm/vs/editor/contrib/find/findController.js';
import 'monaco-editor/esm/vs/editor/contrib/hover/hover.js';
import 'monaco-editor/esm/vs/editor/contrib/suggest/suggestController.js';
import * as monaco from 'monaco-editor/esm/vs/editor/editor.api';
import { light, dark } from './themes/monaco';
import overrides from './themes/monaco-overrides';
import { ThemeName } from '../Preferences/withThemeName';
import { Annotation } from '../../types';
import { EditorProps } from './EditorProps';
import { Registry } from 'monaco-textmate'; // peer dependency
import { wireTmGrammars } from 'monaco-editor-textmate';
// import { MonacoYJSBinding } from './MonacoYjs';
import ResizeDetector from '../shared/ResizeDetector';
import { HunspellEngine } from './hunspell';
import { createMutex } from '../../utils/mutex';
import { YTextEvent } from 'yjs/dist/src/types/YText';
import { loadWASM } from 'onigasm';
import { laTeXCompletionProvider } from './LaTeXCompletionProvider';

// @ts-ignore
global.MonacoEnvironment = {
    getWorkerUrl(_: string, __: string) {
        return 'dist/editor.worker.bundle.js';
    },
};

monaco.editor.defineTheme('light', light as any);
monaco.editor.defineTheme('dark', dark as any);

type Props = EditorProps & {
    theme: ThemeName;
};

// Store editor states such as cursor position, selection and scroll position for each model
const editorStates = new Map<string, monaco.editor.ICodeEditorViewState | undefined | null>();

const findModel = (path: string) => {
    return monaco.editor.getModels().find((model) => {
        return model.uri.path === `/${path}`;
    });
};

class MonacoEditor extends React.Component<Props> {
    static hasHighlightingSetup = false;
    _hoverProvider: monaco.IDisposable | undefined;
    _spellCheck = debounce(this.spellCheckNoDebounce, 1000);
    _toBeSpellCheckedLines: number[] = [];
    _spellCheckEngine: HunspellEngine | undefined;
    _lastSpeckResult: Annotation[] = [];
    _completionProviderLaTeX: monaco.IDisposable | undefined;
    _editingMutex = createMutex();
    static defaultProps: Partial<Props> = {
        lineNumbers: 'on',
        wordWrap: 'on',
        scrollBeyondLastLine: false,
        minimap: {
            enabled: false,
        },
        fontFamily: 'var(--font-monospace)',
        fontLigatures: false,
    };

    /* Clear up models that are not longer used due to file tree changed */
    cleanUpModels(paths: string[]) {
        const models = monaco.editor.getModels().filter((model) => {
            return !paths.includes(model.uri.path.substr(1));
        });
        for (const model of models) {
            console.error('Cleaning up ' + model.uri.path);
            editorStates.delete(model.uri.path);
            model.dispose();
        }
    }

    /* Anti-react pattern, considering rewriting it */
    setCursorPosition(line: number, column: number) {
        if (this._editor) {
            this._editor.setPosition({ lineNumber: line, column });
            this._editor.revealLine(line);
            this._editor.focus();
        }
    }

    /* Anti-react pattern, considering rewriting it */
    async injectPeerEditingEvents(event: YTextEvent) {
        this._editingMutex(() => {
            if (!this._editor) return;
            const monacoModel = this._editor.getModel();
            if (!monacoModel) return;
            let index = 0;
            event.delta.forEach((op) => {
                if (op.retain !== undefined) {
                    index += op.retain;
                } else if ((op as any).insert !== undefined) {
                    const pos = monacoModel.getPositionAt(index);
                    const range = new monaco.Selection(
                        pos.lineNumber,
                        pos.column,
                        pos.lineNumber,
                        pos.column,
                    );
                    /* eslint-disable */
                    monacoModel.pushEditOperations(
                        [],
                        [{ range, text: (op as any).insert }],
                        () => null,
                    );
                    index += (op as any).insert.length;
                    /* eslint-enable */
                } else if (op.delete !== undefined) {
                    const pos = monacoModel.getPositionAt(index);
                    const endPos = monacoModel.getPositionAt(index + op.delete);
                    const range = new monaco.Selection(
                        pos.lineNumber,
                        pos.column,
                        endPos.lineNumber,
                        endPos.column,
                    );
                    /* eslint-disable */
                    monacoModel.pushEditOperations([], [{ range, text: '' }], () => null);
                    /* eslint-enable */
                } else {
                    throw new Error('Unexpected sync protocol');
                }
            });
            monacoModel.pushStackElement();
        });
    }

    static async _setupHighlighting() {
        if (MonacoEditor.hasHighlightingSetup) return;
        MonacoEditor.hasHighlightingSetup = true;
        monaco.languages.register({ id: 'bibtex', extensions: ['.bib'] });
        monaco.languages.register({ id: 'latex', extensions: ['.tex', '.cls', '.sty'] });
        monaco.languages.setLanguageConfiguration('latex', {
            comments: {
                lineComment: '%',
            },
            brackets: [
                ['{', '}'],
                ['[', ']'],
                ['(', ')'],
            ],
            autoClosingPairs: [
                { open: '{', close: '}', notIn: ['string'] },
                { open: '[', close: ']', notIn: ['string'] },
                { open: '(', close: ')', notIn: ['string'] },
                { open: '`', close: '`', notIn: ['string', 'comment'] },
                { open: '"', close: '"', notIn: ['string', 'comment'] },
                { open: '$', close: '$', notIn: ['string', 'comment'] },
            ],
            surroundingPairs: [
                { open: '{', close: '}' },
                { open: '[', close: ']' },
                { open: '(', close: ')' },
                { open: '`', close: '`' },
                { open: '"', close: '"' },
                { open: '$', close: '$' },
            ],
        });

        monaco.languages.setLanguageConfiguration('bibtex', {
            comments: {
                lineComment: '%',
            },
            brackets: [
                ['{', '}'],
                ['[', ']'],
                ['(', ')'],
            ],
            autoClosingPairs: [
                { open: '{', close: '}', notIn: ['string'] },
                { open: '[', close: ']', notIn: ['string'] },
                { open: '(', close: ')', notIn: ['string'] },
            ],
            surroundingPairs: [
                { open: '{', close: '}' },
                { open: '[', close: ']' },
                { open: '(', close: ')' },
            ],
        });
        await loadWASM('bin/onigasm.wasm');
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
        await wireTmGrammars(monaco, registry, grammars);
    }

    async spellCheckNoDebounce() {
        if (this._editor) {
            const model = this._editor.getModel();
            if (model) {
                if (this._spellCheckEngine && this._spellCheckEngine.isReady()) {
                    const checkEntireDocument = this._toBeSpellCheckedLines.includes(-1);
                    if (checkEntireDocument) {
                        const value = model.getValue();
                        this._lastSpeckResult = await this._spellCheckEngine.checkSpell(value, -1);
                    } else {
                        this._lastSpeckResult = this._lastSpeckResult.filter((e) => {
                            return !this._toBeSpellCheckedLines.includes(e.startLineNumber);
                        });

                        for (const line of this._toBeSpellCheckedLines) {
                            const lineContent = model.getLineContent(line);
                            const result = await this._spellCheckEngine.checkSpell(
                                lineContent,
                                line,
                            );
                            this._lastSpeckResult = this._lastSpeckResult.concat(result);
                        }
                    }
                    this._updateMarkers();
                    this._toBeSpellCheckedLines = [];
                }
            }
        }
    }

    _extractKeyStrokeEvent(model: any, e: monaco.editor.IModelContentChangedEvent) {
        // Mux is needed to remove the interference of ?YJS
        this._editingMutex(() => {
            if (e.changes.length === 1) {
                let character = '';
                // console.log(e.changes[0]);
                if (e.changes[0].rangeLength === 1) {
                    const internal_stack: any = model._commandManager;
                    const last_operation_reverse: any = internal_stack.currentOpenStackElement;
                    if (
                        last_operation_reverse &&
                        last_operation_reverse.editOperations &&
                        last_operation_reverse.editOperations.length >= 1
                    ) {
                        const operationsArray = last_operation_reverse.editOperations;
                        const rev_ops: any = operationsArray[operationsArray.length - 1];
                        if (rev_ops && rev_ops.operations && rev_ops.operations.length === 1) {
                            const rev_op: any = rev_ops.operations[0];
                            if (rev_op && rev_op.text) {
                                character = rev_op.text;
                                this.props.onKeyStroke(character, false);
                            }
                        }
                    }
                } else {
                    character = e.changes[0].text;
                    this.props.onKeyStroke(character, true);
                }
            }
            this.props.onSubmitYJSEditingEvent(e);
        });
    }

    _onModelContentChanged(e: monaco.editor.IModelContentChangedEvent) {
        const model = this._editor!.getModel();

        if (model) {
            // Report value Change
            const value = model.getValue();
            const path = model.uri.path.substr(1);
            if (value !== this.props.value) {
                this.props.onValueChange(value, path);
            }

            // Report Key Strokes for preview and YJS
            this._extractKeyStrokeEvent(model, e);

            // Queue for spell check
            let spellCheckLine = -1;
            if (
                e.changes[0].text !== e.eol &&
                e.changes[0].range.startLineNumber === e.changes[0].range.endLineNumber
            ) {
                spellCheckLine = e.changes[0].range.endLineNumber;
            }
            if (!this._toBeSpellCheckedLines.includes(spellCheckLine)) {
                this._toBeSpellCheckedLines.push(spellCheckLine);
            }
            this._spellCheck();
        }
    }

    _onEditorCursorChanged(e: monaco.editor.ICursorPositionChangedEvent) {
        const model = this._editor!.getModel();

        if (model) {
            const path = model.uri.path.substr(1);
            const column = e.position.column;
            const line = e.position.lineNumber;
            this.props.onCursorChange(path, line, column);
            // console.log('Path ' + path + ' ' + column + ' ' + line);
        }
    }

    componentDidMount() {
        const { path, value, annotations, autoFocus, ...rest } = this.props;

        const editor = monaco.editor.create(this._node.current as HTMLDivElement, rest);

        MonacoEditor._setupHighlighting().then(); /* Global */

        this._spellCheckEngine = new HunspellEngine();

        this._contentSubscription = editor.onDidChangeModelContent((e) =>
            this._onModelContentChanged(e),
        );

        this._cursorSubscription = editor.onDidChangeCursorPosition((e) =>
            this._onEditorCursorChanged(e),
        );

        editor.updateOptions({ wordBasedSuggestions: false });

        this._editor = editor;

        this._completionProviderLaTeX = monaco.languages.registerCompletionItemProvider(
            'latex',
            laTeXCompletionProvider
        );

        this._createOrUpdateModel(path, value, autoFocus);

        this._updateMarkers();
    }

    componentDidUpdate(prevProps: Props) {
        const { path, value, annotations, autoFocus, theme, ...rest } = this.props;

        if (this._editor) {
            this._editor.updateOptions(rest);

            // const model = this._editor.getModel();

            if (path !== prevProps.path) {
                // Save the editor state for the previous file so we can restore it when it's re-opened
                editorStates.set(prevProps.path, this._editor.saveViewState());
                this._createOrUpdateModel(path, value, autoFocus);
            }
            // else if (model && value !== model.getValue()) {
            //     // @ts-ignore
            //     this._editor.executeEdits(null, [
            //         {
            //             range: model.getFullModelRange(),
            //             text: value,
            //         },
            //     ]);
            // }
        }

        if (annotations !== prevProps.annotations) {
            this._updateMarkers();
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
        // this._hoverProvider && this._hoverProvider.dispose();
        this._completionProviderLaTeX && this._completionProviderLaTeX.dispose();
        this._spellCheckEngine && this._spellCheckEngine.closeWorker();
        this._editor && this._editor.dispose();
    }

    _createOrUpdateModel = (path: string, value: string, focus?: boolean) => {
        if (this._editor) {
            let model = findModel(path);

            if (!model) {
                console.log('Create ' + path);
                model = monaco.editor.createModel(
                    value,
                    undefined,
                    monaco.Uri.from({ scheme: 'file', path }),
                );

                model.updateOptions({
                    tabSize: 2,
                    insertSpaces: true,
                });
            } else if (!model.isDisposed()) {
                // If a model exists, we need to update it's value
                // This is needed because the content for the file might have been modified externally
                // Use `pushEditOperations` instead of `setValue` or `applyEdits` to preserve undo stack
                model.pushEditOperations(
                    [],
                    [
                        {
                            range: model.getFullModelRange(),
                            text: value,
                        },
                    ],
                    () => null,
                );
            }

            this._editor.setModel(model);

            // Restore the editor state for the file
            const editorState = editorStates.get(path);

            if (editorState) {
                this._editor.restoreViewState(editorState);
            }

            if (focus) {
                this._editor.focus();
            }

            this._toBeSpellCheckedLines = [-1];
            this._lastSpeckResult = [];
        }
    };

    _updateMarkers = () => {
        const toShowAnnotation = this.props.annotations.concat(this._lastSpeckResult);
        // @ts-ignore
        monaco.editor.setModelMarkers(this._editor.getModel(), null, toShowAnnotation);
    };

    _handleResize = debounce(() => {
        this._editor && this._editor.layout();
    }, 250);

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
        height: '100%',
    },
    editor: {
        height: '100%',
        width: '100%',
    },
});
