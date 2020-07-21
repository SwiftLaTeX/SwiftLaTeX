import { Context } from 'koa';
import compose from 'koa-compose';
import Router from 'koa-router';
import axios from 'axios';
import { MongoClient } from 'mongodb';

let mongodbClient: MongoClient = undefined as any;
let mongodbInited = false;
const COLLECTION_NAME = 'projects';
const MONGODB_SHARE_URI = process.env.MONGODB_SHARE_URI || 'mongodb://localhost:27017/share';

const fetchShareFile = async (ctx: Context) => {
    const uri = ctx.request.query.uri;
    if (!uri) {
        ctx.status = 404;
        return;
    }

    const legal = uri.startsWith('https://s3.swiftlatex.com/')
        || uri.startsWith('https://www.swiftlatex.com/')
        || uri.startsWith('https://drive.google.com/')
        || uri.startsWith('https://www.dropbox.com/s/')
        || uri.startsWith('http://localhost:3011/');

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
        ctx.status = 504;
        return;
    }
};

const queryProject = async (ctx: Context) => {
    if (!mongodbInited) {
        ctx.status = 504;
        return;
    }

    const pid = ctx.request.query.p;
    if (!pid) {
        ctx.status = 404;
        return;
    }

    try {
        const dbCollection = mongodbClient.db().collection(COLLECTION_NAME);
        const result = await dbCollection.findOne({ pid: pid });
        if (result) {
            ctx.body = result['uri'];
            return;
        } else {
            ctx.status = 404;
        }
    } catch (e) {
        ctx.status = 504;
        return;
    }
};

const shareProject = async (ctx: Context) => {
    if (!mongodbInited) {
        ctx.status = 504;
        return;
    }

    const uri = ctx.request.query.uri;
    const pid = ctx.request.query.p;
    if (!uri || !pid) {
        ctx.status = 404;
        return;
    }

    const legal = uri.startsWith('https://s3.swiftlatex.com/')
        || uri.startsWith('https://www.swiftlatex.com/')
        || uri.startsWith('https://drive.google.com/')
        || uri.startsWith('https://www.dropbox.com/s/')
        || uri.startsWith('http://localhost:3011/');

    if (!legal) {
        ctx.status = 404;
        return;
    }

    try {
        const dbCollection = mongodbClient.db().collection(COLLECTION_NAME);
        const result = await dbCollection.findOne({ pid: pid });
        if (!result) {
            console.log(`Inserting ${pid} ${uri}`);
            await dbCollection.insertOne({ pid: pid, uri: uri });
        }
        ctx.status = 200;
    } catch (e) {
        ctx.status = 504;
        return;
    }

};

async function initMongoDB() {
    try {
        mongodbClient = await MongoClient.connect(MONGODB_SHARE_URI, { useUnifiedTopology: true });
        const dbCollection = mongodbClient.db().collection(COLLECTION_NAME);
        await dbCollection.updateOne({ 'pid': 'initial' }, { '$set': { 'uri': '' } }, { 'upsert': true });
        const indexExists = await dbCollection.indexExists('pid');
        if (!indexExists) {
            await dbCollection.createIndex('pid');
        }
        mongodbInited = true;
    } catch (e) {
        console.log(e);
    }
}

export default function sharingHub() {
    const router = new Router();
    initMongoDB().then();
    router.get(`/s/create`, shareProject);
    router.get(`/s/query`, queryProject);
    router.get(`/s/fetch`, fetchShareFile);
    return compose([router.routes(), router.allowedMethods()]);
}
