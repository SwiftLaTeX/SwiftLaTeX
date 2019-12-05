FROM python:3.8-alpine
RUN apk add --no-cache git gcc libc-dev linux-headers

RUN git clone https://github.com/SwiftLaTeX/SwiftLaTeX.git /app && \
    pip3 install -r /app/requirements.txt && echo "0.1"

WORKDIR /app
CMD ["python3", "wsgi.py"]
