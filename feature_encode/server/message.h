/*
 * message.h
 * Copyright (C) 2017 zhangyule <zyl2336709@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdlib.h>
#include <stdio.h>

#include "base/string_printf.hpp"

namespace coding {
  enum MsgType {
    SHUTDOWN_REPLY = -3,
    RELOAD_REPLY = -2,
    ENCODE_REPLY = -1,
    DEFAULT = 0,
    ENCODE = 1,
    RELOAD = 2,
    SHUTDOWN = 3,
  };

  struct Message {
    MsgType type;
    char data_path[256];
    char map_path[256];
    uint64_t featuresize;
  };

  inline std::string DumpMessage(Message& msg) {
    return base::StringPrintf("type:%d, data_path:%s, map_path:%s, featuresize:%lu",
        msg.type, msg.data_path, msg.map_path, msg.featuresize);
  }
}


#endif /* !MESSAGE_H */
