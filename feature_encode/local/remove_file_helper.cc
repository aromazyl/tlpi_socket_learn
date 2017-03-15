/*
 * remove_file_helper.cc
 * Copyright (C) 2017 dl <dl@mpi36.news.usw2.contents.cmcm.com>
 *
 * Distributed under terms of the MIT license.
 */

#include "remove_file_helper.h"


void RemoveFilesHelper::Main() {
  std::string filename;
  is_running_ = true;
  while (is_running_) {
    while (this->remove_que_.TryPop(filename)) {
      int ret = remove(filename.c_str());
      if (ret == 0) {
        LOG(WARNING) << filename << " removed.";
      } else {
        LOG(WARNING) << filename << " removed failure.";
      }
    }
  }
}

void RemoveFilesHelper::Run() {
  this->is_running_ = true;
  thread_.reset(new std::thread(&RemoveFilesHelper::Main, this));
}

void RemoveFilesHelper::Stop() {
  is_running_ = false;
  thread_->join();
}

void RemoveFilesHelper::Append(const std::string& filename) {
  this->remove_que_.Push(filename);
}
