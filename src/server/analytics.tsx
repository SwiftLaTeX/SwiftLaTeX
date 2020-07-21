import { Context } from 'koa';
import compose from 'koa-compose';
import Router from 'koa-router';
import axios from 'axios';
import { MongoClient } from 'mongodb';

const HIT_URL = '04cd307674c4166727511267b681f84e';

let mongodbClient: MongoClient = undefined as any;
let mongodbInited = false;
const MONGODB_ANALYTICS_URI = process.env.MONGODB_ANALYTICS_URI || 'mongodb://localhost:27017/analytics';

const analytics = async (ctx: Context) => {
    const clientIp =
        ctx.request.headers['x-real-ip'] || ctx.request.headers['x-forwarded-for'] || '127.0.0.1';
    let url = 'https://www.google-analytics.com' + ctx.url.slice(10).replace(HIT_URL, 'collect');
    url = url + (url.indexOf('?') === -1 ? '?' : '&') + 'uip=' + encodeURIComponent(clientIp);
    const headerConfigs = {
        headers: {
            'user-agent': ctx.request.headers['user-agent'],
            cookie: ctx.request.headers.cookie,
            'accept-language': ctx.request.headers['accept-language'],
        },
    };
    try {
        const response = await axios.get(url, headerConfigs);
        ctx.body = response.data;

        if (mongodbInited) {
            await mongodbClient.db().collection('log').insertOne({ 'r': url });
        }

    } catch (e) {
        console.log(e);
        ctx.status = 503;
    }
};

async function initMongoDB() {
    try {
        mongodbClient = await MongoClient.connect(MONGODB_ANALYTICS_URI, { useUnifiedTopology: true });
        mongodbInited = true;
    } catch (e) {
        console.log(e);
    }
}

export default function gaproxy() {
    const router = new Router();
    initMongoDB().then();
    router.get(`/analytics/${HIT_URL}`, analytics);
    router.get(`/analytics/r/${HIT_URL}`, analytics);
    router.get(`/analytics/j/${HIT_URL}`, analytics);
    router.post(`/analytics/${HIT_URL}`, analytics);
    router.post(`/analytics/r/${HIT_URL}`, analytics);
    router.post(`/analytics/j/${HIT_URL}`, analytics);
    return compose([router.routes(), router.allowedMethods()]);
}
