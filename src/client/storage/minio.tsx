import { BackendStorage, ItemEntry, UserInfo } from './backendStorage';
import { getRandomName } from '../utils/randomNames';

const BASE_URL = 'https://s3.swiftlatex.com/swiftlatex2';



export class MinioStorage extends BackendStorage {

    constructor(token: string) {
        super(token);
    }

    async get(scope: string, key: string): Promise<ArrayBuffer> {
        const url = BASE_URL + '/' + this.token + '/' + scope + '/' + key;
        const response = await fetch(url, {cache: "no-cache"});
        if (!response.ok) {
            throw new Error('Cannot fetch file');
        }
        return response.arrayBuffer();
    }

    // async getPublicLink(scope: string, key: string): Promise<string> {
    //     return BASE_URL + '/' + this.token + '/' + scope + '/' + key;
    // }

    async list(scope: string): Promise<ItemEntry[]> {
        const url = this.token + '/' + scope;
        const response = await fetch(`/list?uri=${url}`, { cache: "no-cache"});
        if (!response.ok) {
            throw new Error('Cannot list scope');
        }
        const response_json = await response.json();
        if (response_json.result !== 'ok') {
            throw new Error('Cannot list scope due to server error');
        }
        const data = response_json.data.Contents;
        const result = [];
        for (let i = 0; i < data.length; i++) {
            const entry = data[i];
            const r = {
                itemKey: entry.Key.slice(url.length + 1),
                scope: scope,
                _id: entry.Key,
                modifiedTime: entry.LastModified,
            };
            result.push(r);
        }

        return result;
    }

    async put(scope: string, key: string, blobLike: Blob): Promise<string> {
        const url = this.token + '/' + scope + '/' + key;
        const presigned_req = await fetch(`/upload?uri=${url}`, { cache: "no-cache" });
        if (!presigned_req.ok) {
            throw new Error('Cannot get presigned url');
        }
        const presigned_json = await presigned_req.json();
        if (presigned_json.result !== 'ok') {
            throw new Error('Cannot get pre-signed url');
        }
        const uploadFormData = new FormData();
        const credential = presigned_json.data.fields;
        const uploadUrl = presigned_json.data.url;
        for (const key in credential) {
            if (credential.hasOwnProperty(key)) {
                uploadFormData.append(key, credential[key]);
            }
        }
        uploadFormData.append('file', blobLike);
        const result = await fetch(uploadUrl, { method: 'POST', body: uploadFormData });
        if (!result.ok) {
            throw new Error('Upload failure');
        }
        return BASE_URL + '/' + url;
    }

    async getUserInfo(): Promise<UserInfo> {
        let random_name = localStorage.getItem('playground_name');
        if (!random_name) {
            random_name = getRandomName();
            localStorage.setItem('playground_name', random_name);
        }
        return {
            username: random_name,
            email: 'adventurer@swiftlatex.com',
        };
    }

}