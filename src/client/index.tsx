import { GoogleStorage } from './storage/google';
import { DropboxStorage } from './storage/dropbox';
import { genRandomString } from './utils/fileUtilities';
import { GithubStorage } from './storage/github';


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

export function githubLogin() {
    const dropbox_url = GithubStorage.getAuthUrl();
    window.open(dropbox_url);
    wait4Token();
}

export function localLogin() {
    let play_token = localStorage.getItem('playground_token');
    if (!play_token) {
        play_token = genRandomString();
        localStorage.setItem('playground_token', play_token);
    }
    localStorage.setItem('provider', 'minio');
    localStorage.setItem('access_token', play_token);
    window.location.href = '/project.html';
}

export function init() {
    document.getElementById('googleLogin')!.addEventListener('click', googleLogin);
    document.getElementById('dropboxLogin')!.addEventListener('click', dropboxLogin);
    document.getElementById('githubLogin')!.addEventListener('click', githubLogin);
    document.getElementById('localLogin')!.addEventListener('click', localLogin);
    const build_number = process.env.BUILD_NUMBER;
    document.getElementById('buildNumber')!.innerHTML += build_number!;
}

//
init();

