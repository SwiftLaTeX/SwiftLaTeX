import { Context } from 'koa';
import compose from 'koa-compose';
import Router from 'koa-router';
import axios from 'axios';
const HIT_URL = '04cd307674c4166727511267b681f84e';
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
    } catch (e) {
        ctx.type = 'json';
        ctx.body = {
            result: 'failed',
        };
    }
};

export default function gaproxy() {
    const router = new Router();

    router.get(`/analytics/${HIT_URL}`, analytics);
    router.get(`/analytics/r/${HIT_URL}`, analytics);
    router.get(`/analytics/j/${HIT_URL}`, analytics);
    router.post(`/analytics/${HIT_URL}`, analytics);
    router.post(`/analytics/r/${HIT_URL}`, analytics);
    router.post(`/analytics/j/${HIT_URL}`, analytics);
    return compose([router.routes(), router.allowedMethods()]);
}
