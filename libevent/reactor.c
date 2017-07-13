/*
 * reactor.c
 * Copyright (C) 2017 zhangyule <zyl2336709@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#define MAX_EVENTS 1024
#define BUFLEN 128
#define SERV_PORT 8080

struct myevent_s {
  int fd;
  int events;
  void *arg;
  void (*call_back)(int fd, int events, void *arg);
  int status;
  char buf[BUFFLEN];
  int len;
  long last_active;
};

int g_efd;
struct myevent_s g_events[MAX_EVENTS+1];
void eventset(struct myevent_s *ev, int fd,
    void (*call_back)(int,int,void*), void* arg) {
  ev->fd = fd;
  ev->call_back = call_back;
  ev->events = 0;
  ev->arg = arg;
  ev->status = 0;
  ev->last_active = time(NULL);
  return;
}

void recvdata(int fd, int events, void *arg);
void senddata(int fd, int events, void *arg);

void eventadd(int efd, int events, struct myevent_s *ev) {
  struct epoll_event epv = {0, {0}};
  int op;
  epv.data.ptr = ev;
  epv.events = ev->events = events;

  if (ev->status == 1)
    op = EPOLL_CTL_MOD;
  else
  { op = EPOLL_CTL_ADD; ev->status = 1; }
}
