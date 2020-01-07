from flask import Flask, request, render_template, send_from_directory
app = Flask(__name__)



@app.after_request
def after_request(response):
    if request.url.endswith(".wasm"):
        response.headers['Content-Type'] = 'application/wasm'
    response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate, public, max-age=0"
    response.headers["Expires"] = 0
    response.headers["Pragma"] = "no-cache"
    return response


# Consider using nginx to serve the following files

@app.route('/<path:path>')
def send_editor(path):
    return send_from_directory('web', path)


if __name__ == '__main__':
    app.run(host="0.0.0.0", port=8080, debug=True)
