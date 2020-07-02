import { BackendStorage, ItemEntry, UserInfo } from './backendStorage';
import { genRandomString } from '../utils/fileUtilities';

const GITHUB_ID = 'bc15ebccd57164f28d0a';
const GITHUB_REDIRECT_URL = process.env.NODE_ENV === 'production' ? 'https://www.swiftlatex.com/auth.html' : 'http://localhost:3011/auth.html';

export class GithubStorage extends BackendStorage {

    constructor(token: string) {
        super(token);
    }


    async get(scope: string, key: string): Promise<ArrayBuffer> {
        return new ArrayBuffer(1);
    }


    static getAuthUrl(): string {
        const secureState = 'github' + genRandomString();
        window.localStorage.setItem('oauthState', secureState);
        return `https://github.com/login/oauth/authorize?client_id=${GITHUB_ID}&redirect_uri=${GITHUB_REDIRECT_URL}&state=${secureState}`;
    }


    async list(scope: string): Promise<ItemEntry[]> {
        return [];
    }

    async put(scope: string, key: string, blobLike: Blob): Promise<string> {
        return '';
    }


    async getUserInfo(): Promise<UserInfo> {
        return undefined;
    }


}