/*
 * client.h
 * Copyright (C) 2017 zhangyule <zyl2336709@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <glog/logging.h>
#include <gflags/gflags.h>

#include "server/message.h"
#include "base/string_printf.hpp"

DEFINE_string(command, "",
    "option: ENCODE, RELOAD, SHUTDOWN");

DEFINE_string(map_path, "",
    "feature map path");

DEFINE_int64(feature_size, 1000,
    "feature size");

DEFINE_string(data_path, "",
    "feature data path");

DEFINE_string(socket_path, "",
    "socket path");

#define BACKLOG 5
#define BUF_SIZE 1024

using namespace coding;

int
main(int argc, char *argv[]) {

  google::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);

  struct sockaddr_un addr;
  int sfd, cfd;
  ssize_t numRead;
  char buf[BUF_SIZE];

  sfd = socket(AF_UNIX, SOCK_STREAM, 0);
  CHECK(sfd != -1) << base::StringPrintf("socket init failure");

  memset(&addr, 0, sizeof(struct sockaddr_un));
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, FLAGS_socket_path.c_str(), sizeof(addr.sun_path) - 1);
  CHECK(!(connect(sfd, (struct sockaddr*)&addr, sizeof(struct sockaddr_un)) == -1))
    << "connect";

  Message msg;

  if (FLAGS_command == "ENCODE") {
    msg.type = coding::ENCODE;
  } else if (FLAGS_command == "RELOAD") {
    msg.type = coding::RELOAD;
  } else {
    msg.type = coding::SHUTDOWN;
  }

  CHECK(FLAGS_data_path.size() < 256);
  CHECK(FLAGS_map_path.size() < 256);

  memcpy(msg.data_path, FLAGS_data_path.c_str(), FLAGS_data_path.size() + 1);
  memcpy(msg.map_path, FLAGS_map_path.c_str(), FLAGS_map_path.size() + 1);
  msg.featuresize = FLAGS_feature_size;
  memcpy(buf, &msg, sizeof(msg));

  CHECK(write(sfd, buf, sizeof(msg)) == sizeof(msg)) <<
    "partial/failed write";

  close(cfd);

  return 0;
}


#endif /* !CLIENT_H */
