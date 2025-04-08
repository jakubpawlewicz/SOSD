#pragma once

#include "../util.h"
#include "base.h"
#include "rs/builder.h"
#include "rs/radix_spline.h"

template <class KeyType, int size_scale>
class RS : public Competitor {
 public:
  uint64_t Build(const std::vector<KeyValue<KeyType>>& data) {
    if (!parameters_set_) util::fail("RS parameters not set.");

    return util::timing([&] {
      auto min = std::numeric_limits<KeyType>::min();
      auto max = std::numeric_limits<KeyType>::max();
      if (data.size() > 0) {
        min = data.front().key;
        max = data.back().key;
      }
      rs::Builder<KeyType> rsb(min, max, num_radix_bits_, max_error_);
      for (const auto& key_and_value : data) rsb.AddKey(key_and_value.key);
      rs_ = rsb.Finalize();
    });
  }

  SearchBound EqualityLookup(const KeyType lookup_key) const {
    const rs::SearchBound sb = rs_.GetSearchBound(lookup_key);
    return {sb.begin, sb.end};
  }

  std::string name() const { return "RS"; }

  std::size_t size() const { return rs_.GetSize(); }

  bool applicable(bool _unique, const std::string& data_filename) {
    num_radix_bits_ = size_scale % 100;
    max_error_ = 1 << (size_scale / 100);
    parameters_set_ = true;
    return true;
}

  int variant() const { return size_scale; }

 private:
  rs::RadixSpline<KeyType> rs_;
  size_t num_radix_bits_;
  size_t max_error_;
  bool parameters_set_ = false;
};
