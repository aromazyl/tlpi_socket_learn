/*
 * remove_file_helper.h
 * Copyright (C) 2017 dl <dl@mpi36.news.usw2.contents.cmcm.com>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef REMOVE_FILE_HELPER_H
#define REMOVE_FILE_HELPER_H

#include <thread>
#include <memory>
#include <atomic>
#include <glog/logging.h>

#include "./thread_pool/thread_safe_queue.hpp"

class RemoveFilesHelper {
  public:
    static RemoveFilesHelper* Get() {
      static RemoveFilesHelper helper;
      return &helper;
    }

  private:
    void Main();

  public:
    void Run();

    void Stop();

    void Append(const std::string& filename);

  private:
    std::atomic<bool> is_running_;
    ThreadSafeQueue<std::string> remove_que_;
    std::shared_ptr<std::thread> thread_;
};



#endif /* !REMOVE_FILE_HELPER_H */
