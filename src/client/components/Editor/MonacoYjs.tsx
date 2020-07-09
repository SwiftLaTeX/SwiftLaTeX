/********************************************************************************
 * Copyright (C) 2018 TypeFox and others.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License v. 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * This Source Code may also be made available under the following Secondary
 * Licenses when the conditions for such availability set forth in the Eclipse
 * Public License v. 2.0 are satisfied: GNU General Public License, version 2
 * with the GNU Classpath Exception which is available at
 * https://www.gnu.org/software/classpath/license.html.
 *
 * SPDX-License-Identifier: EPL-2.0 OR GPL-2.0 WITH Classpath-exception-2.0
 ********************************************************************************/

import * as monaco from 'monaco-editor/esm/vs/editor/editor.api';
import * as Y from 'yjs';
import { WebsocketProvider } from 'y-websocket';
/* eslint-disable @typescript-eslint/no-explicit-any */
const createMutex = () => {
    let token = true;
    return (f: any, g: any) => {
        if (token) {
            token = false;
            try {
                f();
            } finally {
                token = true;
            }
        } else if (g !== undefined) {
            g();
        }
    };
};

export class MonacoYJSBinding {
    private mux: any;
    private monacoHander: monaco.IDisposable | undefined;
    private initFlag: boolean = false; /* Prevent multiple sync events */
    private provider: WebsocketProvider | undefined;
    constructor(
        protected monacoModel: monaco.editor.IModel,
        onTypeContent: (delta: string, isInserted: boolean) => void
    ) {
        this.mux = createMutex();
        this.monacoHander = undefined;
        this.provider = undefined;
        this.init_yjs(monacoModel, onTypeContent);
    }

    init_yjs(
        monacoModel: monaco.editor.IModel,
        onTypeContent: (delta: string, isInserted: boolean) => void
    ): void {
        console.log('Opening url ' + monacoModel.uri.toString());
        const ydoc = new Y.Doc();
        const unique_id = (window.location.pathname + monacoModel.uri.toString())
            .replace(/\//gm, '_')
            .replace(':', '_')
            .replace(/\./gm, '_');

        const ytext = ydoc.getText('document');

        console.log('Start binding editor events');
        this.monacoHander = monacoModel.onDidChangeContent((e) => {
            this.mux(() => {
                if (e.changes.length === 1) {
                    let character = '';
                    // console.log(e.changes[0]);
                    if (e.changes[0].rangeLength === 1) {
                        const internal_stack: any = (monacoModel as any)._commandManager;
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
                                    onTypeContent(character, false);
                                }
                            }
                        }
                    } else {
                        character = e.changes[0].text;
                        onTypeContent(character, true);
                    }
                }
                if (!this.initFlag) return;
                ydoc.transact(() => {
                    e.changes
                        .sort((change1, change2) => change2.rangeOffset - change1.rangeOffset)
                        .forEach((change) => {
                            ytext.delete(change.rangeOffset, change.rangeLength);
                            ytext.insert(change.rangeOffset, change.text);
                        });
                });
            });
        });

        ytext.observe((event) => {
            this.mux(() => {
                if (!this.initFlag) return;
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
                            pos.column
                        );
                        /* eslint-disable */
                        monacoModel.pushEditOperations(
                            [],
                            [{ range, text: (op as any).insert }],
                            () => null
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
                            endPos.column
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
        });

        monacoModel.onWillDispose(() => {
            console.log('Disposing editor event handlers');
            this.initFlag = false;
            if (this.monacoHander) {
                this.monacoHander.dispose();
            }
            ytext._eH.l.length = 0;
            ydoc.destroy();
            // provider.disconnect();
            if (this.provider) {
                this.provider.destroy();
            }
        });

        const enableShare = false;
        if (enableShare) {
            this.provider = new WebsocketProvider('ws://localhost:8081', unique_id, ydoc);
            this.provider.on('sync', () => {
                this.initFlag = true;
                const remote_source = ytext.toString();
                const local_source = monacoModel.getValue();
                if (remote_source === local_source) {
                    console.log('All synced');
                } else {
                    if (remote_source.length === 0) {
                        ytext.insert(0, local_source);
                        console.log('Using local source');
                    } else {
                        monacoModel.setValue(remote_source);
                        console.log('Trusting remote source');
                    }
                }
            });
            this.provider.connect();
        }
    }
}
