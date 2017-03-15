/*
 * message_handers.h
 * Copyright (C) 2017 zhangyule <zyl2336709@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef MESSAGE_HANDERS_H
#define MESSAGE_HANDERS_H

#include <stdlib.h>
#include <gflags/gflags.h>
#include <unistd.h>

#include "glog/logging.h"
#include "server/message.h"

namespace coding {

class ShutdownHandler;
class ReloadHandler;
class EncodeHandler;

struct Handler {
  virtual void Handle(Message* msg) const = 0;
  static Handler* CreateHandler(MsgType type);
};

struct ShutdownHandler : public Handler {
  virtual void Handle(Message* msg) const;
};

struct ReloadHandler : public Handler {
  virtual void Handle(Message* msg) const;
};

struct EncodeHandler : public Handler {
  virtual void Handle(Message* msg) const;
};

}

#endif /* !MESSAGE_HANDERS_H */
