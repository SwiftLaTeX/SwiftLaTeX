import { Context } from 'koa';
import compose from 'koa-compose';
import Router from 'koa-router';
import axios from 'axios';
const GITHUB_URL = 'https://github.com/login/oauth/access_token';
const githubServerOauth = async (ctx: Context) => {
    try{
        const response = await axios.post(GITHUB_URL, {'client_id': '', 'client_secret': '', code: ''});
        console.log(response.data);
        ctx.body = response.data;
    } catch {

    }

};

export default function githubOauth() {
    const router = new Router();
    router.get('/github', githubServerOauth);
    return compose([router.routes(), router.allowedMethods()]);
}
