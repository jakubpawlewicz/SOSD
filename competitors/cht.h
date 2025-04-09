#pragma once

#include "../util.h"
#include "base.h"
#include "cht/builder.h"
#include "cht/cht.h"

template <class KeyType, int size_scale>
class CHT : public Competitor {
 public:
  uint64_t Build(const std::vector<KeyValue<KeyType>>& data) {
    if (!parameters_set_) util::fail("CHT parameters not set.");

    return util::timing([&] {
      auto min = std::numeric_limits<KeyType>::min();
      auto max = std::numeric_limits<KeyType>::max();
      if (data.size() > 0) {
        min = data.front().key;
        max = data.back().key;
      }
      cht::Builder<KeyType> chtb(min, max, num_bins_, max_error_,
                                 /*single_pass=*/false, /*use_cache=*/false);
      for (const auto& key_and_value : data) chtb.AddKey(key_and_value.key);
      cht_ = chtb.Finalize();
    });
  }

  SearchBound EqualityLookup(const KeyType lookup_key) const {
    const cht::SearchBound sb = cht_.GetSearchBound(lookup_key);
    return {sb.begin, sb.end};
  }

  std::string name() const { return "CHT"; }

  std::size_t size() const { return cht_.GetSize(); }

  bool applicable(bool _unique, const std::string& data_filename) {
    int s = size_scale % 100;
    int m = size_scale / 100;
    // s = 14 -> e = 4
    // s = 28, m = 4 -> e = 4
    // s = 28, m = 10 -> e = 20
    int e = (m - 4) * (s - 14) * 4 / 21 + 4;
    num_bins_ = 1 << (s - e);
    max_error_ = 1 << e;
    parameters_set_ = true;
    return true;
  }

  int variant() const { return size_scale; }

 private:
  cht::CompactHistTree<KeyType> cht_;
  size_t num_bins_ = 64;
  size_t max_error_;
  bool parameters_set_ = false;
};
