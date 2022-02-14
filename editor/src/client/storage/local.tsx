/* Do not use it in production */
import { BackendStorage, ItemEntry, UserInfo } from './backendStorage';

export class LocalStorage extends BackendStorage {


    constructor(token: string) {
        super(token);
    }

    async get(scope: string, key: string): Promise<ArrayBuffer | undefined> {
        const internal_key = scope + this.PATH_JOINER_STR + key;
        return await Get(internal_key);
    }

    async list(scope: string): Promise<ItemEntry[]> {
        return [];
    }

    async put(scope: string, key: string, blobLike: Blob): Promise<string> {
        throw new Error('Not implemented yet');
    }

    async getUserInfo(): Promise<UserInfo> {
        return {
            username: 'local',
            email: 'local@swiftlatex.com',
        };
    }

    async getPublicUrl(scope: string, key: string): Promise<string | undefined> {
        return undefined;
    }

    static getAuthUrl(): string {
        localStorage.setItem('provider', 'local');
        localStorage.setItem('access_token', 'local');
        return '/project.html';
    }
}
