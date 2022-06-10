#pragma once

#include "../util.h"
#include "base.h"
#include "FastMap.h"

template <template <class> class Index, class KeyType, int size_scale>
class FastMapBase : public Competitor {
 protected:
  static KeyType AdjustKey(KeyType key)
  {
    if constexpr (std::is_signed_v<KeyType>)
      return key;
    else
      return key ^ std::numeric_limits<KeyTypeSigned>::min();
  }
  typedef std::make_signed_t<KeyType> KeyTypeSigned;

  Upp::Vector<KeyTypeSigned> GetKeys(const std::vector<KeyValue<KeyType>>& data) const {
    Upp::Vector<KeyTypeSigned> keys;
    keys.SetCount((int)data.size());
    for (int i = 0; i < keys.GetCount(); i++)
      keys[i] = (KeyTypeSigned)AdjustKey(data[i].key);
    return keys;
  }

  uint64_t Build(const Upp::Vector<KeyTypeSigned>& keys) {
    return util::timing([&] {
      idx_.Create(keys, params_);
    });
  }
 public:
  uint64_t Build(const std::vector<KeyValue<KeyType>>& data) {
    return Build(GetKeys(data));
  }

  SearchBound EqualityLookup(const KeyType lookup_key) const {
    FastMap::Intvl intvl = idx_->FindIntvl((KeyTypeSigned)AdjustKey(lookup_key));
    return {(size_t)intvl.a, (size_t)intvl.b};
  }

  std::size_t size() const { return idx_->GetMemSize(); }

  int variant() const { return size_scale; }

 protected:
  FastMap::Params params_;
  Upp::One<Index<KeyTypeSigned>> idx_;
};

template <template <class> class Index, class KeyType, int size_scale>
class FastMapApxBase : public FastMapBase<Index, KeyType, size_scale> {
  typedef FastMapBase<Index, KeyType, size_scale> B;
  using B::idx_;
  using B::AdjustKey;
  typedef typename B::KeyTypeSigned KeyTypeSigned;
 public:
  uint64_t Build(const std::vector<KeyValue<KeyType>>& data) {
    keys_ = B::GetKeys(data);
    return B::Build(keys_);
  }

  SearchBound EqualityLookup(const KeyType _lookup_key) const {
    KeyTypeSigned lookup_key = (KeyTypeSigned)AdjustKey(_lookup_key);
    typename Index<KeyTypeSigned>::State state;
    FastMap::Intvl intvl = idx_->TryFindFirst(lookup_key, state);
    for(;;) {
      if (intvl.b > keys_.GetCount()) {
        intvl.a += keys_.GetCount() - intvl.b;
        intvl.b = keys_.GetCount();
      }
      intvl.a = Upp::max(0, intvl.a);
      auto range = Upp::SubRange(keys_, intvl.a, intvl.b - intvl.a);
      if (lookup_key < range[0])
        intvl = idx_->TryFindNext(lookup_key, intvl, range, intvl.a, state);
      else if (lookup_key > range[range.GetCount() - 1])
        intvl = idx_->TryFindNext(lookup_key, intvl, range, intvl.b, state);
      else
        return {(size_t)intvl.a, (size_t)intvl.b};
    }
  }
 protected:
  Upp::Vector<KeyTypeSigned> keys_;
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
    params_.block_size = size_scale / 100;
    params_.deep_block_size = size_scale % 100;
    return true;
  }
};

template <class KeyType, int size_scale>
class FastMapPGMBucket : public FastMapBase<FastMap::LinApxOptBucketPile, KeyType, size_scale> {
  typedef FastMapBase<FastMap::LinApxOptBucketPile, KeyType, size_scale> B;
  using B::params_;
 public:
  std::string name() const { return "FastMapPGMBucket"; }

  bool applicable(bool _unique, const std::string& data_filename) {
    params_.avg_reads = 1.0;
    params_.block_size = size_scale / 100;
    params_.lambda = double(size_scale % 100);
    return true;
  }
};

template <class KeyType, int size_scale>
class FastMapApx : public FastMapApxBase<FastMap::LinApxOptBucketPile, KeyType, size_scale> {
  typedef FastMapApxBase<FastMap::LinApxOptBucketPile, KeyType, size_scale> B;
  using B::params_;
 public:
  std::string name() const { return "FastMapApx"; }

  bool applicable(bool _unique, const std::string& data_filename) {
    params_.lambda = 4.0;
    params_.avg_reads = double(size_scale % 1000) * 0.1;
    params_.block_size = size_scale / 1000;
    return true;
  }
};
