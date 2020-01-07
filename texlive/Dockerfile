FROM ubuntu:19.10
RUN   apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -q -y wget \
    texlive-full \
    python3 \
    python3-pip \
    git
RUN git clone https://github.com/elliott-wen/texlive-server.git /app && \
    pip3 install -r /app/requirements.txt && echo "0.5"
COPY pdflatex.fmt /app/
COPY pdflatexori.fmt /app/
WORKDIR /app
CMD ["python3", "wsgi.py"]
