import random
import string
from slugify import slugify

def random_string(N):
    return ''.join(random.choice(string.ascii_uppercase + string.digits + string.ascii_lowercase) for _ in range(N))


def clean_string(before_str):
    return slugify(before_str)