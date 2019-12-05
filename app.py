from flask import Flask, session, request, redirect, abort, send_from_directory
import sandbox_auth
import google_auth
import dropbox_auth
import config
import time
import hashlib

app = Flask(__name__)
app.secret_key = b'abcdefghijklmnopqrstuvwxyz/'


def generate_jwt_token(username, access_token, expiry):
    tmp = username + config.JWT_SIGNKEY + access_token + config.JWT_SIGNKEY + str(expiry) + config.JWT_SIGNKEY
    sha256_hash = hashlib.sha256()
    sha256_hash.update(tmp.encode())
    return sha256_hash.hexdigest()


def is_logged_in():
    if 'permission' not in session or 'access_token' not in session or 'refresh_token' not in session or 'oauth_type' not in session:
        print("Non Login")
        return False

    if 'expiry' not in session or time.time() - session['expiry'] > 86400 * 7:
        print("Expiry")
        return False

    return True


def get_auth_class(oauth_type):
    if oauth_type == "sandbox":
        return sandbox_auth.SandboxOAuth
    elif oauth_type == "dropbox":
        return dropbox_auth.DropboxOAuth
    elif oauth_type == "google":
        return google_auth.GoogleOAuth
    else:
        return None


@app.route('/oauth/login_oauth')
def login_oauth():
    oauth_type = request.args.get("type", "")
    permission_level = request.args.get("permission", "limited")
    if permission_level != 'full':
        permission_level = 'limited'
    if is_logged_in():
        match_type = oauth_type == session['oauth_type']
        no_permission_upgrade = session['permission'] == "full" or session['permission'] == permission_level
        if match_type and no_permission_upgrade:
            return redirect("/ide/index.html")
    session.clear()
    authtool = get_auth_class(oauth_type)
    authorization_base_url, random_state = authtool.get_authorization_url(permission_level)
    session['VPF'] = random_state
    session['oauth_type'] = oauth_type
    session['permission'] = permission_level
    return redirect(authorization_base_url)


@app.route('/oauth/login_cb')
def login_callback():
    code = request.args.get("code", "")
    given_vpf = request.args.get("state", "")
    saved_vpf = session.get("VPF", "")
    oauth_type = session.get("oauth_type", "")
    if code == "" or oauth_type == "":
        return abort(500)
    if not given_vpf == saved_vpf:
        print("It may be an attack %s vs %s" % (given_vpf, saved_vpf))
        return abort(500)

    oauth_tool = get_auth_class(oauth_type)
    if oauth_tool is None:
        return abort(500)
    oauth_resp = oauth_tool.get_access_token(code)
    if oauth_resp is None:
        return abort(500)
    access_token = oauth_resp[0]
    refresh_token = oauth_resp[1]
    user_id = oauth_resp[2]
    if oauth_resp is None:
        return abort(500)

    user_info = oauth_tool.get_user_login_info(access_token, user_id)

    if user_info is None:
        abort(500)

    session['username'] = user_info['email']
    # Generate Google FireBase Token
    expiry = int(time.time()) + 1800
    jwt_token = generate_jwt_token(session['username'], access_token, expiry)
    # put it in the session for future use

    session['alternative_name'] = user_info['name']
    session['oauth_type'] = oauth_type
    session['access_token'] = access_token
    session['refresh_token'] = refresh_token
    session['jwt_token'] = jwt_token
    session['expiry'] = expiry
    return redirect("/ide/index.html")


@app.route('/oauth/logout')
def logout():
    session.clear()
    return redirect("/")


@app.route('/oauth/token')
def renew_token():
    if not is_logged_in():
        return "alert('please login in first'); window.location.href = '/';"

    oauth_tool = get_auth_class(session['oauth_type'])
    if oauth_tool is None:
        abort(500)

    exp = session['expiry']
    if time.time() - exp > 0:  # Refresh token
        print("Refreshing Token")
        newtoken = oauth_tool.refresh_token(session['refresh_token'])
        if newtoken is None:
            abort(500)
        session['access_token'] = newtoken
        expiry = int(time.time()) + 1800
        session['expiry'] = expiry
        jwt_token = generate_jwt_token(session['username'], newtoken, expiry)
        session['jwt_token'] = jwt_token

    return "window.permission = '%s'; window.expiry = %s; window.access_token = '%s'; window.oauth_type = '%s'; " \
           "window.username = '%s'; window.alternative_username = '%s'; window.jwt_token = '%s';" \
           % (session['permission'], session['expiry'], session['access_token'], session['oauth_type'],
              session['username'],
              session['alternative_name'], session['jwt_token'])


@app.after_request
def after_request(response):
    if request.url.endswith(".wasm"):
        response.headers['Content-Type'] = 'application/wasm'
    response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate, public, max-age=0"
    response.headers["Expires"] = 0
    response.headers["Pragma"] = "no-cache"
    return response


# Consider using nginx to serve the following files.

@app.route('/')
def index():
    return send_from_directory('portal', "index.html")


@app.route('/<path:path>')
def send_portal(path):
    if path == "":
        path = "index.html"
    return send_from_directory('portal', path)


@app.route('/ide/<path:path>')
def send_editor(path):
    return send_from_directory('ide', path)


if __name__ == '__main__':
    app.run(host="0.0.0.0", port=3000, debug=True)
