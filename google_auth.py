import config
import string_utils
import requests
import logging
import traceback



class GoogleOAuth():

    @staticmethod
    def get_authorization_url(level):
        random_token = string_utils.random_string(16)
        scope = "https://www.googleapis.com/auth/drive.file"
        if level != "limited":
            scope = "https://www.googleapis.com/auth/drive"
        authorization_base_url = 'https://accounts.google.com/o/oauth2/v2/auth?client_id=%s&response_type=code&access_type=offline&state=%s&scope=%s email profile' \
                                 '&redirect_uri=%s&prompt=consent' % (
                                     config.GOOGLE_CLIENT_ID, random_token, scope, config.OAUTH_REDIRECT_URI)
        return authorization_base_url, random_token

    @staticmethod
    def get_user_login_info(accesstoken, jwt):
        decoded_str = "https://www.googleapis.com/oauth2/v3/tokeninfo?id_token=%s" % jwt
        try:
            r = requests.get(decoded_str, timeout=20)
            idata = r.json()
            userinfo = {}
            userinfo['email'] = idata['email']
            userinfo['email_verified'] = idata['email_verified']
            userinfo['name'] = idata['name']
            return userinfo
        except:
            traceback.print_exc()
            logging.warning("Unable to fetch basic login info for google!")
            return None

    @staticmethod
    def get_access_token(code):
        token_url = "https://www.googleapis.com/oauth2/v4/token"
        data = {'code':code, 'grant_type':'authorization_code', 'client_id':config.GOOGLE_CLIENT_ID, 'client_secret': config.GOOGLE_CLIENT_SECRET, 'redirect_uri': config.OAUTH_REDIRECT_URI}
        try:
            r = requests.post(token_url, data, timeout=20)
            idata = r.json()
            print(idata)
            return [idata['access_token'],idata['refresh_token'], idata['id_token']]
        except:
            logging.warning("Unable to fetch access token for google!")
            return None

    @staticmethod
    def refresh_token(refresh_token):
        token_url = "https://www.googleapis.com/oauth2/v4/token"
        data = {'refresh_token': refresh_token, 'grant_type': 'refresh_token', 'client_id': config.GOOGLE_CLIENT_ID,
                'client_secret': config.GOOGLE_CLIENT_SECRET}
        try:
            r = requests.post(token_url, data, timeout=20)
            idata = r.json()
            print(idata)
            return idata['access_token']
        except:
            logging.warning("Unable to refresh access token for google!")
            return None

