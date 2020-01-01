from flask import Flask, send_file
import time
import os.path
import pykpathsea
from flask_cors import cross_origin


import re
import os
startup_time = time.time()
cache_hit_db = {}
file_miss_db = {}
app = Flask(__name__)

regex = re.compile(r'[^a-zA-Z0-9 _\-\.]')


@app.route('/')
def index():
    htmlString = "<h1>I have happily served the following files</h1>"
    for key in cache_hit_db:
        htmlString += "<div>%s</div>"%(key)
    return htmlString



@app.route('/tex/<filename>')
@cross_origin()
def fetch_file(filename):
    if filename.endswith(".fmt"): #Dont do kpathsea search
        if os.path.exists(filename):
            return send_file(filename)
        else:
            return "Format File not found", 301

    if filename in cache_hit_db:
        urls = cache_hit_db[filename]
        return send_file(urls)

    if filename in file_miss_db:
        return "File not found", 301
    
    res = pykpathsea.find_file(san(filename))

    if res is None or not os.path.isfile(res):
        if len(file_miss_db) > 102400:
            file_miss_db.clear()
        file_miss_db[filename] = 1;
        return "File not found", 301
    else:
        cache_hit_db[filename] = res
        return send_file(res)

def san(name):
    return regex.sub('', name)





