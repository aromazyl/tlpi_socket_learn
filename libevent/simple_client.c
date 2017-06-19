/*
 * simple_client.c
 * Copyright (C) 2017 zhangyule <zyl2336709@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#include <netinet/in.h>
#include <sys/socket.h>

#include <netdb.h>

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char** argv) {
  const char query[] =
    "GET / HTTP /1.0\r\n"
    "Host: www.google.com\r\n"
    "\r\n";
  const char hostname[] = "www.google.com";
  struct sockaddr_in sin;
  struct hostent *h;
  const char* cp;
  int fd;
  ssize_t n_written, remaining;
  char buf[1024];
  h = gethostbyname(hostname);
  if (!h) {
    fprintf(stderr, "Couldn't lookup %s:%s\n", hostname, hstrerror(h_errno));
    return 1;
  }
  if (h->h_addrtype != AF_INET) {
     fprintf(stderr, "no ipv support sorry.\n");
     exit(1);
  }

  fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0) {
    perror("socket");
    exit(1);
  }

  sin.sin_family = AF_INET;
  sin.sin_port = htons(80);
  sin.sin_addr = *(struct in_addr*)(h->h_addr);
  if (connect(fd, (struct sockaddr*)&sin, sizeof(sin))) {
    perror("connect");
    fclose(fd);
    return 1;
  }
  cp = query;
  remaining = strlen(query);
  while (remaining) {
    n_written = send(fd, cp, remaining, 0);
    if (n_written <= 0) {
      perror("send");
      return 1;
    }
    remaining -= n_written;
    cp += n_written;
  }

  while (1) {
    ssize_t result = recv(fd, buf, sizeof(buf), 0);
    if (result == 0) {
      break;
    } else if (result < 0) {
      perror("recv");
      close(fd);
      return 1;
    }
    fwrite(buf, 1, result, stdout);
  }

  fclose(fd);

  return 0;
}
