FROM ubuntu:18.04
RUN   apt-get update && apt-get install -q -y \
    python3 \
    python3-pip \
    git

RUN git clone git@github.com:SwiftLaTeX/SwiftLaTeX.git /app && \
    pip3 install -r /app/requirements.txt && echo "0.1"

WORKDIR /app
CMD ["python3", "wsgi.py"]