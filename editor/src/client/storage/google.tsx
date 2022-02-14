import { ItemEntry, BackendStorage, UserInfo } from './backendStorage';
import { genRandomString } from '../utils/fileUtilities';
import { set, get } from 'idb-keyval';
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

    }

    async get(scope: string, key: string): Promise<ArrayBuffer | undefined> {
        if (this.refreshErrorDetected) {
            throw new Error('Token failure');
        }
        const itemKey = scope + '_' + key;
        const fid = await this._itemKeyToFid(itemKey);
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
        const fid = await this._itemKeyToFid(itemKey);
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
        const fid = await this._itemKeyToFid(itemKey);
        return this._uploadFile(itemKey, blobLike, fid);
    }

    async _uploadFile(itemKey: string, blobLike: Blob, fid: string | undefined): Promise<string> {
        const root_id = await this._resolveRootFolderID();
        const metadata = {
            name: itemKey,
            mimeType: 'application/octet-stream',
            parents: [root_id],
        };
        const headers = {
            Authorization: 'Bearer ' + this.token,
        };

        let url = BASE_URL + '/upload/drive/v3/files?uploadType=multipart';
        let method = 'POST';
        if (fid) {
            url = BASE_URL + '/upload/drive/v3/files/' + fid + '?uploadType=multipart';
            method = 'PUT';
        }
        const uploadForm = new FormData();
        uploadForm.append('metadata', new Blob([JSON.stringify(metadata)], {type:'application/json'}));
        uploadForm.append('file', blobLike);
        const uploadResponse = await fetch(url, { method, body: uploadForm, headers });
        if (!uploadResponse.ok) {
            throw new Error('Unable to upload file');
        }

        const uploadResponseJson = await uploadResponse.json();
        if (!fid) {
            fid = uploadResponseJson.id;

        }
        let link = `https://drive.google.com/uc?id=${fid}`
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
        if (!response.ok) {
            throw new Error('Unable to set permission');
        }
    }

    async _itemKeyToFid(itemKey: string): Promise<undefined | string> {
        let fid = await this._getCache(itemKey);
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
        await this._putCache(itemKey, fid);
        return fid;
    }

    async _createRootFolder(): Promise<string> {
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
        await this._setPermission(createFolderResponseJson.id!);
        return createFolderResponseJson.id;
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
        if (jsonR.items.length > 0) {
            /* Create the root folder */
            this.__rootID = jsonR.items[0].id;
        } else {
            this.__rootID = await this._createRootFolder();
        }
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

    async _putCache(itemKey: string, url: string | undefined) {
        if (url) {
            await set(itemKey + 'google', url);
        }
    }

    async _getCache(itemKey: string): Promise<undefined | string> {
        const c = await get(itemKey + 'google');
        if (!c) {
            return undefined;
        }
        return c as string;
    }
}
