function wait4Token() {
    NProgress.start();
    window.addEventListener('message', (event) => {
        NProgress.done();
        if (event.data.authResult === 'ok') {
            window.location.href = '/project.html';
        } else {
            alert('Login failure');
        }
    });
}

function googleLogin() {
    const google_url = GoogleStorage.getAuthUrl();
    window.open(google_url);
    wait4Token();
}

function dropboxLogin() {
    const dropbox_url = DropboxStorage.getAuthUrl();
    window.open(dropbox_url);
    wait4Token();
}

function makePlayID(length) {
    let result = '';
    const characters = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789';
    const charactersLength = characters.length;
    for (let i = 0; i < length; i++) {
        result += characters.charAt(Math.floor(Math.random() * charactersLength));
    }
    return result;
}

function localLogin() {
    let play_token = localStorage.getItem('playground_token');
    if (!play_token) {
        play_token = makePlayID(32);
        localStorage.setItem('playground_token', play_token);
    }
    localStorage.setItem('provider', 'minio');
    localStorage.setItem('access_token', play_token);
    window.location.href = '/project.html';
}

fetch('dist/build-stats.js').then(resp => {
    return resp.json()
}).then(buildStat => {
    $('.buildNumber').append(buildStat['buildDate']);
});