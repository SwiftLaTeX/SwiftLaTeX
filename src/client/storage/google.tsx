import { ItemEntry, BackendStorage, UserInfo } from './backendStorage';
import { genRandomString } from '../utils/fileUtilities';

const GOOGLE_CLIENT_ID = '691913119884-481fqunn0n41a86p7rv1g4aqrr3t1dmr.apps.googleusercontent.com';
const GOOGLE_REDIRECT_URL = window.location.href.startsWith('https://www.swiftlatex.com/')
    ? 'https://www.swiftlatex.com/auth.html'
    : 'http://localhost:3011/auth.html';
const BASE_URL = 'https://www.googleapis.com';
const ROOT_FOLDER_NAME = 'swiftlatex';

export class GoogleStorage extends BackendStorage {
    refreshErrorDetected: boolean;
    __rootID: string = '';

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

    async get(scope: string, key: string): Promise<ArrayBuffer | undefined> {
        if (this.refreshErrorDetected) {
            throw new Error('Token failure');
        }
        const itemKey = scope + '_' + key;
        const fid = await this._itemKeyToFileInfo(itemKey);
        if (!fid) {
            return undefined;
        }
        const headers = {
            Authorization: 'Bearer ' + this.token,
        };
        const downloadUrl = `https://www.googleapis.com/drive/v2/files/${fid}?alt=media&source=downloadUrl`;
        const response = await fetch(downloadUrl, { method: 'GET', headers });
        if (response.status === 404) {
            return undefined;
        }
        if (!response.ok) {
            throw new Error('Unable to get file');
        }
        return response.arrayBuffer();
    }

    async getPublicUrl(scope: string, key: string): Promise<string | undefined> {
        if (this.refreshErrorDetected) {
            throw new Error('Token failure');
        }

        const itemKey = scope + '_' + key;
        const fid = await this._itemKeyToFileInfo(itemKey);
        if (!fid) {
            return undefined;
        }
        return `https://drive.google.com/uc?id=${fid}`;
    }

    static getAuthUrl(): string {
        const secureState = 'google' + genRandomString();
        window.localStorage.setItem('oauthState', secureState);
        return `https://accounts.google.com/o/oauth2/v2/auth?client_id=${GOOGLE_CLIENT_ID}&response_type=token&state=${secureState}&scope=https://www.googleapis.com/auth/drive.file email profile &redirect_uri=${GOOGLE_REDIRECT_URL}`;
    }

    async list(scope: string): Promise<ItemEntry[]> {
        if (this.refreshErrorDetected) {
            throw new Error('Token failure');
        }
        const root_id = await this._resolveRootFolderID();
        const query = "'" + root_id + "' in parents and title contains '" + scope + "'";
        const fields = 'items(webContentLink, id, title, modifiedDate)';
        const url =
            BASE_URL +
            '/drive/v2/files?' +
            'q=' +
            encodeURIComponent(query) +
            '&fields=' +
            encodeURIComponent(fields) +
            '&maxResults=1000';
        const headers = {
            Authorization: 'Bearer ' + this.token,
        };
        const response = await fetch(url, { method: 'GET', headers });
        if (response.status !== 200) {
            throw new Error('Failed to list scope ' + scope);
        }
        const jsonR = await response.json();
        const result = [];
        for (let j = 0; j < jsonR.items.length; j++) {
            const item = jsonR.items[j];
            const entry = {
                itemKey: item.title.slice(scope.length + 1),
                scope,
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
        const root_id = await this._resolveRootFolderID();
        const metadata = {
            title: itemKey,
            mimeType: 'application/octet-stream',
            parents: [
                {
                    kind: 'drive#fileLink',
                    id: root_id,
                },
            ],
        };
        const headers = {
            Authorization: 'Bearer ' + this.token,
            'Content-Type': 'application/json; charset=UTF-8',
        };

        let url = BASE_URL + '/upload/drive/v2/files?uploadType=resumable';
        let method = 'POST';
        if (fid) {
            url = BASE_URL + '/upload/drive/v2/files/' + fid + '?uploadType=resumable';
            method = 'PUT';
        }

        const response = await fetch(url, { method, body: JSON.stringify(metadata), headers });
        if (response.status !== 200) {
            throw new Error('Unable to create file');
        }
        const locationStr = response.headers.get('location');
        const headers_upload = {
            Authorization: 'Bearer ' + this.token,
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
        let link = uploadResponseJson.webContentLink;
        if (link.includes('&export=download')) {
            link = link.replace('&export=download', '');
        }
        if (!fid) {
            await this._setPermission(uploadResponseJson.id);
        }
        return link;
    }

    async _setPermission(fid: string): Promise<void> {
        const url = BASE_URL + `/drive/v2/files/${fid}/permissions`;
        const metadata = {
            role: 'reader',
            type: 'anyone',
            value: 'anyone',
        };
        const headers = {
            Authorization: 'Bearer ' + this.token,
            'Content-Type': 'application/json; charset=UTF-8',
        };
        const response = await fetch(url, {
            method: 'POST',
            body: JSON.stringify(metadata),
            headers,
        });
        if (response.status !== 200) {
            throw new Error('Unable to set permission');
        }
    }

    async _itemKeyToFileInfo(itemKey: string): Promise<undefined | string> {
        let fid = this._getCache(itemKey);
        if (fid) return fid;
        const root_id = await this._resolveRootFolderID();
        const query = "'" + root_id + "' in parents and title contains '" + itemKey + "'";
        const fields = 'items(id)';
        const url =
            BASE_URL +
            '/drive/v2/files?' +
            'q=' +
            encodeURIComponent(query) +
            '&fields=' +
            encodeURIComponent(fields) +
            '&maxResults=1000';
        const headers = {
            Authorization: 'Bearer ' + this.token,
        };
        const response = await fetch(url, { method: 'GET', headers });
        if (response.status !== 200) {
            throw new Error('Failed to get share link ' + itemKey);
        }
        const jsonR = await response.json();
        if (jsonR.items.length === 0) {
            return undefined;
        }
        fid = jsonR.items[0].id;
        this._putCache(itemKey, fid);
        return fid;
    }

    async _resolveRootFolderID(): Promise<string> {
        if (this.__rootID) {
            return this.__rootID;
        }
        const query = "'" + 'root' + "' in parents and title='" + ROOT_FOLDER_NAME + "'";
        const fields = 'items(id)';
        const url =
            BASE_URL +
            '/drive/v2/files?' +
            'q=' +
            encodeURIComponent(query) +
            '&fields=' +
            encodeURIComponent(fields) +
            '&maxResults=1000';
        const headers = {
            Authorization: 'Bearer ' + this.token,
        };
        const response = await fetch(url, { method: 'GET', headers });
        if (response.status !== 200) {
            throw new Error('Failed to resolve Root ID');
        }
        const jsonR = await response.json();
        if (jsonR.items.length === 0) {
            /* Create the root folder */
            const metadata = {
                title: ROOT_FOLDER_NAME,
                mimeType: 'application/vnd.google-apps.folder',
                parents: [
                    {
                        kind: 'drive#fileLink',
                        id: 'root',
                    },
                ],
            };
            const createFolderUrl = BASE_URL + '/drive/v2/files';
            const createFolderHeaders = {
                Authorization: 'Bearer ' + this.token,
                'Content-Type': 'application/json; charset=UTF-8',
            };
            const createFolderResponse = await fetch(createFolderUrl, {
                method: 'POST',
                body: JSON.stringify(metadata),
                headers: createFolderHeaders,
            });
            if (createFolderResponse.status !== 200) {
                throw new Error('Unable to create root folder');
            }
            const createFolderResponseJson = await createFolderResponse.json();
            this.__rootID = createFolderResponseJson.id;
            return this.__rootID;
        }
        this.__rootID = jsonR.items[0].id;
        return this.__rootID;
    }

    async getUserInfo(): Promise<UserInfo> {
        const url = 'https://www.googleapis.com/oauth2/v2/userinfo';
        const headers = {
            Authorization: 'Bearer ' + this.token,
        };
        const response = await fetch(url, { method: 'GET', headers });
        if (response.status !== 200) {
            throw new Error('getUserInfo failed with status ' + response.status);
        }
        const responseR = await response.json();
        return {
            username: responseR.name,
            email: responseR.email,
        };
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
