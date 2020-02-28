import config
import requests
import logging
import utils
import abstract_auth

class OneDriveOAuth(abstract_auth.Auth):

    @staticmethod
    def get_authorization_url():
        random_token = utils.random_string(16)

        authorization_base_url = 'https://login.live.com/oauth20_authorize.srf?client_id=%s&scope=Files.ReadWrite.AppFolder offline_access User.Read&response_type=code&redirect_uri=%s&state=%s' % (
            config.ONEDRIVE_CLIENT_ID, config.OAUTH_REDIRECT_URI, random_token)
        return authorization_base_url, random_token

    @staticmethod
    def get_access_token(code):
        token_url = "https://login.live.com/oauth20_token.srf"
        data = {'code': code, 'grant_type': 'authorization_code', 'client_id': config.ONEDRIVE_CLIENT_ID,
                'client_secret': config.ONEDRIVE_CLIENT_SECRET, 'redirect_uri': config.OAUTH_REDIRECT_URI}
        try:
            r = requests.post(token_url, data, timeout=20)
            idata = r.json()

            return [idata['access_token'], idata['refresh_token'], idata['user_id']]
        except:
            logging.warning("Unable to fetch access token for onedrive!")
            return None

    @staticmethod
    def get_user_login_info(token, userid):
        token_url = "https://graph.microsoft.com/v1.0/users/" + userid
        headers = {'Authorization': 'bearer %s' % token}
        try:
            r = requests.get(token_url, headers=headers, timeout=20)
            idata = r.json()
            userinfo = {}
            userinfo['name'] = idata['displayName']
            userinfo['email'] = idata['userPrincipalName']
            return userinfo
        except:
            logging.warning("Unable to fetch access token for onedrive!")
            return None

    @staticmethod
    def refresh_token(token):
        return token
