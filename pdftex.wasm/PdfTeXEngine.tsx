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
	Error
}

const ENGINE_PATH = 'swiftlatexpdftex.js';

export class CompileResult {
	pdf: Uint8Array | undefined = undefined;
	status: number = -254;
	log: string = 'No log';
}

export class PdfTeXEngine {
	private latexWorker: Worker | undefined = undefined;
	public latexWorkerStatus: EngineStatus = EngineStatus.Init;
	constructor() {

	}

	public async loadEngine(): Promise<void> {
		if (this.latexWorker !== undefined) {
			throw new Error('Other instance is running, abort()');
		}
		this.latexWorkerStatus = EngineStatus.Init;
		await new Promise((resolve, reject) => {
			this.latexWorker = new Worker(ENGINE_PATH);
			this.latexWorker.onmessage = (ev: any) => {
				const data: any = ev['data'];
				const cmd: string = data['result'] as string;
				if (cmd === 'ok') {
					this.latexWorkerStatus = EngineStatus.Ready;
					resolve();
				} else {
					this.latexWorkerStatus = EngineStatus.Error;
					reject();
				}
			};
		});
		this.latexWorker!.onmessage = (_: any) => {
		};
		this.latexWorker!.onerror = (_: any) => {
		};
	}

	public isReady(): boolean {
		return this.latexWorkerStatus === EngineStatus.Ready;
	}

	private checkEngineStatus(): void {
		if (!this.isReady()) {
			throw Error('Engine is still spinning or not ready yet!');
		}
	}

	public async compileLaTeX(): Promise<CompileResult> {
		this.checkEngineStatus();
		this.latexWorkerStatus = EngineStatus.Busy;
		const start_compile_time = performance.now();
		const res: CompileResult = await new Promise((resolve, _) => {
			this.latexWorker!.onmessage = (ev: any) => {
				const data: any = ev['data'];
				const cmd: string = data['cmd'] as string;
				if (cmd !== "compile") return;
				const result: string = data['result'] as string;
				const log: string = data['log'] as string;
				const status: number = data['status'] as number;
				this.latexWorkerStatus = EngineStatus.Ready;
				console.log('Engine compilation finish ' + (performance.now() - start_compile_time));
				const nice_report = new CompileResult();
				nice_report.status = status;
				nice_report.log = log;
				if (result === 'ok') {
					const pdf: Uint8Array = new Uint8Array(data['pdf']);
					nice_report.pdf = pdf;
				}
				resolve(nice_report);
			};
			this.latexWorker!.postMessage({ 'cmd': 'compilelatex' });
			console.log('Engine compilation start');
		});
		this.latexWorker!.onmessage = (_: any) => {
		};

		return res;
	}

	/* Internal Use */
	public async compileFormat(): Promise<void> {
		this.checkEngineStatus();
		this.latexWorkerStatus = EngineStatus.Busy;
		await new Promise((resolve, reject) => {
			this.latexWorker!.onmessage = (ev: any) => {
				const data: any = ev['data'];
				const cmd: string =  data['cmd'] as string;
				if (cmd !== "compile") return;
				const result: string = data['result'] as string;
				const log: string =  data['log'] as string;
				// const status: number = data['status'] as number;
				this.latexWorkerStatus = EngineStatus.Ready;
				if (result === 'ok') {
					const formatArray = data['pdf']; /* PDF for result */
					const formatBlob = new Blob([formatArray], { type: 'application/octet-stream' });
					const formatURL = URL.createObjectURL(formatBlob);
					setTimeout(() => { URL.revokeObjectURL(formatURL); }, 30000);
					console.log('Download format file via ' + formatURL);
					resolve();
				} else {
					reject(log);
				}
			};
			this.latexWorker!.postMessage({ 'cmd': 'compileformat' });
		});
		this.latexWorker!.onmessage = (_: any) => {
		};
	}

	public setEngineMainFile(filename: string): void {
		this.checkEngineStatus();
		if (this.latexWorker !== undefined) {
			this.latexWorker.postMessage({ 'cmd': 'setmainfile', 'url': filename });
		}
	}

	public writeMemFSFile(filename: string, srccode: string | Uint8Array): void {
		this.checkEngineStatus();
		if (this.latexWorker !== undefined) {
			this.latexWorker.postMessage({ 'cmd': 'writefile', 'url': filename, 'src': srccode });
		}
	}

	public makeMemFSFolder(folder: string): void {
		this.checkEngineStatus();
		if (this.latexWorker !== undefined) {
			if (folder === '' || folder === '/') {
				return;
			}
			this.latexWorker.postMessage({ 'cmd': 'mkdir', 'url': folder });
		}
	}

	public flushCache(): void {
		this.checkEngineStatus();
		if (this.latexWorker !== undefined) {
			// console.warn('Flushing');
			this.latexWorker.postMessage({ 'cmd': 'flushcache' });
		}

	}

	public setTexliveEndpoint(url: string): void {
		if (this.latexWorker !== undefined) {
			this.latexWorker.postMessage({ 'cmd': 'settexliveurl', 'url': url });
			this.latexWorker = undefined;
		}
	}

	public closeWorker(): void {
		if (this.latexWorker !== undefined) {
			this.latexWorker.postMessage({ 'cmd': 'grace' });
			this.latexWorker = undefined;
		}
	}
}
