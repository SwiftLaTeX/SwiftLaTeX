import logging

import dropbox
import requests

import abstract_auth
import config
import utils


class DropboxOAuth(abstract_auth.Auth):

    @staticmethod
    def get_authorization_url():
        random_token = utils.random_string(16)
        authorization_base_url = 'https://www.dropbox.com/oauth2/authorize?client_id=%s&redirect_uri=%s&response_type=code&state=%s' % (
            config.DROPBOX_CLIENT_ID, config.OAUTH_REDIRECT_URI, random_token)
        return authorization_base_url, random_token

    @staticmethod
    def get_user_login_info(access_token, id):
        try:
            dbx = dropbox.Dropbox(access_token)
            tuserinfo = dbx.users_get_current_account()
            userinfo = {}
            userinfo['name'] = tuserinfo.name.display_name
            userinfo['email'] = tuserinfo.email
            userinfo['email_verified'] = tuserinfo.email_verified
            return userinfo
        except:
            logging.warning("Unable to fetch basic information")
            return  None

    @staticmethod
    def get_access_token(code):
        token_url = "https://api.dropbox.com/1/oauth2/token"
        data = {'code':code, 'grant_type':'authorization_code', 'redirect_uri':config.OAUTH_REDIRECT_URI}
        auth = requests.auth.HTTPBasicAuth(config.DROPBOX_CLIENT_ID, config.DROPBOX_CLIENT_SECRET)
        try:
            r = requests.post(token_url, data, auth=auth, timeout=20)
            idata = r.json()
            return [idata['access_token'], idata['access_token'], idata['access_token']]
        except:
            logging.warning("Unable to fetch access token!")
            return None

    @staticmethod
    def refresh_token(token):
        return token

