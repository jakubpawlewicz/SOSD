#pragma once

#include "../util.h"
#include "base.h"
#include "FastMap.h"

template <class KeyType, int size_scale>
class FastMapBucket : public Competitor {
 public:
  uint64_t Build(const std::vector<KeyValue<KeyType>>& data) {
    Upp::Vector<KeyType> keys;
    keys.SetCount((int)data.size());
    for (int i = 0; i < keys.GetCount(); i++)
      keys[i] = data[i].key;
    return util::timing([&] {
      idx_.Create(keys, params_);
    });
  }

  SearchBound EqualityLookup(const KeyType lookup_key) const {
    FastMap::Intvl intvl = idx_->FindIntvl(lookup_key);
    return {(size_t)intvl.a, (size_t)intvl.b};
  }

  std::string name() const { return "FastMapBucket"; }

  std::size_t size() const { return idx_->GetMemSize(); }

  bool applicable(bool _unique, const std::string& data_filename) {
    params_.lambda = (double)size_scale;
    return true;;
  }

  int variant() const { return size_scale; }

 private:
  FastMap::Params params_;
  Upp::One<FastMap::BucketPile<KeyType>> idx_;
};
