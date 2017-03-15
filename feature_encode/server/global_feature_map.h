/*
 * global_feature_map.h
 * Copyright (C) 2017 dl <dl@mpi36.news.usw2.contents.cmcm.com>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef GLOBAL_FEATURE_MAP_H
#define GLOBAL_FEATURE_MAP_H

#include <unordered_map>
#include "../MurmurHash2.h"
#include "local/fast_io.h"

class GlobalFeatureMap {
  public:
    static GlobalFeatureMap* Get() {
      static GlobalFeatureMap map;
      return &map;
    }

  public:
    void Reserve(uint64_t size) {
      fmap_.reserve(size);
    }
    void ReLoad(const std::string& featureMap) {
      fmap_.clear();
      LineStream stream;
      stream.Load(featureMap.c_str());
      stream.Init(2048 * 10, 128);
      char* str;
      int len;
      char fname[200];
      int index;
      while (stream.GetLine(&str, &len)) {
        sscanf(str, "%d\t%s\t", &index, fname);
        uint64_t key = MurmurHash64A(fname, strlen(fname), 0);
        GlobalFeatureMap::Get()->Insert(key, index);
      }
    }

    void Insert(uint64_t key, int value) {
      fmap_[key] = value;
    }

    bool Get(uint64_t key, int* value) const {
      if (!fmap_.count(key)) return false;
      *value = fmap_.at(key);
      return true;
    }

  private:
    std::unordered_map<uint64_t, int> fmap_;
};

#endif /* !GLOBAL_FEATURE_MAP_H */
