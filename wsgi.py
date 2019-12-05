from gevent.pywsgi import WSGIServer
import app



http_server = WSGIServer(('0.0.0.0', 3000), app.app)
http_server.serve_forever()
