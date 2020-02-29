import datetime
import json
import time

import boto3
import pymongo
from botocore.client import Config
from flask import Flask, session, request, redirect, abort, render_template, flash, Response, send_from_directory
from flask_limiter import Limiter
from flask_paginate import Pagination, get_page_parameter
from flask_pymongo import PyMongo
from flask_session import Session

import config
import dropbox_auth
import github_auth
import google_auth
import onedrive_auth
import utils

app = Flask(__name__)

# Mongo
app.config["MONGO_URI"] = config.DB_URL
mongo = PyMongo(app)

# Aws
aws = boto3.resource('s3',
                     endpoint_url=config.S3_ENDPOINT,
                     aws_access_key_id=config.S3_KEY,
                     aws_secret_access_key=config.S3_SECRET,
                     config=Config(signature_version='s3v4'),
                     region_name='us-east-1')
sts = boto3.client('sts',
                   endpoint_url=config.S3_ENDPOINT,
                   aws_access_key_id=config.S3_KEY,
                   aws_secret_access_key=config.S3_SECRET,
                   config=Config(signature_version='s3v4'),
                   region_name='us-east-1')

# Session
app.config["SESSION_TYPE"] = "filesystem"
sess = Session(app)


# Limit Rate
def limiter_key_func():
    user_record = is_logged_in()
    if user_record:
        return user_record['_id']
    else:
        return 'guest'


limiter = Limiter(
    app,
    key_func=limiter_key_func,
    default_limits=["100 per second"]
)


# Protect CSRF
@app.before_request
def csrf_protect():
    if request.method == "POST":
        token = session.pop('_csrf_token', None)
        if not token or token != request.form.get('csrf_token'):
            abort(403)


def generate_csrf_token():
    if '_csrf_token' not in session:
        session['_csrf_token'] = utils.random_string(32)
    return session['_csrf_token']


app.jinja_env.globals['csrf_token'] = generate_csrf_token


# User Login Control
def is_logged_in():
    if 'oauth_type' not in session or 'username' not in session:
        return None
    db_userid = db_get_user_id(session['username'], session['oauth_type'])
    return mongo.db.users.find_one({"_id": db_userid})


def get_auth_class(oauth_type):
    if oauth_type == "dropbox":
        return dropbox_auth.DropboxOAuth
    elif oauth_type == "google":
        return google_auth.GoogleOAuth
    elif oauth_type == "github":
        return github_auth.GithubOAuth
    elif oauth_type == "onedrive":
        return onedrive_auth.OneDriveOAuth
    else:
        return None


@limiter.exempt
@app.route('/oauth/login_oauth')
def login_oauth():
    oauth_type = request.args.get("type", "")
    if is_logged_in() is not None and session['oauth_type'] == oauth_type:
        return redirect("/project")
    auth_strategy = get_auth_class(oauth_type)
    if auth_strategy is None:
        return abort(500)
    authorization_base_url, random_state = auth_strategy.get_authorization_url()
    session['VPF'] = random_state
    session['oauth_type'] = oauth_type
    return redirect(authorization_base_url)


@limiter.exempt
@app.route('/oauth/login_cb')
def login_callback():
    code = request.args.get("code", "")
    given_vpf = request.args.get("state", "")
    saved_vpf = session.get("VPF", "")
    oauth_type = session.get("oauth_type", "")

    if code == "" or oauth_type == "":
        return abort(500)
    if not given_vpf == saved_vpf:
        return abort(500)

    auth_strategy = get_auth_class(oauth_type)
    if auth_strategy is None:
        return abort(500)
    oauth_resp = auth_strategy.get_access_token(code)
    if oauth_resp is None:
        return abort(500)

    access_token = oauth_resp[0]
    refresh_token = oauth_resp[1]
    user_id = oauth_resp[2]
    user_info = auth_strategy.get_user_login_info(access_token, user_id)

    if user_info is None:
        return abort(500)

    username = user_info['email']
    display_name = user_info['name']
    expiry = int(time.time()) + 3600

    # Write Session
    session['username'] = username
    session['display_name'] = display_name
    session['oauth_type'] = oauth_type
    session['expiry'] = expiry

    # Write Database
    db_create_or_update_user(username, display_name, oauth_type, refresh_token, access_token,
                             expiry)

    return redirect("/project")


@app.route('/sandbox')
def guest_login():
    if config.ALLOW_GUEST != 'true':
        return abort(500)

    user_record = is_logged_in()
    if user_record:
        return redirect("/project")

    session['username'] = utils.random_string(8) + '@swiftlatex.com'
    session['display_name'] = 'guest'
    session['oauth_type'] = 'guest'
    session['expiry'] = time.time() + 3600

    # Write Database
    db_create_or_update_user(session['username'], session['display_name'], session['oauth_type'], 'guest', 'guest',
                             session['expiry'])
    return redirect("/project")


@limiter.exempt
@app.route('/oauth/logout')
def logout():
    session.clear()
    return redirect("/")


# Project Management
@limiter.exempt
@app.route('/')
def index():
    user_record = is_logged_in()
    if user_record:
        return redirect("/project")
    return render_template("index.html")


@limiter.exempt
@app.route('/ping')
def ping():
    return 'ok', 200


@app.route('/key')
def sign_key():
    user_record = is_logged_in()
    if not user_record:
        if config.ALLOW_GUEST != 'true':
            return abort(500)

    referrer = request.headers.get("Referer")
    if not referrer or referrer == '' or len(referrer) < config.PROJECT_ID_LEN + 1:
        return abort(500)

    project_id = referrer[-17:-1]
    project_record = mongo.db.projects.find_one({'_id': project_id})
    if not project_record:
        return abort(500)

    project_record['modified_time'] = datetime.datetime.now()
    mongo.db.projects.save(project_record)

    if user_record:
        if project_id not in user_record['project_list']:
            user_record['project_list'].append(project_id)
            mongo.db.users.save(user_record)

    if project_id + '.exp' in session:  # Try using cache one if it is not expired
        to_expire = int(session[project_id + '.exp'])
        remaining = to_expire - time.time()
        if remaining > 120:
            return session[project_id]
    try:
        policy = '''
            {
              "Version": "2012-10-17",
              "Statement": [
                {
                  "Action": [
                    "s3:GetObject",
                    "s3:PutObject"
                  ],
                  "Effect": "Allow",
                  "Resource": [
                    "arn:aws:s3:::%s/%s/*"
                  ],
                  "Sid": "AllowObjectOperation"
                }
              ]
        }
        ''' % (config.S3_BUCKET, project_id)
        res = sts.assume_role(
            RoleArn="arn:xxx:xxx:xxx:xxxx",
            RoleSessionName="anything",
            Policy=policy
        )
        expected_exp = int(time.time() + 3600)
        json_obj = {
            '_APIKEY': res['Credentials']['AccessKeyId'],
            '_APISECRET': res['Credentials']['SecretAccessKey'],
            '_SESSIONTOKEN': res['Credentials']['SessionToken'],
            '_PREFIX': project_id + '/',
            '_EXPIRY': expected_exp,
            '_ENDPOINT': config.S3_ENDPOINT,
            '_BUCKET': config.S3_BUCKET
        }

        cached_javascript = json.dumps(json_obj)
        # Cache it
        session[project_id] = cached_javascript
        session[project_id + '.exp'] = expected_exp
        return Response(cached_javascript, mimetype='text/javascript')
    except:
        return abort(500)


@app.route('/project')
def project():
    user_record = is_logged_in()
    if not user_record:
        return redirect("/")

    if not user_record:
        return abort(500)

    if user_record['_id'].endswith('guest'):
        flash('You are using the sandbox mode. Please be aware that all files will be removed once you logout. ')

    page = request.args.get(get_page_parameter(), type=int, default=1)
    per_page = 10
    skip_offset = per_page * (page - 1)
    projects = mongo.db.projects.find({'_id': {'$in': user_record['project_list']}}).sort('modified_time',
                                                                                          pymongo.DESCENDING).skip(
        skip_offset).limit(per_page)
    pagination = Pagination(page=page, per_page=10, total=projects.count(), search=False, record_name='Projects',
                            css_framework='bootstrap4')
    return render_template("project.html", projects=projects,
                           pagination=pagination, user_id=user_record['_id'].replace('|', ' (') + ')')


@app.route('/project/delete', methods=['POST'])
def delete_project():
    user_record = is_logged_in()
    if not user_record:
        return redirect("/")
    project_id = request.form.get("project_id", type=str, default="ID?")
    if project_id in user_record['project_list']:
        user_record['project_list'].remove(project_id)
        mongo.db.users.save(user_record)
        flash('Project successfully deleted.')
    return redirect("/project")


@app.route('/project/create', methods=['POST'])
def create_project():
    user_record = is_logged_in()
    if not user_record:
        return redirect("/")
    project_name = request.form.get("project_name", type=str, default="Exciting Project")
    project_name = utils.clean_string(project_name)

    project_url = utils.random_string(config.PROJECT_ID_LEN)
    try:
        aws.Bucket('swiftlatex').put_object(Key=project_url + '/manifest', Body='%s' % datetime.datetime.now())
    except:
        return abort(500)
    project = {}
    project['_id'] = project_url
    project['project_name'] = project_name
    project['author_id'] = user_record['_id']
    project['author_name'] = user_record['display_name']
    project['created_time'] = datetime.datetime.now()
    project['modified_time'] = datetime.datetime.now()
    mongo.db.projects.insert_one(project)
    user_record['project_list'].append(project_url)
    mongo.db.users.save(user_record)

    return redirect('/project/postcreate/%s/' % project_url)


@app.route('/project/postcreate/<pid>/')
def postcreate_project(pid):
    if not pid or pid == '' or len(pid) != config.PROJECT_ID_LEN:
        return abort(500)
    return render_template("postcreate.html")


@app.route('/project/rename', methods=['POST'])
def rename_project():
    user_record = is_logged_in()
    if not user_record:
        return redirect("/")
    project_id = request.form.get("project_id", type=str, default="ID?")
    project_name = request.form.get("project_name", type=str, default="ID?")
    project_name = utils.clean_string(project_name)
    project_record = mongo.db.projects.find_one({'_id': project_id})
    if project_record:
        project_record['project_name'] = project_name
        mongo.db.projects.save(project_record)
        flash('Project successfully renamed.')
    return redirect("/project")


# Database Utils
def db_get_user_id(username, oauth_type):
    return username + '|' + oauth_type


def db_create_or_update_user(username, display_name, oauth_type, refresh_token, access_token, expiry):
    db_userid = db_get_user_id(username, oauth_type)
    record = mongo.db.users.find_one({"_id": db_userid})
    if record:
        record['refresh_token'] = refresh_token
        record['access_token'] = access_token
        record['expiry'] = expiry
        record['display_name'] = display_name
        mongo.db.users.save(record)
    else:
        record = {}
        record['refresh_token'] = refresh_token
        record['access_token'] = access_token
        record['expiry'] = expiry
        record['display_name'] = display_name
        record['enroll_date'] = datetime.datetime.now()
        record['_id'] = db_userid
        record['project_list'] = []
        mongo.db.users.insert_one(record)


# Consider serving them using nginx for better performance
@app.route('/u/<string:pid>/')
def proxy_ide_index(pid):
    return send_from_directory('IDE', 'index.html')


@app.route('/u/<string:pid>/<path:url>')
def proxy_ide(pid, url):
    return send_from_directory('IDE', url)


@app.route('/bin/<string:file>')
def proxy_engine(file):
    return send_from_directory('bin', file)


# For Debug Purpose
@app.after_request
def after_request(response):
    if request.url.endswith(".wasm"):
        response.headers['Content-Type'] = 'application/wasm'
    response.headers["Cache-Control"] = "max-age=86400"
    return response


if __name__ == '__main__':
    app.run(host="localhost", port=5000, debug=True)
