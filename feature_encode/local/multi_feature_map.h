/*
 * multi_feature_map.h
 * Copyright (C) 2017 dl <dl@mpi36.news.usw2.contents.cmcm.com>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef MULTI_FEATURE_MAP_H
#define MULTI_FEATURE_MAP_H

#include <unordered_map>
#include <vector>

class MultiFeatureMap {
  public:
    MultiFeatureMap(int parts, uint64_t range);
    ~MultiFeatureMap();

  public:
    void Insert(uint64_t key, uint64_t val);
    bool Find(uint64_t key, uint64_t* val);

  public:
    void ParalLoadMap(const std::string& filename);

  private:
    std::vector<std::unordered_map<uint64_t, uint64_t>> maps_;
    const int kParts;
    const uint64_t kRange;
};

#endif /* !MULTI_FEATURE_MAP_H */
