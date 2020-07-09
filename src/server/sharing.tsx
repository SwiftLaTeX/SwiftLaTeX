import { Context } from 'koa';
import compose from 'koa-compose';
import Router from 'koa-router';
import axios from 'axios';

const fetchShareFile = async (ctx: Context) => {
    const uri = ctx.request.query.uri;
    if (!uri) {
        ctx.status = 404;
        return;
    }
    try {
        const response = await axios.get(uri);
        ctx.body = response.data;
    } catch (e) {
        ctx.status = 504;
        return;
    }
};

export default function sharingHub() {
    const router = new Router();
    router.get(`/fetchShare`, fetchShareFile);
    return compose([router.routes(), router.allowedMethods()]);
}
