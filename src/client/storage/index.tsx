import { DropboxStorage } from './dropbox';
import { GoogleStorage } from './google';
import { MinioStorage } from './minio';
import { BackendStorage } from './backendStorage';
import { LocalStorage } from './localdisk';
export { BackendStorage } from './backendStorage';

export function initializeStorage() {
    const provider = localStorage.getItem('provider');
    const stored_token = window.localStorage.getItem('access_token');
    if (!stored_token || !provider) {
        throw new Error('Not signed in yet');
    }
    let storage: BackendStorage | undefined;
    if (provider === 'google') {
        storage = new GoogleStorage(stored_token);
    } else if (provider === 'dropbox') {
        storage = new DropboxStorage(stored_token);
    } else if (provider === 'minio') {
        storage = new MinioStorage(stored_token);
    } else if (provider === 'local') {
        storage = new LocalStorage(stored_token);
    } else {
        throw new Error('Not supported backend');
    }
    return storage;
}
