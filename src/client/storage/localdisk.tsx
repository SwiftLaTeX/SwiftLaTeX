/* Do not use it in production */
import { BackendStorage, ItemEntry, UserInfo } from './backendStorage';

export class LocalStorage extends BackendStorage {
    constructor(token: string) {
        super(token);
    }

    async get(scope: string, key: string): Promise<ArrayBuffer | undefined> {
        const compound = scope + '/' + key;
        const url = 'upload/' + encodeURIComponent(compound);
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
        const compound = scope;
        const url = '/local/list?uri=' + encodeURIComponent(compound);
        const response = await fetch(url, { cache: 'no-cache' });
        if (!response.ok) {
            throw new Error('Cannot list scope');
        }
        const response_json = await response.json();
        const data = response_json.data;
        const result = [];
        for (let i = 0; i < data.length; i++) {
            const entry = data[i];
            const r = {
                itemKey: entry,
                scope,
                _id: entry.Key,
                modifiedTime: new Date().toString(),
            };
            result.push(r);
        }
        return result;
    }

    async put(scope: string, key: string, blobLike: Blob): Promise<string> {
        const compound = scope + '/' + key;
        const uploadUrl = '/local/upload?uri=' + encodeURIComponent(compound);
        const uploadFormData = new FormData();
        uploadFormData.append('file', blobLike);
        const result = await fetch(uploadUrl, { method: 'POST', body: uploadFormData });
        if (!result.ok) {
            throw new Error('Upload failure');
        }
        return window.location.protocol + '//' + window.location.host + '/upload/' + compound;
    }

    async getUserInfo(): Promise<UserInfo> {
        return {
            username: 'local',
            email: 'local@swiftlatex.com',
        };
    }

    async getPublicUrl(scope: string, key: string): Promise<string | undefined> {
        const compound = scope + '/' + key;
        const url = window.location.protocol + '//' + window.location.host + '/upload/' + compound;
        return url;
    }

    static getAuthUrl(): string {
        localStorage.setItem('provider', 'local');
        localStorage.setItem('access_token', 'local');
        return '/project.html';
    }
}
