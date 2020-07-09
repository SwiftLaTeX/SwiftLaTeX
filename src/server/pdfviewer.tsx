import { Context } from 'koa';
import compose from 'koa-compose';
import Router from 'koa-router';
import axios from 'axios';
import * as Canvas from 'canvas';
const pdfjsLib = require("pdfjs-dist/es5/build/pdf.js");


function NodeCanvasFactory() {
}

NodeCanvasFactory.prototype = {
    create: function NodeCanvasFactory_create(width: number, height: number) {
        const canvas = Canvas.createCanvas(width, height);
        const context = canvas.getContext('2d');
        return {
            canvas: canvas,
            context: context,
        };
    },

    reset: function NodeCanvasFactory_reset(canvasAndContext: any, width: number, height: number) {
        canvasAndContext.canvas.width = width;
        canvasAndContext.canvas.height = height;
    },

    destroy: function NodeCanvasFactory_destroy(canvasAndContext: any) {

        // Zeroing the width and height cause Firefox to release graphics
        // resources immediately, which can greatly reduce memory consumption.
        canvasAndContext.canvas.width = 0;
        canvasAndContext.canvas.height = 0;
        canvasAndContext.canvas = null;
        canvasAndContext.context = null;
    },
};


const pdfview = async (ctx: Context) => {
    try {
        const dstUrl = ctx.request.query.uri;

        if (!dstUrl) {
            ctx.status = 404;
            return;
        }

        const legal = dstUrl.startsWith('https://s3.swiftlatex.com/')
            || dstUrl.startsWith('https://drive.google.com/')
            || dstUrl.startsWith('https://www.dropbox.com/s/')
            || dstUrl.startsWith('https://play.min.io:9000/');

        if (!legal) {
            ctx.status = 405;
            return;
        }

        const response = await axios.get(dstUrl, {
            responseType: 'arraybuffer',
            timeout: 10000,
        });

        const doc = await pdfjsLib.getDocument({ data: response.data }).promise;

        const page = await doc.getPage(1);

        const viewport = page.getViewport({ scale: 2.5 });
        // @ts-ignore
        const canvasFactory = new NodeCanvasFactory();
        const canvasAndContext = canvasFactory.create(
            viewport.width,
            viewport.height,
        );
        const renderContext = {
            canvasContext: canvasAndContext.context,
            viewport: viewport,
            canvasFactory: canvasFactory,
        };

        await page.render(renderContext).promise;
        ctx.type = 'image/png';
        ctx.set('Expires', '864000');
        ctx.body = canvasAndContext.canvas.toBuffer();
    } catch (e) {
        console.log(e);
        ctx.status = 403;
    }

};

export default function pdfViewer() {
    const router = new Router();
    router.get(`/pdfviewer`, pdfview);
    return compose([router.routes(), router.allowedMethods()]);
}
