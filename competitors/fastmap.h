#pragma once

#include "../util.h"
#include "base.h"
#include "FastMap.h"

template <class KeyType>
class FastMapUtils {
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

  struct Range {
    const std::vector<KeyValue<KeyType>> *data_;
    int begin_;
    int count_;
    int GetCount() const { return count_; }
    KeyTypeSigned operator[](int i) const { return AdjustKey((*data_)[begin_ + i].key); }
    Range(const std::vector<KeyValue<KeyType>> *data, int begin, int count)
      : data_(data), begin_(begin), count_(count) {}
  };
};

template <template <class> class Index, class KeyType, int size_scale>
class FastMapBase : public FastMapUtils<KeyType>, public Competitor {
  typedef FastMapUtils<KeyType> B;
 protected:
  typedef typename B::KeyTypeSigned KeyTypeSigned;
  using B::AdjustKey;

  uint64_t Build(const Upp::Vector<KeyTypeSigned>& keys) {
    last_idx_ = keys.size() - 1;
    while (last_idx_ > 0 && keys[last_idx_ - 1] == keys[last_idx_])
      last_idx_--;
    last_key_ = keys[last_idx_];
    return util::timing([&] {
      idx_.Create(keys, params_);
    });
  }
 public:
  uint64_t Build(const std::vector<KeyValue<KeyType>>& data) {
    return Build(B::GetKeys(data));
  }

  SearchBound EqualityLookup(const KeyType _lookup_key) const {
    KeyTypeSigned lookup_key = (KeyTypeSigned) AdjustKey(_lookup_key);
    if (lookup_key == last_key_)
      return {last_idx_, last_idx_ + 1};
    FastMap::Intvl intvl = idx_->FindIntvl(lookup_key);
    intvl.a = Upp::max(0, intvl.a);
    intvl.b = Upp::min((int)last_idx_, intvl.b);
    return {(size_t)intvl.a, (size_t)intvl.b};
  }

  std::size_t size() const { return idx_->GetMemSize(); }

  int variant() const { return size_scale; }

 protected:
  FastMap::Params params_;
  Upp::One<Index<KeyTypeSigned>> idx_;
  size_t last_idx_;
  KeyTypeSigned last_key_;
};

template <template <class> class Index, class KeyType, int size_scale>
class FastMapApxBase : public FastMapBase<Index, KeyType, size_scale> {
  typedef FastMapBase<Index, KeyType, size_scale> B;
  using B::idx_;
  using B::AdjustKey;
  typedef typename B::KeyTypeSigned KeyTypeSigned;
 public:
  uint64_t Build(const std::vector<KeyValue<KeyType>>& data) {
    data_ = &data;
    return B::Build(data);
  }

  SearchBound EqualityLookup(const KeyType _lookup_key) const {
    KeyTypeSigned lookup_key = (KeyTypeSigned)AdjustKey(_lookup_key);
    if (lookup_key == B::last_key_)
      return {B::last_idx_, B::last_idx_ + 1};
    typename Index<KeyTypeSigned>::State state;
    FastMap::Intvl intvl = idx_->TryFindFirst(lookup_key, state);
    for(;;) {
      if (intvl.b > (int) data_->size()) {
        intvl.a += (int) data_->size() - intvl.b;
        intvl.b = (int) data_->size();
      }
      intvl.a = Upp::max(0, intvl.a);
      auto range = typename B::Range(data_, intvl.a, intvl.b - intvl.a);
      if (_lookup_key < (*data_)[intvl.a].key)
        intvl = idx_->TryFindNext(lookup_key, intvl, range, intvl.a, state);
      else if (_lookup_key == (*data_)[intvl.a].key) {
        while (intvl.a > 0 && _lookup_key == (*data_)[intvl.a - 1].key)
          intvl.a--;
        return {(size_t)intvl.a, (size_t)intvl.a};
      }
      else if (_lookup_key > (*data_)[intvl.b - 1].key)
        intvl = idx_->TryFindNext(lookup_key, intvl, range, intvl.b, state);
      else if (_lookup_key == (*data_)[intvl.b - 1].key) {
        while (intvl.b < (int) data_->size() && _lookup_key == (*data_)[intvl.b].key)
          intvl.b++;
        intvl.b--;
        return {(size_t)intvl.b, (size_t)intvl.b};
      }
      else
        return {(size_t)intvl.a, (size_t)intvl.b};
    }
  }
 protected:
  const std::vector<KeyValue<KeyType>> *data_;
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
