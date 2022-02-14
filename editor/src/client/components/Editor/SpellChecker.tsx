/********************************************************************************
 * Copyright (C) 2019 Elliott Wen.
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
import { Annotation } from '../../types';
import { set, get } from 'idb-keyval';
export enum EngineStatus {
    Init = 1,
    Ready,
    Busy,
    Error,
}

const HUNSPELL_ENGINE_PATH = 'bin/myspell.js';

export class HunspellEngine {
    private hunspellWorker: Worker | undefined = undefined;
    hunspellWorkerStatus: EngineStatus = EngineStatus.Init;
    private ignoreWordsMap: Map<string, number> = new Map<string, number>();

    constructor() {
        this.loadIgnoreWords().then();
        this.hunspellWorkerStatus = EngineStatus.Init;
        this.hunspellWorker = new Worker(HUNSPELL_ENGINE_PATH);
        this.hunspellWorkerStatus = EngineStatus.Ready;

    }

    isReady(): boolean {
        return this.hunspellWorkerStatus === EngineStatus.Ready;
    }

    private checkEngineStatus(): void {
        if (!this.isReady()) {
            throw Error('Engine is still spinning or not ready yet!');
        }
    }

    async loadIgnoreWords() {
        const dics = await get('custom_dic') as string;
        if (dics) {
            dics.split('|').forEach((element) => {
                if (element) {
                    this.ignoreWordsMap.set(element, 1);
                }
            });
        }
    }

    async persistIgnoreWords() {
        let persist = '';
        for (let key of this.ignoreWordsMap.keys()) {
            persist += (key + '|');
        }
        await set('custom_dic', persist);
    }

    addIgnoreWord(input: string) {
        if (input) {
            this.ignoreWordsMap.set(input, 1);
            this.persistIgnoreWords().then();
        }
    }

    removeIgnoreWord(input: string) {
        if (input) {
            this.ignoreWordsMap.delete(input);
            this.persistIgnoreWords().then();
        }
    }

    async checkSpell(input: string, providedLine: number): Promise<Annotation[]> {
        this.checkEngineStatus();
        this.hunspellWorkerStatus = EngineStatus.Busy;
        const start_compile_time = performance.now();
        const res: Annotation[] = await new Promise((resolve, _) => {
            this.hunspellWorker!.onmessage = (ev: any) => {
                const data: any = ev.data;
                const result: string = data.result as string;
                this.hunspellWorkerStatus = EngineStatus.Ready;
                console.log('Engine hunspell finish ' + (performance.now() - start_compile_time));
                if (!result) {
                    resolve([]);
                }
                const nice_reports: Annotation[] = [];
                const items = result.split('|');
                for (const item of items) {
                    const terms = item.split(',');
                    if (terms.length >= 3) {
                        // console.log(terms);
                        let lineNo = parseInt(terms[1]) + 1;
                        if (providedLine !== -1) {
                            lineNo = providedLine;
                        }

                        const spellToken = terms[0];
                        const inCustomDic = this.ignoreWordsMap.has(spellToken);
                        const columnStart = parseInt(terms[2]) + 1;
                        const columnEnd = columnStart + terms[0].length;
                        const message = `Potential spelling error in ${spellToken}`;
                        const r: Annotation = {
                            message,
                            startLineNumber: lineNo,
                            endLineNumber: lineNo,
                            startColumn: columnStart,
                            endColumn: columnEnd,
                            severity: inCustomDic ? 1 : 2,
                            source: 'spell',
                        };
                        nice_reports.push(r);
                    }
                }
                resolve(nice_reports);
            };
            this.hunspellWorker!.postMessage({ cmd: 'process', msg: input });
            console.log('Engine hunspell start');
        });
        this.hunspellWorker!.onmessage = (_: any) => {
        };

        return res;
    }

    async suggest(input: string): Promise<string[]> {
        this.checkEngineStatus();

        this.hunspellWorkerStatus = EngineStatus.Busy;
        const start_suggest_time = performance.now();
        const res: string[] = await new Promise((resolve, _) => {
            this.hunspellWorker!.onmessage = (ev: any) => {
                const data: any = ev.data;
                const result: string = data.result as string;
                this.hunspellWorkerStatus = EngineStatus.Ready;
                console.log('Engine hunspell suggest finish ' + (performance.now() - start_suggest_time));
                if (!result) {
                    resolve([]);
                }

                const items = result.split('|');
                resolve(items.filter(e => {
                    return e !== '';
                }));
            };
            this.hunspellWorker!.postMessage({ cmd: 'suggest', msg: input });
        });
        console.log('Engine suggest start');
        this.hunspellWorker!.onmessage = (_: any) => {
        };
        return res;
    }

    closeWorker(): void {
        if (this.hunspellWorker !== undefined) {
            this.hunspellWorker.postMessage({ cmd: 'close' });
            this.hunspellWorker = undefined;
        }
    }
}
