import config
import requests
import logging
import utils
import abstract_auth
import traceback


class GithubOAuth(abstract_auth.Auth):

    @staticmethod
    def get_authorization_url():
        random_token = utils.random_string(16)
        authorization_base_url = "https://github.com/login/oauth/authorize?client_id=%s&scope=user public_repo&redirect_uri=%s&state=%s" % (
            config.GITHUB_CLIENT_ID, config.OAUTH_REDIRECT_URI, random_token)
        return authorization_base_url, random_token

    @staticmethod
    def get_access_token(code):
        token_url = "https://github.com/login/oauth/access_token"
        headers = {'Accept': 'application/json'}
        data = {'code': code, 'grant_type': 'authorization_code', 'client_id': config.GITHUB_CLIENT_ID,
                'client_secret': config.GITHUB_CLIENT_SECRET, 'redirect_uri': config.OAUTH_REDIRECT_URI}
        try:
            r = requests.post(token_url, headers=headers, data=data, timeout=20)
            idata = r.json()
            return [idata['access_token'], idata['access_token'], idata['access_token']]
        except:
            traceback.print_exc()
            logging.warning("Unable to fetch access token for github!")
            return None

    @staticmethod
    def get_user_login_info(token, userid):
        token_url = "https://api.github.com/user"
        headers = {'Authorization': 'bearer %s' % token}
        userinfo = {}
        try:
            r = requests.get(token_url, headers=headers, timeout=20)
            idata = r.json()
            userinfo['name'] = idata['name']
            userinfo['email'] = idata['email']
        except:
            logging.warning("Unable to fetch name for github!")
            return None

        if not userinfo['email']:
            email_url = "https://api.github.com/user/emails"
            headers = {'Authorization': 'bearer %s' % token}
            try:
                r = requests.get(email_url, headers=headers, timeout=20)
                idata = r.json()
                userinfo['email'] = idata[0]['email']
            except:
                logging.warning("Unable to fetch email for github!")
                return None

        return userinfo

    @staticmethod
    def refresh_token(token):
        return token
