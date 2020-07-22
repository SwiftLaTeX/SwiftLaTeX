import { BackendStorage, ItemEntry, UserInfo } from './backendStorage';
import { getRandomName } from '../utils/randomNames';
import { genRandomString } from '../utils/fileUtilities';

export class MinioStorage extends BackendStorage {
    constructor(token: string) {
        super(token);
    }

    async get(scope: string, key: string): Promise<ArrayBuffer | undefined> {
        const url = await this.getPublicUrl(scope, key);
        const response = await fetch(url, { cache: 'no-cache' });
        if (response.status === 404) {
            return undefined;
        }
        if (!response.ok) {
            throw new Error('Cannot fetch file');
        }
        return response.arrayBuffer();
    }

    async list(scope: string): Promise<ItemEntry[]> {
        const compound = this.token + '/' + scope;
        const url = '/minio/list?uri=' + encodeURIComponent(compound);
        const response = await fetch(url, { cache: 'no-cache' });
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
                itemKey: entry.Key.slice(compound.length + 1),
                scope,
                _id: entry.Key,
                modifiedTime: entry.LastModified,
            };
            result.push(r);
        }
        return result;
    }

    async put(scope: string, key: string, blobLike: Blob): Promise<string> {
        // console.log('Putting ' + blobLike.size);
        const compound = this.token + '/' + scope + '/' + key;
        const url = '/minio/upload?uri=' + encodeURIComponent(compound);
        const presigned_req = await fetch(url, { cache: 'no-cache' });
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
        return uploadUrl + '/' + compound;
    }

    async getUserInfo(): Promise<UserInfo> {
        const random_name = localStorage.getItem('playground_name')!;
        return {
            username: random_name,
            email: 'adventurer@swiftlatex.com',
        };
    }

    async getPublicUrl(scope: string, key: string): Promise<string> {
        const compound = this.token + '/' + scope + '/' + key;
        const url =
            window.location.protocol +
            '//' +
            window.location.host +
            '/minio/get?uri=' +
            encodeURIComponent(compound);
        return url;
    }

    static getAuthUrl(): string {
        let random_name = localStorage.getItem('playground_name');
        if (!random_name) {
            random_name = getRandomName();
            localStorage.setItem('playground_name', random_name);
        }
        let play_token = localStorage.getItem('playground_token');
        if (!play_token) {
            play_token = genRandomString();
            localStorage.setItem('playground_token', play_token);
        }
        localStorage.setItem('provider', 'minio');
        localStorage.setItem('access_token', play_token);
        return '/project.html';
    }
}
