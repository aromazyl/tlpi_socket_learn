/*
 * message_handlers.cc
 * Copyright (C) 2017 dl <dl@mpi36.news.usw2.contents.cmcm.com>
 *
 * Distributed under terms of the MIT license.
 */

#include <atomic>
#include <functional>
#include <glog/logging.h>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <vector>
#include <string>

#include "server/global_feature_map.h"
#include "MurmurHash2.h"
#include "local/remove_file_helper.h"
#include "thread_pool/simple_threadpool.hpp"
#include "server/message_handlers.h"


DECLARE_bool(remove_file);

namespace coding {
Handler* Handler::CreateHandler(MsgType type) {
  switch (type) {
    case ENCODE: return new EncodeHandler;
    case RELOAD: return new ReloadHandler;
    case SHUTDOWN: return new ShutdownHandler;
    default: return new ShutdownHandler;
  }
}

void ShutdownHandler::Handle(Message* msg) const {
  if (FLAGS_remove_file) RemoveFilesHelper::Get()->Stop();
  LOG(INFO) << "system shut down";
  exit(0);
}

void ReloadHandler::Handle(Message* msg) const {
  GlobalFeatureMap::Get()->Reserve(msg->featuresize);
  LOG(INFO) << "reload map" << DumpMessage(*msg);
  GlobalFeatureMap::Get()->ReLoad(msg->map_path);
  LOG(INFO) << "redload success";
}

namespace {
  void generate_f(bool remove_file, const std::string& filename, const std::string& dump, std::atomic<int>* counter) {
    LOG(INFO) << "start encode:" << filename << std::endl;
    FILE* o = fopen(dump.c_str(), "w+");
    CHECK(o) << dump << ", open failure";
    FILE* f = fopen(filename.c_str(), "r");
    CHECK(f) << filename << ", open failure";
    char buf[1024 * 1024];
    std::vector<std::string> slices;
    std::vector<std::string> sub_slices;
    std::string str;
    while(NULL != fgets(buf, sizeof(buf), f)) {
      str.assign(buf);
      boost::trim(str);
      boost::split(slices, str, boost::is_any_of("\t "));
      if(slices.size() <= 3) {continue;}
      boost::split(sub_slices, slices[2], boost::is_any_of(":"));
      fprintf(o, "1\t%s", sub_slices[0].c_str());
      bool dumped = false;
      for(int i = 3; i < slices.size(); ++i) {
        uint64_t key = MurmurHash64A(slices[i].c_str(), slices[i].length(), 0);
        int val;
        if (!dumped) {
          if (GlobalFeatureMap::Get()->Get(key, &val)) {
            fprintf(o, "\t%d:1", val);
            dumped = true;
          }
        } else {
          if (GlobalFeatureMap::Get()->Get(key, &val)) {
            fprintf(o, " %d:1", val);
          }
        }
      }
      fprintf(o, "\n");
    }
    fclose(f);
    fclose(o);

    LOG(INFO) << filename + " encode done" << std::endl;

    if (remove_file) {
      LOG(WARNING) << "start remove " << filename;
      RemoveFilesHelper::Get()->Append(filename);
    }

    (*counter) += 1;
  }
}

void EncodeHandler::Handle(Message* msg) const {
  LOG(INFO) << "encode data " << DumpMessage(*msg);
  ThreadPool pool;
  const std::string data_path = msg->data_path;
  char buf[1024];
  FILE* f = fopen(data_path.c_str(), "r");
  std::atomic<int> counter(0);
  int size = 0;
  bool remove_file = FLAGS_remove_file;
  while (fgets(buf, sizeof(buf), f)) {
    ++size;
    std::string tmp(buf);
    if (*(tmp.rbegin()) == '\n') tmp.resize(tmp.size() - 1);
    pool.Submit(std::bind(&generate_f, remove_file, tmp, tmp + "_id", &counter));
  }
  fclose(f);
  while (counter != size) {
    usleep(1000);
  }
}

}
