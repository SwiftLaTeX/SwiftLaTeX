
import string_utils



class SandboxOAuth():

    @staticmethod
    def get_authorization_url(level):
        random_token = string_utils.random_string(16)
        authorization_base_url = '/oauth/login_cb?code=sandbox&state=' + random_token
        return authorization_base_url, random_token

    @staticmethod
    def get_user_login_info(accesstoken, jwt):
            userinfo = {}
            userinfo['email'] = 'local@swiftlatex.com'
            userinfo['email_verified'] = True
            userinfo['name'] = 'Local'
            return userinfo


    @staticmethod
    def get_access_token(code):
        return "Sandbox"

    @staticmethod
    def refresh_token(refresh_token):
        return "Sandbox"

