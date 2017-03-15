/*
 * multi_feature_map.cc
 * Copyright (C) 2017 dl <dl@mpi36.news.usw2.contents.cmcm.com>
 *
 * Distributed under terms of the MIT license.
 */

#include "multi_feature_map.h"
#include "./fast_io.h"


MultiFeatureMap::MultiFeatureMap(int parts, uint64_t range) : 
 kParts(parts), kRange(range) {
  map.resize(parts);
  for (int i = 0; i < parts; ++i) {
    map[i].reserve(range / parts + 1);
  }
}

~MultiFeatureMap() {}

void MultiFeatureMap::Insert(uint64_t key, uint64_t val) {
  map[key - key / kParts * kParts][key] = val;
}

bool MultiFeatureMap::Find(uint64_t key, uint64_t* val) {
  const int part = key - key / kParts * kParts;
  if (map[part].count(key)) {
    *val = map[part][key];
    return true;
  } else {
    return false;
  }
}

void MultiFeatureMap::ParalLoadMap(const std::string& filename) {
  std::vector<LineStream> streams(kParts, NULL);
  for (auto& p : streams) {
    p.Load(filename.c_str());
    p.Init(2048 * 10, 128);
  }

}
