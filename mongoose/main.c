// Copyright (c) 2015 Cesanta Software Limited
// All rights reserved

#include "mongoose.h"
#include "tinydir.h"

static const char *s_http_port = "3011";
static struct mg_serve_http_opts s_http_server_opts;

static void handle_list_call(struct mg_connection *nc, int ev, void *p) {
#define MAX_REPLY_SIZE 1024 * 1024

  struct http_message *hm = (struct http_message *)p;

  tinydir_dir dir;

  int re = tinydir_open(&dir, "upload/manifest");

  if (re == 0) {
    /* Send headers */
    char *ansBuf = malloc(MAX_REPLY_SIZE);

    int currentPos = 0;

    mg_printf(nc, "%s",
              "HTTP/1.1 200 OK\r\nContent-Type: "
              "application/json\r\nTransfer-Encoding: chunked\r\n\r\n");

    currentPos += snprintf(ansBuf + currentPos, MAX_REPLY_SIZE - currentPos,
                           "{\"data\":[ ");

    while (dir.has_next) {
      tinydir_file file;

      tinydir_readfile(&dir, &file);

      if (!file.is_dir) {
        currentPos += snprintf(ansBuf + currentPos, MAX_REPLY_SIZE - currentPos,
                               "\"%s\",", file.name);
      }

      tinydir_next(&dir);
    }

    currentPos -= 1;

    currentPos +=
        snprintf(ansBuf + currentPos, MAX_REPLY_SIZE - currentPos, "]}");

    // printf("%s\n", ansBuf);

    mg_send_http_chunk(nc, ansBuf, currentPos);

    mg_send_http_chunk(nc, "", 0); /* Send empty chunk, the end of response */

    tinydir_close(&dir);

    // printf("done\n");

    free(ansBuf);

  } else {

    mg_printf(nc, "%s",
              "HTTP/1.1 400 Bad Request\r\nTransfer-Encoding: chunked\r\n\r\n");

    mg_send_http_chunk(nc, "", 0); /* Send empty chunk, the end of response */
  }
}

struct file_writer_data {
  void *fp;
  int bytes_written;
};

static void handle_upload_call(struct mg_connection *nc, int ev, void *p) {
  struct file_writer_data *data = (struct file_writer_data *)nc->user_data;
  struct mg_http_multipart_part *mp = (struct mg_http_multipart_part *)p;
  switch (ev) {
  case MG_EV_HTTP_PART_BEGIN: {
    if (data == NULL) {
      data = calloc(1, sizeof(struct file_writer_data));
      char filepath[1024] = {0};
      snprintf(filepath, 1023, "upload/%s", mp->file_name);
      data->fp = fopen(filepath, "w");
      data->bytes_written = 0;
      if (data->fp == NULL) {
        mg_printf(nc, "%s",
                  "HTTP/1.1 500 Failed to open a file\r\n"
                  "Content-Length: 0\r\n\r\n");
        nc->flags |= MG_F_SEND_AND_CLOSE;
        return;
      }
      nc->user_data = (void *)data;
    }
    break;
  }
  case MG_EV_HTTP_PART_DATA: {
    if (fwrite(mp->data.p, 1, mp->data.len, data->fp) != mp->data.len) {
      mg_printf(nc, "%s",
                "HTTP/1.1 500 Failed to write to a file\r\n"
                "Content-Length: 0\r\n\r\n");
      nc->flags |= MG_F_SEND_AND_CLOSE;
      return;
    }
    data->bytes_written += mp->data.len;
    break;
  }
  case MG_EV_HTTP_PART_END: {
    mg_printf(nc,
              "HTTP/1.1 200 OK\r\n"
              "Content-Type: text/plain\r\n"
              "Connection: close\r\n\r\n"
              "Written %ld of POST data to a temp file\n\n",
              (long)ftell(data->fp));
    nc->flags |= MG_F_SEND_AND_CLOSE;
    fclose(data->fp);
    free(data);
    nc->user_data = NULL;
    break;
  }
  }
}

static void ev_handler(struct mg_connection *nc, int ev, void *p) {
  struct http_message *hm = (struct http_message *)p;

  if (ev == MG_EV_HTTP_REQUEST) {
    mg_serve_http(nc, hm, s_http_server_opts);
  }
}

int main(void) {
  struct mg_mgr mgr;
  struct mg_connection *nc;

  mg_mgr_init(&mgr, NULL);
  printf("Starting web server on port %s\n", s_http_port);
  nc = mg_bind(&mgr, s_http_port, ev_handler);
  if (nc == NULL) {
    printf("Failed to create listener\n");
    return 1;
  }
  mg_register_http_endpoint(nc, "/local/list", handle_list_call);

  mg_register_http_endpoint(nc, "/local/upload", handle_upload_call);

  // Set up HTTP server parameters
  mg_set_protocol_http_websocket(nc);
  s_http_server_opts.document_root = "."; // Serve current directory
  s_http_server_opts.enable_directory_listing = "no";

  for (;;) {
    mg_mgr_poll(&mgr, 1000);
  }
  mg_mgr_free(&mgr);

  return 0;
}