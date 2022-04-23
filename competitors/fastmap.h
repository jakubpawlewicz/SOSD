#pragma once

#include "../util.h"
#include "base.h"
#include "FastMap.h"

template <template <class> class Index, class KeyType, int size_scale>
class FastMapBase : public Competitor {
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

  std::size_t size() const { return idx_->GetMemSize(); }

  int variant() const { return size_scale; }

 protected:
  FastMap::Params params_;
  Upp::One<Index<KeyType>> idx_;
};

template <class KeyType, int size_scale>
class FastMapBucket : public FastMapBase<FastMap::BucketPile, KeyType, size_scale> {
  typedef FastMapBase<FastMap::BucketPile, KeyType, size_scale> B;
  using B::params_;
 public:
  std::string name() const { return "FastMapBucket"; }

  bool applicable(bool _unique, const std::string& data_filename) {
    params_.lambda = (double)size_scale;
    return true;
  }
};

template <class KeyType, int size_scale>
class FastMapPGMFull : public FastMapBase<FastMap::LinApxOptPile, KeyType, size_scale> {
  typedef FastMapBase<FastMap::LinApxOptPile, KeyType, size_scale> B;
  using B::params_;
 public:
  std::string name() const { return "FastMapPGMFull"; }

  bool applicable(bool _unique, const std::string& data_filename) {
    params_.avg_reads = 1.0;
    params_.block_size = (double)size_scale + 2;
    params_.deep_block_size = 10;
    return true;
  }
};
