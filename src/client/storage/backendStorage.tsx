export type ItemEntry = {
    name: string;
    scope: string;
    modifiedTime: string;
    _id: string;
};

export type UserInfo = {
    username: string;
    email: string;
};

export abstract class BackendStorage {
    token: string;

    protected constructor(token: string) {
        this.token = token;

    }

    abstract get(scope: string, itemKey: string): Promise<ArrayBuffer>; /* Text */
    abstract getPublicLink(scope: string, itemKey: string): Promise<string>;
    abstract put(scope: string, itemKey: string, bloblike: Blob): Promise<string>;

    abstract list(scope: string): Promise<ItemEntry[]>; /* itemKey */
    static getAuthUrl(): string {
        return '';
    };
    abstract getUserInfo():Promise<UserInfo>;
}


