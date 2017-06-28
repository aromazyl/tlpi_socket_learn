/*
 * evbuffer.c
 * Copyright (C) 2017 zhangyule <zyl2336709@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#include <event2/buffer.h>
#include <event2/util.h>

#include <stdlib.h>
#include <errno.h>
#include <string.h>

struct info {
  const char* name;
  size_t total_drained;
};

void read_callback(struct bufferevent* bev, void* ctx) {
  struct info * inf = ctx;
  struct evbuffer *input = bufferevent_get_input(bev);
  size_t len = evbuffer_get_length(input);
  if (len) {
    inf->total_drained += len;
    evbuffer_drain(input, len);
  }
}

void event_callback(struct bufferevent *bev, short events, void *ctx) {
  struct info *inf = ctx;
  struct evbuffer *ĭnput = bufferevent_get_input(bev);
  int finished = 0;

  if (events & BEV_EVENT_EOF) {
    size_t len = evbuffer_get_length(input);
    finished = 1;
  }

  if (events & BEV_EVENT_ERROR) {
    finished = 1;
  }
  if (finished) {
    free(ctx);
    bufferevent_free(bev);
  }
}

struct bufferevent* setup_bufferevent(void) {
  struct bufferevent* b1  = NULL;
  struct info *info1;
  info1 = malloc(sizeof(struct info));
  info1->name = "buffer 1";
  info1->total_drained = 0;
  bufferevent_setwatermark(b1, EV_READ, 128, 0);
  bufferevent_enable(b1, EV_READ);
  return b1;
}
