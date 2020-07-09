import { GoogleStorage } from './storage/google';
import { DropboxStorage } from './storage/dropbox';
import { MinioStorage } from './storage/minio';
import { LocalStorage } from './storage/localdisk';
// import { GithubStorage } from './storage/github';

export function wait4Token() {
    window.addEventListener('message', (event) => {
        if (event.data.authResult === 'ok') {
            // console.log(localStorage.getItem('provider'));
            window.location.href = '/project.html';
        } else {
            alert('Login failure');
        }
    });
}

export function googleLogin() {
    const google_url = GoogleStorage.getAuthUrl();
    window.open(google_url);
    wait4Token();
}

export function dropboxLogin() {
    const dropbox_url = DropboxStorage.getAuthUrl();
    window.open(dropbox_url);
    wait4Token();
}

export function minioLogin() {
    const local_url = MinioStorage.getAuthUrl();
    window.location.href = local_url;
}

export function localLogin() {
    const local_url = LocalStorage.getAuthUrl();
    window.location.href = local_url;
}

export function init() {
    document.getElementById('googleLogin')!.addEventListener('click', googleLogin);
    document.getElementById('dropboxLogin')!.addEventListener('click', dropboxLogin);
    document.getElementById('minioLogin')!.addEventListener('click', minioLogin);
    document.getElementById('localLogin')!.addEventListener('click', localLogin);
    const build_number = process.env.BUILD_NUMBER;
    document.getElementById('buildNumber')!.innerHTML += build_number!;
}

//
init();
