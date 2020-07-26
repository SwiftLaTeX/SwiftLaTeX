import { BackendStorage, ItemEntry, UserInfo } from './backendStorage';
import { genRandomString } from '../utils/fileUtilities';

const DROPBOX_ID = 'gu8xjkr74jsx4rn';
const DROPBOX_REDIRECT_URL = window.location.href.startsWith('https://www.swiftlatex.com/')
    ? 'https://www.swiftlatex.com/auth.html'
    : 'http://localhost:3011/auth.html';

export class DropboxStorage extends BackendStorage {
    constructor(token: string) {
        super(token);
    }

    _cleanUrl(item: string): string {
        if (item.endsWith('?dl=0')) {
            item = item.slice(0, item.length - 4) + 'raw=1';
        }
        item = 'https://dl.dropboxusercontent.com/' + item.slice(24); /* replace to cors friendly endpoint */
        return item;
    }

    async get(scope: string, key: string): Promise<ArrayBuffer | undefined> {
        const itemKey = '/' + scope + '/' + key;
        const url = 'https://content.dropboxapi.com/2/files/download';
        const headers = {
            'Dropbox-API-Arg': JSON.stringify({ path: itemKey }),
            Authorization: 'Bearer ' + this.token,
        };

        const response = await fetch(url, { method: 'POST', headers });
        if (response.status === 409 || response.status === 404) {
            return undefined;
        }

        if (!response.ok) {
            throw new Error('get share link failed status ' + response.status);
        }
        const arrayB = await response.arrayBuffer();
        return arrayB;
    }

    async getPublicUrl(scope: string, key: string): Promise<string | undefined> {
        const itemKey = '/' + scope + '/' + key;
        return this._getShareLink(itemKey);
    }

    async _getShareLink(itemKey: string): Promise<string | undefined> {
        const cacheAns = this._getCache(itemKey);
        if (cacheAns) {
            return cacheAns;
        }
        const url = 'https://api.dropbox.com/2/sharing/list_shared_links';
        const options = {
            path: itemKey,
            direct_only: true,
        };
        const headers = {
            Authorization: 'Bearer ' + this.token,
            'Content-Type': 'application/json; charset=UTF-8',
        };
        const response = await fetch(url, {
            method: 'POST',
            body: JSON.stringify(options),
            headers,
        });
        if (response.status !== 200) {
            throw new Error('get share link failed status ' + response.status);
        }
        const jsonR = await response.json();
        if (jsonR.links.lenght === 0) {
            return undefined;
        }
        const rUrl = this._cleanUrl(jsonR.links[0].url);
        this._putCache(itemKey, rUrl);
        return rUrl;
    }

    static getAuthUrl(): string {
        const secureState = 'dropbox' + genRandomString();
        window.localStorage.setItem('oauthState', secureState);
        return `https://www.dropbox.com/oauth2/authorize?response_type=token&client_id=${DROPBOX_ID}&redirect_uri=${DROPBOX_REDIRECT_URL}&state=${secureState}`;
    }

    async list(scope: string): Promise<ItemEntry[]> {
        const url = 'https://api.dropboxapi.com/2/files/list_folder';
        const options = {
            path: '/' + scope,
        };
        const headers = {
            Authorization: 'Bearer ' + this.token,
            'Content-Type': 'application/json; charset=UTF-8',
        };
        const response = await fetch(url, {
            method: 'POST',
            body: JSON.stringify(options),
            headers,
        });
        if (response.status !== 200 && response.status !== 409) {
            throw new Error('List scope failed with ' + response.status);
        }
        if (response.status === 409) {
            return [];
        }
        const jsonR = await response.json();
        const entries = jsonR.entries;
        const result = [];
        for (let i = 0; i < entries.length; i++) {
            const entry = entries[i];
            const r = {
                itemKey: entry.name,
                scope,
                _id: entry.id,
                modifiedTime: entry.client_modified,
            };
            result.push(r);
        }
        return result;
    }

    async put(scope: string, key: string, blobLike: Blob): Promise<string> {
        const itemKey = '/' + scope + '/' + key;
        return this._put(itemKey, blobLike);
    }

    async _put(itemKey: string, blobLike: Blob): Promise<string> {
        const url = 'https://content.dropboxapi.com/2/files/upload';
        const args = {
            path: itemKey,
            mode: { '.tag': 'overwrite' },
            mute: true,
        };
        const headers = {
            Authorization: 'Bearer ' + this.token,
            'Content-Type': 'application/octet-stream',
            'Dropbox-API-Arg': JSON.stringify(args),
        };
        const response = await fetch(url, { method: 'POST', body: blobLike, headers });
        if (response.status !== 200 && response.status !== 409) {
            throw new Error('Put failed with status ' + response.status);
        }
        const shareLink = await this._createShareLink(itemKey);
        return shareLink;
    }

    async _createShareLink(itemKey: string): Promise<string> {
        const cacheAns = this._getCache(itemKey);
        if (cacheAns) {
            return cacheAns;
        }
        const url = 'https://api.dropboxapi.com/2/sharing/create_shared_link_with_settings';
        const options = {
            path: itemKey,
        };
        const headers = {
            Authorization: 'Bearer ' + this.token,
            'Content-Type': 'application/json; charset=UTF-8',
        };
        const response = await fetch(url, {
            method: 'POST',
            body: JSON.stringify(options),
            headers,
        });
        if (response.status !== 200 && response.status !== 409) {
            throw new Error('create share link failed with status ' + response.status);
        }
        if (response.status === 409) {
            /* Already exists */
            const existUrl = await this._getShareLink(itemKey);
            return existUrl!;
        } else {
            const jsonR = await response.json();
            let rUrl = this._cleanUrl(jsonR.url);
            this._putCache(itemKey, rUrl);
            return rUrl;
        }
    }

    _putCache(itemKey: string, url: string) {
        url && window.localStorage.setItem(itemKey + 'dropbox', url);
    }

    _getCache(itemKey: string) {
        return window.localStorage.getItem(itemKey + 'dropbox');
    }

    async getUserInfo(): Promise<UserInfo> {
        const url = 'https://api.dropboxapi.com/2/users/get_current_account';
        const headers = {
            Authorization: 'Bearer ' + this.token,
        };
        const response = await fetch(url, { method: 'POST', headers });
        if (response.status !== 200) {
            throw new Error('getUserInfo failed with status ' + response.status);
        }
        const responseR = await response.json();
        return {
            username: responseR.name.display_name,
            email: responseR.email,
        };
    }
}
