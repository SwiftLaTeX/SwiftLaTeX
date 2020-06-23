// MIT License:
//
// Copyright (c) 2010-2012, Joe Walnes
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.


/**
 * This behaves like a WebSocket in every way, except if it fails to connect,
 * or it gets disconnected, it will repeatedly poll until it succesfully connects
 * again.
 *
 * It is API compatible, so when you have:
 *   ws = new WebSocket('ws://....');
 * you can replace with:
 *   ws = new ReconnectingWebSocket('ws://....');
 *
 * The event stream will typically look like:
 *  onconnecting
 *  onopen
 *  onmessage
 *  onmessage
 *  onclose // lost connection
 *  onconnecting
 *  onopen  // sometime later...
 *  onmessage
 *  onmessage
 *  etc...
 *
 * It is API compatible with the standard WebSocket API.
 *
 * Latest version: https://github.com/joewalnes/reconnecting-websocket/
 * - Joe Walnes
 *
 * Latest TypeScript version: https://github.com/daviddoran/typescript-reconnecting-websocket/
 * - David Doran
 */
export class MimicWebsocket {
    //Should only be used to read WebSocket readyState
    public readyState:number;

    //Set up the default 'noop' event handlers
    public onopen:(ev:Event) => void = function (_:Event) {};
    public onclose:(ev:CloseEvent) => void = function (_:CloseEvent) {};
    public onmessage:(ev:MessageEvent) => void = function (_:MessageEvent) {};
    public onerror:(ev:Event) => void = function (_:Event) {};

    constructor() {
        this.readyState = WebSocket.CONNECTING;
    }

    langServer: Worker = undefined as any;
    langServerPath: string = 'bin/langServer.js'

    public connect() {
        this.langServer = new Worker(this.langServerPath);

        this.langServer.onmessage = (ev: any) => {
            const data: any = ev.data;
            const cmd: string = data.cmd as string;
            if (cmd === 'close') {
                this.onclose(new CloseEvent('close'));
            } else if (cmd === 'transmit') {
                const wrapped_msg = data.msg;
                // console.log('recv msg ' + wrapped_msg);
                this.onmessage(new MessageEvent('message', {data: wrapped_msg}));
            } else if (cmd === 'error') {
                console.error('Language server error detect! ');
                this.onerror(new CustomEvent('error'));
            } else if (cmd === 'connected') {
                this.onopen(new CustomEvent('open'));
                this.readyState = WebSocket.OPEN;
            }
        };
    }

    public send(data:any) {
        if (this.langServer) {
            // console.log('Send msg ' + data);
            this.langServer!.postMessage({ cmd: 'process', msg: data });
        } else {
            throw 'INVALID_STATE_ERR : Pausing to reconnect websocket';
        }
    }

    /**
     * Returns boolean, whether websocket was FORCEFULLY closed.
     */
    public close():boolean {
        this.readyState = WebSocket.CLOSED;
        if (this.langServer) {
            this.langServer!.postMessage({ cmd: 'close' });
            return true;
        }
        return false;
    }
}

