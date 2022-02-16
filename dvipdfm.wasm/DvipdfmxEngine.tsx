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
export enum EngineStatus {
    Init = 1,
    Ready,
    Busy,
    Error,
}

const XDVPDFMX_ENGINE_PATH = 'swiftlatexdvipdfm.js';

export class CompileResult {
    pdf: Uint8Array | undefined = undefined;
    status: number = -254;
    log: string = 'No log';
}

export class DvipdfmxEngine {
    private latexWorker: Worker | undefined = undefined;
    latexWorkerStatus: EngineStatus = EngineStatus.Init;
    constructor() {}

    async loadEngine(): Promise<void> {
        if (this.latexWorker !== undefined) {
            throw new Error('Other instance is running, abort()');
        }
        this.latexWorkerStatus = EngineStatus.Init;
        await new Promise((resolve, reject) => {
            this.latexWorker = new Worker(XDVPDFMX_ENGINE_PATH);
            this.latexWorker.onmessage = (ev: any) => {
                const data: any = ev.data;
                const cmd: string = data.result as string;
                if (cmd === 'ok') {
                    this.latexWorkerStatus = EngineStatus.Ready;
                    resolve();
                } else {
                    this.latexWorkerStatus = EngineStatus.Error;
                    reject();
                }
            };
        });
        this.latexWorker!.onmessage = (_: any) => {};
        this.latexWorker!.onerror = (_: any) => {};
    }

    isReady(): boolean {
        return this.latexWorkerStatus === EngineStatus.Ready;
    }

    private checkEngineStatus(): void {
        if (!this.isReady()) {
            throw Error('Engine is still spinning or not ready yet!');
        }
    }

    async compilePDF(): Promise<CompileResult> {
        this.checkEngineStatus();
        this.latexWorkerStatus = EngineStatus.Busy;
        const start_compile_time = performance.now();
        const res: CompileResult = await new Promise((resolve, _) => {
            this.latexWorker!.onmessage = (ev: any) => {
                const data: any = ev.data;
                const cmd: string = data.cmd as string;
                if (cmd !== 'compile') return;
                const result: string = data.result as string;
                const log: string = data.log as string;
                const status: number = data.status as number;
                this.latexWorkerStatus = EngineStatus.Ready;
                console.log(
                    'Engine compilation finish ' + (performance.now() - start_compile_time)
                );
                const nice_report = new CompileResult();
                nice_report.status = status;
                nice_report.log = log;
                if (result === 'ok') {
                    const pdf: Uint8Array = new Uint8Array(data.pdf);
                    nice_report.pdf = pdf;
                }
                resolve(nice_report);
            };
            this.latexWorker!.postMessage({ cmd: 'compilepdf' });
            console.log('Engine compilation start');
        });
        this.latexWorker!.onmessage = (_: any) => {};

        return res;
    }

    setEngineMainFile(filename: string): void {
        this.checkEngineStatus();
        if (this.latexWorker !== undefined) {
            this.latexWorker.postMessage({ cmd: 'setmainfile', url: filename });
        }
    }

    writeMemFSFile(filename: string, srccode: string | ArrayBuffer): void {
        this.checkEngineStatus();
        if (srccode === undefined) {
            return;
        }
        if (srccode instanceof ArrayBuffer) {
            srccode = new Uint8Array(srccode);
        }
        if (this.latexWorker !== undefined) {
            this.latexWorker.postMessage({ cmd: 'writefile', url: filename, src: srccode });
        }
    }

    makeMemFSFolder(folder: string): void {
        this.checkEngineStatus();
        if (this.latexWorker !== undefined) {
            if (folder === '' || folder === '/') {
                return;
            }
            this.latexWorker.postMessage({ cmd: 'mkdir', url: folder });
        }
    }

    setTexliveEndpoint(url: string): void {
        if (this.latexWorker !== undefined) {
            this.latexWorker.postMessage({ 'cmd': 'settexliveurl', 'url': url });
            this.latexWorker = undefined;
        }
    }

    closeWorker(): void {
        if (this.latexWorker !== undefined) {
            this.latexWorker.postMessage({ cmd: 'grace' });
            this.latexWorker = undefined;
        }
    }
}
