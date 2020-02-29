import os

DROPBOX_CLIENT_ID = os.getenv("DROPBOX_CLIENT_ID", "")

DROPBOX_CLIENT_SECRET = os.getenv("DROPBOX_CLIENT_SECRET", "")

GOOGLE_CLIENT_ID = os.getenv("GOOGLE_CLIENT_ID", "")

GOOGLE_CLIENT_SECRET = os.getenv("GOOGLE_CLIENT_SECRET", "")

OAUTH_REDIRECT_URI = os.getenv("OAUTH_REDIRECT_URI", "http://localhost/oauth/login_cb")

S3_BUCKET = os.getenv("S3_BUCKET", "swiftlatex")

S3_KEY = os.getenv("S3_KEY", "")

S3_SECRET = os.getenv("S3_SECRET", "")

S3_ENDPOINT = os.getenv("S3_ENDPOINT", "http://localhost:9000")

ONEDRIVE_CLIENT_ID = os.getenv("ONEDRIVE_CLIENT_ID", "")

ONEDRIVE_CLIENT_SECRET = os.getenv("ONEDRIVE_CLIENT_SECRET", "")

GITHUB_CLIENT_ID = os.getenv("GITHUB_CLIENT_ID", "")

GITHUB_CLIENT_SECRET = os.getenv("GITHUB_CLIENT_SECRET", "")

DB_URL = os.getenv("MONGO_URL", "mongodb://localhost:27017/swiftlatex")

PROJECT_ID_LEN = 16

ALLOW_GUEST = os.getenv("ALLOW_GUEST", "true")
