#include <MurmurHash2.h>
#include <algorithm>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <iostream>
#include <boost/filesystem.hpp>
#include <unordered_map>
#include <gperftools/profiler.h>
#include <string.h>
#include <condition_variable>
#include <unistd.h>
#include <utility>
#include <gflags/gflags.h>
#include <gperftools/profiler.h>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <stdio.h>

#include "fast_io.h"
#include "./thread_pool/simple_threadpool.hpp"
#include "./remove_file_helper.h"


DEFINE_string(map_path, "map.txt", "Feature map file path");

DEFINE_string(data_path, "feature.txt",
    "Feature data file txt, pathes writed in a single file");

DEFINE_int64(feature_size, 1000,
    "Estimate the feature size, may"
    " improve the feature map load performance");

DEFINE_bool(remove_file, false,
    "I think you may want to remove the converted file"
    ", just remain the ided file, than open thes option.");

const int TOKEN_N = 3;

std::unordered_map<uint64_t, int> string2id;

char sep = 1;

struct MapStream {
  MapStream(const char* filename) {
    stream = fopen(filename, "a+");
  }
  ~MapStream() {
    if (stream) fclose(stream);
  }
  FILE* stream;
};

std::atomic<int> counter(0);

namespace {
  MapStream* mstream = NULL;
}

void generate_f(const std::string& filename, const std::string& dump, int* max_index) {
  std::cout << "start encode:" << filename << std::endl;
  FILE* o = fopen(dump.c_str(), "w+");
  FILE* f = fopen(filename.c_str(), "r");
  char buf[1024 * 1024];
  std::vector<std::string> slices;
  std::vector<std::string> sub_slices;
  std::string str;
  while(NULL != fgets(buf, sizeof(buf), f)) {
    str.assign(buf);
    boost::trim(str);
    boost::split(slices, str, boost::is_any_of("\t "));
    if(slices.size() <= TOKEN_N) {continue;}
    boost::split(sub_slices, slices[2], boost::is_any_of(":"));
    fprintf(o, "1\t%s", sub_slices[0].c_str());
    bool dumped = false;
    for(int i = 3; i < slices.size(); ++i) {
      uint64_t key = MurmurHash64A(slices[i].c_str(), slices[i].length(), 0);
      if (!dumped) {
        if (string2id.count(key)) {
          fprintf(o, "\t%d:1", string2id.at(key));
          // printf("%d matched %s, hash:%lu\n", string2id.at(key), slices[i].c_str(), key);
          dumped = true;
        }
      } else {
        if (string2id.count(key)) {
          fprintf(o, " %d:1", string2id.at(key));
          // printf("%d matched %s, hash:%lu\n", string2id.at(key), slices[i].c_str(), key);
        }
      }
    }
    fprintf(o, "\n");
  }
  fclose(f);
  fclose(o);

  LOG(INFO) << filename + " encode done" << std::endl;

  if (FLAGS_remove_file) {
    LOG(WARNING) << "start remove " << filename;
    RemoveFilesHelper::Get()->Append(filename);
  }

  counter += 1;
}

typedef std::chrono::time_point<std::chrono::high_resolution_clock> mClock;
class TimeCost {
  public:
    TimeCost() {
      c1 = std::chrono::high_resolution_clock::now();
      c2 = std::chrono::high_resolution_clock::now();
    }
    void Break() {
      if (t1 <= t2) {
        c1 = std::chrono::high_resolution_clock::now();
        t1 = std::clock();
      } else {
        c2 = std::chrono::high_resolution_clock::now();
        t2 = std::clock();
      }
    }
    void Dump(const std::string& tag) {
      std::clock_t delta_t = t1 > t2 ? t1 - t2 : t2 - t1;

      std::cout << "********************" << tag << "******************\n"
                << "Wall clock time passed: "
                << std::chrono::duration<double, std::milli>(delta_t).count()
                << " ms\n";
    }
  private:
    std::clock_t t1 = 0;
    std::clock_t t2 = 0;
    mClock c1;
    mClock c2;

};

int main(int argc, char* argv[]) {
  TimeCost time_clock;
  time_clock.Break();
  google::ParseCommandLineFlags(&argc, &argv, true);
  time_clock.Break();
  time_clock.Dump("google parse command flags init");

  int index;
  char fname[1024];
  LineStream stream;
  stream.Load(FLAGS_map_path.c_str());
  stream.Init(2048 * 10, 128);
  time_clock.Break();
  time_clock.Dump("stream load file");
  char* str;
  int len;
  int max_index = -1;

  LOG(INFO) << "Featuresize:" << FLAGS_feature_size << std::endl
            << "Map path:" << FLAGS_map_path << std::endl
            << "Data path:" << FLAGS_data_path << std::endl;

  if (FLAGS_remove_file) RemoveFilesHelper::Get()->Run();

  string2id.reserve(FLAGS_feature_size);
  time_clock.Break();
  time_clock.Dump("string2id reserve memory");
  //str = (char*)malloc(1025 * 46);
  while (stream.GetLine(&str, &len)) {
    sscanf(str, "%d\t%s\t", &index, fname);
    uint64_t key = MurmurHash64A(fname, strlen(fname), 0);
    string2id[key] = index;
    // printf("feature:%s, hash:%lu\n", fname, key);
    max_index = max_index > index ? max_index : index;
  }
  time_clock.Break();
  time_clock.Dump("time clock load map");

  LOG(INFO) << "Feature map loaded succesfully";

  std::vector<std::string> data_files;

  FILE* data_file = fopen(FLAGS_data_path.c_str(), "r");

  char filename[1000];

  std::string filename_str;

  filename_str.reserve(1000);

  while(NULL != fgets(filename, sizeof(filename), data_file)) {
    filename_str.assign(filename);
    boost::trim(filename_str);
    data_files.push_back(filename_str);
  }
  fclose(data_file);

  mstream = new MapStream(FLAGS_map_path.c_str());

  ThreadPool pool;

  for(auto & data_filename: data_files) {
    std::string dump = data_filename + std::string("_id");
    LOG(INFO) << "Start Process File:" << data_filename
              << "\t DumpInto File:" << dump;
    pool.Submit(std::bind(&generate_f, data_filename, dump, &max_index));
  }

  while (counter < data_files.size()) {
    std::this_thread::yield();
  }
  delete mstream;

  if (FLAGS_remove_file) RemoveFilesHelper::Get()->Stop();

  return 0;
}
