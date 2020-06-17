import { ItemEntry, BackendStorage, UserInfo } from './backendStorage';

const GOOGLE_CLIENT_ID = '691913119884-481fqunn0n41a86p7rv1g4aqrr3t1dmr.apps.googleusercontent.com';
const GOOGLE_REDIRECT_URL = process.env.NODE_ENV === 'production' ? 'https://www.swiftlatex.com/auth.html': 'http://localhost:3011/auth.html';
const BASE_URL = 'https://www.googleapis.com';


export class GoogleStorage extends BackendStorage {

    refreshErrorDetected: boolean;

    constructor(token: string) {
        super(token);
        this.refreshErrorDetected = false;
        setTimeout(() => this._autoRefreshToken(), 500);
    }


    _autoRefreshToken() {
        const google_url = GoogleStorage.getAuthUrl();
        const iframe = document.createElement('iframe');
        const handler = (event: any) => {
            if (!event.data.authResult) return;
            document.body.removeChild(iframe);
            clearTimeout(watchDog);
            if (event.data.authResult === 'ok') {
                window.removeEventListener('message', handler);
                this.token = window.localStorage.getItem('access_token')!;
                setTimeout(() => this._autoRefreshToken(), 15 * 1000 * 60);
            } else {
                console.error('Oops, unable to fetch a new google token.');
                this.refreshErrorDetected = true;
            }
        };
        const watchDog = setTimeout(() => {
            document.body.removeChild(iframe);
            window.removeEventListener('message', handler);
            console.error('Oops, timeout to fetch a new google token.');
            this.refreshErrorDetected = true;
        }, 30000);
        window.addEventListener('message', handler);
        iframe.src = google_url;
        iframe.width = '0';
        iframe.height = '0';
        document.body.appendChild(iframe);
    }

    async get(scope: string, key: string): Promise<ArrayBuffer> {
        if (this.refreshErrorDetected) {
            throw new Error('Token failure');
        }
        const itemKey = scope + '_' + key;
        const fid = await this._itemKeyToFileInfo(itemKey);
        if (!fid) {
            throw new Error('Get File Failure. File does not exist');
        }
        const headers = {
            'Authorization': 'Bearer ' + this.token,
        };
        const downloadUrl = `https://www.googleapis.com/drive/v2/files/${fid}?alt=media&source=downloadUrl`;
        const response = await fetch(downloadUrl, { method: 'GET', headers: headers });
        if (!response.ok) {
            throw new Error('Unable to get file');
        }
        return response.arrayBuffer();
    }

    async getPublicLink(scope: string, key: string): Promise<string> {
        if (this.refreshErrorDetected) {
            throw new Error('Token failure');
        }

        const itemKey = scope + '_' + key;
        const fid = await this._itemKeyToFileInfo(itemKey);
        if (!fid) {
            throw new Error('Cannot get public link because the file does not exist');
        }
        return `https://drive.google.com/uc?id=${fid}&export=download`
    }


    static getAuthUrl(): string {
        return `https://accounts.google.com/o/oauth2/v2/auth?client_id=${GOOGLE_CLIENT_ID}&response_type=token&state=google&scope=https://www.googleapis.com/auth/drive.file email profile &redirect_uri=${GOOGLE_REDIRECT_URL}`;
    }


    async list(scope: string): Promise<ItemEntry[]> {
        if (this.refreshErrorDetected) {
            throw new Error('Token failure');
        }
        const query = '\'' + 'root' + '\' in parents and title contains \'' + scope + '\'';
        const fields = 'items(webContentLink, id, title, modifiedDate)';
        const url = BASE_URL + '/drive/v2/files?'
            + 'q=' + encodeURIComponent(query)
            + '&fields=' + encodeURIComponent(fields)
            + '&maxResults=1000';
        const headers = {
            'Authorization': 'Bearer ' + this.token,
        };
        const response = await fetch(url, { method: 'GET', headers: headers });
        if (response.status !== 200) {
            throw new Error('Failed to list scope ' + scope);
        }
        const jsonR = await response.json();
        const result = [];
        for (let j = 0; j < jsonR.items.length; j++) {
            const item = jsonR.items[j];
            const entry = {
                name: item.title.slice(scope.length + 1),
                scope: scope,
                modifiedTime: item.modifiedDate,
                _id: item.id,
            };
            result.push(entry);
        }
        return result;

    }

    async put(scope: string, key: string, blobLike: Blob): Promise<string> {
        if (this.refreshErrorDetected) {
            throw new Error('Token failure');
        }
        const itemKey = scope + '_' + key;
        const fid = await this._itemKeyToFileInfo(itemKey);
        return this._uploadFile(itemKey, blobLike, fid);
    }


    async _uploadFile(itemKey: string, blobLike: Blob, fid: string | undefined): Promise<string> {
        const metadata = {
            title: itemKey,
            mimeType: 'application/octet-stream',
            parents: [{
                kind: 'drive#fileLink',
                id: 'root',
            }],
        };
        const headers = {
            'Authorization': 'Bearer ' + this.token,
            'Content-Type': 'application/json; charset=UTF-8',
        };

        let url = BASE_URL + '/upload/drive/v2/files?uploadType=resumable';
        let method = 'POST';
        if (fid) {
            url = BASE_URL + '/upload/drive/v2/files/' + fid + '?uploadType=resumable';
            method = 'PUT';
        }

        const response = await fetch(url, { method: method, body: JSON.stringify(metadata), headers: headers });
        if (response.status !== 200) {
            throw new Error('Unable to create file');
        }
        const locationStr = response.headers.get('location');
        const headers_upload = {
            'Authorization': 'Bearer ' + this.token,
        };
        const uploadResponse = await fetch(locationStr as string, {
            method: 'POST',
            body: blobLike,
            headers: headers_upload,
        });
        if (uploadResponse.status !== 200) {
            throw new Error('Unable to create file');
        }
        const uploadResponseJson = await uploadResponse.json();
        const link = uploadResponseJson.webContentLink;
        if (!fid) {
            await this._setPermission(uploadResponseJson.id);
        }
        return link;
    }

    async _setPermission(fid: string): Promise<void> {
        const url = BASE_URL + `/drive/v2/files/${fid}/permissions`;
        const metadata = {
            'role': 'reader',
            'type': 'anyone',
            'value': 'anyone',
        };
        const headers = {
            'Authorization': 'Bearer ' + this.token,
            'Content-Type': 'application/json; charset=UTF-8',
        };
        const response = await fetch(url, { method: 'POST', body: JSON.stringify(metadata), headers: headers });
        if (response.status !== 200) {
            throw new Error('Unable to set permission');
        }
    }

    async _itemKeyToFileInfo(itemKey: string): Promise<undefined | string> {
        let fid = this._getCache(itemKey);
        if (fid) return fid;
        const query = '\'' + 'root' + '\' in parents and title=\'' + itemKey + '\'';
        const fields = 'items(id)';
        const url = BASE_URL + '/drive/v2/files?'
            + 'q=' + encodeURIComponent(query)
            + '&fields=' + encodeURIComponent(fields)
            + '&maxResults=1000';
        const headers = {
            'Authorization': 'Bearer ' + this.token,
        };
        const response = await fetch(url, { method: 'GET', headers: headers });
        if (response.status !== 200) {
            throw new Error('Failed to get share link ' + itemKey);
        }
        const jsonR = await response.json();
        if (jsonR.items.length === 0) {
            return undefined;
        }
        fid = jsonR.items[0].id;
        this._putCache(itemKey, fid)
        return fid;
    }

    async getUserInfo(): Promise<UserInfo> {
        const url = 'https://www.googleapis.com/oauth2/v2/userinfo';
        const headers = {
            'Authorization': 'Bearer ' + this.token,
        };
        const response = await fetch(url, { method: 'GET', headers: headers });
        if (response.status !== 200) {
            throw new Error('getUserInfo failed with status ' + response.status);
        }
        const responseR = await response.json();
        return {
            username: responseR.name,
            email: responseR.email
        }
    }

    _putCache(itemKey: string, url: string | undefined) {
        url && window.localStorage.setItem(itemKey + 'google', url);
    }

    _getCache(itemKey: string): undefined | string {
        const c = window.localStorage.getItem(itemKey + 'google');
        if (!c) {
            return undefined;
        }
        return c;
    }
}