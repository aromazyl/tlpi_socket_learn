/*
 * server.h
 * Copyright (C) 2017 zhangyule <zyl2336709@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <gflags/gflags.h>
#include <glog/logging.h>
#include <string>

#include "server/message.h"
#include "base/string_printf.hpp"
#include "server/message_handlers.h"
#include "local/remove_file_helper.h"

#define BACKLOG 5
#define BUF_SIZE 1024

DEFINE_string(socket_path, "", "Accessable path for socket domain.");

DEFINE_bool(remove_file, false, "Delete converted file?");

using namespace coding;

int
main(int argc, char *argv[]) {

  google::ParseCommandLineFlags(&argc, &argv, true);

  // const std::string FLAGS_socket_path = argv[1];

  struct sockaddr_un addr;
  int sfd, cfd;
  ssize_t numRead;
  char buf[BUF_SIZE];

  LOG(INFO) << "socket init";
  sfd = socket(AF_UNIX, SOCK_STREAM, 0);
  CHECK(sfd != -1) << base::StringPrintf("socket init failure");

  LOG(INFO) << "remove path:" << FLAGS_socket_path;
  CHECK(!(remove(FLAGS_socket_path.c_str()) == -1 && errno != ENOENT))
    << base::StringPrintf("remove-%s", FLAGS_socket_path.c_str());

  if (FLAGS_remove_file) RemoveFilesHelper::Get()->Run();
  memset(&addr, 0, sizeof(struct sockaddr_un));
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, FLAGS_socket_path.c_str(), sizeof(addr.sun_path) - 1);

  LOG(INFO) << "bind to target path";
  CHECK(!(bind(sfd, (struct sockaddr*)&addr, sizeof(struct sockaddr_un)) == -1))
    << "bind";
  CHECK(listen(sfd, BACKLOG) != -1) << "listen";

  Handler* handlers[100] = {0};

  Message msg;
  for (;;) {
    LOG(INFO) << "accept";
    cfd = accept(sfd, NULL, NULL);
    CHECK(cfd != -1) << "accept";
    LOG(INFO) << "start read";
    while (numRead = read(cfd, buf, BUF_SIZE) > 0) {
      memcpy(&msg, buf, sizeof(msg));
      if (msg.type < 0) {
        LOG(INFO) << "reply:" << DumpMessage(msg);
        continue;
      }
      if (!handlers[msg.type + 50]) handlers[msg.type + 50] =
        Handler::CreateHandler(msg.type);
      handlers[msg.type + 50]->Handle(&msg);
    }
  }

  close(cfd);

  return 0;
}


#endif /* !SERVER_H */
