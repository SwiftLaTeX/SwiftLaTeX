FROM ubuntu:19.10
RUN   apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -q -y wget \
    texlive-full \
    python3 \
    python3-pip \
    git
RUN git clone https://github.com/SwiftLaTeX/SwiftLaTeX /app && \
    pip3 install -r /app/texlive/requirements.txt && echo "0.5"
WORKDIR /app/texlive
CMD ["python3", "wsgi.py"]
