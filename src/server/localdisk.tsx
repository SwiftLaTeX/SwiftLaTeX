/* Do not use in production */
import { Context } from 'koa';
import compose from 'koa-compose';
import Router from 'koa-router';
import * as path from 'path';
import * as fs from 'fs';

const LOCAL_DIR = 'public/upload';
const upload = async (ctx: Context) => {
    if (!ctx.request.query.uri) {
        ctx.status = 404;
        return;
    }

    if (!ctx.request.files) {
        ctx.status = 403;
        return;
    }
    try {
        const dst = path.normalize(LOCAL_DIR + '/' + ctx.request.query.uri);
        const file = ctx.request.files.file;
        const reader = fs.createReadStream(file.path);
        const stream = fs.createWriteStream(dst);
        reader.pipe(stream);
        ctx.status = 200;
    } catch (e) {
        ctx.status = 503;
    }
};

const list = async (ctx: Context) => {
    if (!ctx.request.query.uri) {
        ctx.status = 404;
        return;
    }
    try {
        const dst = path.normalize(LOCAL_DIR + '/' + ctx.request.query.uri);
        const files = await fs.promises.readdir(dst);
        ctx.type = 'json';
        ctx.body = {
            data: files,
        };
    } catch (e) {
        ctx.status = 503;
        return;
    }
};

export default function localBackend() {
    try {
        fs.promises.mkdir('public/upload/manifest', { recursive: true });
    } catch (e) {
    }

    try {
        fs.promises.mkdir('public/upload/asset', { recursive: true });
    } catch (e) {
    }

    const router = new Router();
    router.post('/local/upload', upload);
    router.get('/local/list', list);
    return compose([router.routes(), router.allowedMethods()]);
}
