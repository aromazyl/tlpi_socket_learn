/*
 * bufferevent.c
 * Copyright (C) 2017 zhangyule <zyl2336709@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#include <event2/event.h>
#include <event2/bufferevent.h>
#include <sys/socket.h>
#include <string.h>

void eventcb(struct bufferevent* bev, short events, void * ptr) {
  if (events & BEV_EVENT_CONNECTED) {
  }
}
