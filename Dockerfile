FROM ubuntu:18.04
RUN   apt-get update && apt-get install -q -y \
    python3 \
    python3-pip \
    git

RUN mkdir ~/.ssh && ssh-keyscan github.com >> ~/.ssh/known_hosts
COPY id_rsa /root/.ssh/id_rsa
RUN git clone git@github.com:elliott-wen/newportal.git /app && \
    pip3 install -r /app/requirements.txt && echo "0.1"
RUN rm ~/.ssh/id_rsa
WORKDIR /app
CMD ["python3", "wsgi.py"]