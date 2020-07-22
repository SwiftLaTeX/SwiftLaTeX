import { Context } from 'koa';
import compose from 'koa-compose';
import Router from 'koa-router';
import axios from 'axios';
import level from 'level-ts';

const db = new level<string>('./sharelink_db');


const fetchShareFile = async (ctx: Context) => {
    const uri = ctx.request.query.uri;
    if (!uri) {
        ctx.status = 404;
        return;
    }

    const legal =
        uri.startsWith('https://s3.swiftlatex.com/') ||
        uri.startsWith('https://www.swiftlatex.com/') ||
        uri.startsWith('https://drive.google.com/') ||
        uri.startsWith('https://www.dropbox.com/s/') ||
        uri.startsWith('http://localhost:3011/');

    if (!legal) {
        ctx.status = 404;
        return;
    }

    try {
        const response = await axios.get(uri, { responseType: 'arraybuffer', timeout: 15000 });
        ctx.body = response.data;
        ctx.type = 'application/octet-stream';
        ctx.status = 200;
    } catch (e) {
        console.log(e);
        ctx.status = 504;
    }
};

const queryProject = async (ctx: Context) => {


    const pid = ctx.request.query.p;
    if (!pid) {
        ctx.status = 404;
        return;
    }

    try {
        const result = await db.exists(pid);
        if (result) {
            const uri = await db.get(pid);
            ctx.body = uri;
        } else {
            ctx.status = 404;
        }
    } catch (e) {
        ctx.status = 504;
    }
};

const shareProject = async (ctx: Context) => {


    const uri = ctx.request.query.uri;
    const pid = ctx.request.query.p;
    if (!uri || !pid) {
        ctx.status = 404;
        return;
    }

    const legal =
        uri.startsWith('https://s3.swiftlatex.com/') ||
        uri.startsWith('https://www.swiftlatex.com/') ||
        uri.startsWith('https://drive.google.com/') ||
        uri.startsWith('https://www.dropbox.com/s/') ||
        uri.startsWith('http://localhost:3011/');

    if (!legal) {
        ctx.status = 404;
        return;
    }

    try {
        const result = await db.exists(pid);
        if (!result) {
            console.log(`Inserting ${pid} ${uri}`);
            await db.put(pid, uri);
        }
        ctx.status = 200;
    } catch (e) {
        console.log(e);
        ctx.status = 504;
    }
};


export default function sharingHub() {
    const router = new Router();
    router.get(`/s/create`, shareProject);
    router.get(`/s/query`, queryProject);
    router.get(`/s/fetch`, fetchShareFile);
    return compose([router.routes(), router.allowedMethods()]);
}
