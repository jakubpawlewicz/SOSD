#pragma once

#include "../util.h"
#include "base.h"
#include "FastMap.h"

template <class KeyType, bool make_signed>
class FastMapUtils {
 protected:
  using AdjustedKeyType = std::conditional_t<make_signed, std::make_signed_t<KeyType>, std::make_unsigned_t<KeyType>>;
  static AdjustedKeyType AdjustKey(KeyType key)
  {
    if constexpr (std::is_signed_v<KeyType> == make_signed)
      return key;
    else
      return key ^ std::numeric_limits<AdjustedKeyType>::min();
  }

  Upp::Vector<AdjustedKeyType> GetKeys(const std::vector<KeyValue<KeyType>>& data) const {
    Upp::Vector<AdjustedKeyType> keys;
    keys.SetCount((int)data.size());
    for (int i = 0; i < keys.GetCount(); i++)
      keys[i] = AdjustKey(data[i].key);
    return keys;
  }

  struct Range {
    const std::vector<KeyValue<KeyType>> *data_;
    int begin_;
    int count_;
    int GetCount() const { return count_; }
    AdjustedKeyType operator[](int i) const { return AdjustKey((*data_)[begin_ + i].key); }
    Range(const std::vector<KeyValue<KeyType>> *data, int begin, int count)
      : data_(data), begin_(begin), count_(count) {}
  };
};

template <template <class> class Index, class KeyType, bool make_signed, int size_scale>
class FastMapBase : public FastMapUtils<KeyType, make_signed>, public Competitor {
  typedef FastMapUtils<KeyType, make_signed> B;
 protected:
  typedef typename B::AdjustedKeyType AdjustedKeyType;
  using B::AdjustKey;

  uint64_t Build(const Upp::Vector<AdjustedKeyType>& keys) {
    last_idx_ = keys.size() - 1;
    while (last_idx_ > 0 && keys[last_idx_ - 1] == keys[last_idx_])
      last_idx_--;
    last_key_ = keys[last_idx_];
    return util::timing([&] {
      Upp::String fn(data_filename_);
      fn = Upp::GetHomeDirectory() + "/src/SOSD/fastmap_data/" + Upp::GetFileName(fn);
      fn << '-' << name() << '-' << Upp::IntStr(size_scale) << ".hints";
      if (Upp::FileExists(fn))
      {
        Upp::FileIn in(fn);
        idx_.Create(keys, in);
      }
      else
        idx_.Create(keys, params_);
    });
  }
 public:
  uint64_t Build(const std::vector<KeyValue<KeyType>>& data) {
    return Build(B::GetKeys(data));
  }

  std::string BuildInfo() const {
    return idx_->GetInfo().ToStd();
  }

  SearchBound EqualityLookup(const KeyType _lookup_key) const {
    AdjustedKeyType lookup_key = AdjustKey(_lookup_key);
    if (lookup_key == last_key_)
      return {last_idx_, last_idx_ + 1};
    FastMap::Intvl intvl = idx_->FindIntvl(lookup_key);
    intvl.a = Upp::max(0, intvl.a);
    intvl.b = Upp::min((int)last_idx_, intvl.b);
    return {(size_t)intvl.a, (size_t)intvl.b};
  }

  std::size_t size() const { return idx_->GetMemSize(); }

  int variant() const { return size_scale; }
  virtual std::string name() const = 0;

 protected:
  std::string data_filename_;
  FastMap::Params params_;
  Upp::One<Index<AdjustedKeyType>> idx_;
  size_t last_idx_;
  AdjustedKeyType last_key_;
};

template <template <class> class Index, class KeyType, bool make_signed, int size_scale>
class FastMapApxBase : public FastMapBase<Index, KeyType, make_signed, size_scale> {
  typedef FastMapBase<Index, KeyType, make_signed, size_scale> B;
  using B::idx_;
  using B::AdjustKey;
  typedef typename B::AdjustedKeyType AdjustedKeyType;
 public:
  uint64_t Build(const std::vector<KeyValue<KeyType>>& data) {
    data_ = &data;
    return B::Build(data);
  }

  SearchBound EqualityLookup(const KeyType _lookup_key) const {
    AdjustedKeyType lookup_key = AdjustKey(_lookup_key);
    if (lookup_key == B::last_key_)
      return {B::last_idx_, B::last_idx_ + 1};
    typename Index<AdjustedKeyType>::State state;
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
      else
        return {(size_t)intvl.a, (size_t)intvl.b};
    }
  }
 protected:
  const std::vector<KeyValue<KeyType>> *data_;
};

template <class KeyType, int size_scale>
class FastMapBucket : public FastMapBase<FastMap::BucketPile, KeyType, true, size_scale> {
  typedef FastMapBase<FastMap::BucketPile, KeyType, true, size_scale> B;
  using B::params_;
 public:
  std::string name() const { return "FastMapBucket"; }

  bool applicable(bool _unique, const std::string& data_filename) {
    B::data_filename_ = data_filename;
    int s = size_scale;
    params_.decay = 1.0 / (double) (s % 10);
    s /= 10;
    params_.ends_to_accept = 32;
    params_.eps = double(s % 1000) / 10.0;
    params_.lambda = (double)(s / 1000);
    return true;
  }
};

template <class KeyType, int size_scale>
class FastMapPGM : public FastMapBase<FastMap::LinApxOptSinglePile, KeyType, false, size_scale> {
  typedef FastMapBase<FastMap::LinApxOptSinglePile, KeyType, false, size_scale> B;
  using B::params_;
 public:
  std::string name() const { return "FastMapPGM"; }

  bool applicable(bool _unique, const std::string& data_filename) {
    B::data_filename_ = data_filename;
    params_.avg_reads = 1.0;
    params_.block_size = size_scale / 8;
    params_.level_search_type = size_scale % 8;
    return true;
  }
};

template <template <class> class Index, class KeyType, int size_scale>
class FastMapRegBucketBase : public FastMapBase<Index, KeyType, false, size_scale> {
  typedef FastMapBase<Index, KeyType, false, size_scale> B;
  using B::params_;
 public:
  bool applicable(bool _unique, const std::string& data_filename) {
    B::data_filename_ = data_filename;
    static int sizes[10] = { 402653216, 201326624, 100663328, 41943040, 12582944, 6291488, 3145760, 786464, 24608, 3088 };
    params_.memory_target = sizes[size_scale % 10];
    params_.avg_reads = 1.0;
    params_.block_size = Upp::Null;
    params_.memory_cost = Upp::Null;
    params_.level_search_type = size_scale / 10;
    return true;
  }
};

template <class KeyType, int size_scale>
class FastMapRegBucketSingle : public FastMapRegBucketBase<FastMap::RegBucketSinglePile, KeyType, size_scale> {
 public:
  std::string name() const { return "FastMapRegBucketSingle"; }
};

template <class KeyType, int size_scale>
class FastMapRegBucketDouble : public FastMapRegBucketBase<FastMap::RegBucketDoublePile, KeyType, size_scale> {
 public:
  std::string name() const { return "FastMapRegBucketDouble"; }
};

template <class KeyType, int size_scale>
class FastMapPGMRegBucketSingle : public FastMapRegBucketBase<FastMap::LinApxOptRegBucketPile, KeyType, size_scale> {
 public:
  std::string name() const { return "FastMapPGMRegBucketSingle"; }
};

template <class KeyType, int size_scale>
class FastMapPGMRegBucketDouble : public FastMapRegBucketBase<FastMap::LinApxOptDoubleRegBucketPile, KeyType, size_scale> {
 public:
  std::string name() const { return "FastMapPGMRegBucketDouble"; }
};

template <class KeyType, int size_scale>
class FastMapPGMFull : public FastMapBase<FastMap::LinApxOptPile, KeyType, true, size_scale> {
  typedef FastMapBase<FastMap::LinApxOptPile, KeyType, true, size_scale> B;
  using B::params_;
 public:
  std::string name() const { return "FastMapPGMFull"; }

  bool applicable(bool _unique, const std::string& data_filename) {
    B::data_filename_ = data_filename;
    params_.avg_reads = 1.0;
    params_.block_size = size_scale / 100;
    params_.deep_block_size = size_scale % 100;
    return true;
  }
};

template <class KeyType, int size_scale>
class FastMapPGMBucket : public FastMapBase<FastMap::LinApxOptBucketPile, KeyType, true, size_scale> {
  typedef FastMapBase<FastMap::LinApxOptBucketPile, KeyType, true, size_scale> B;
  using B::params_;
 public:
  std::string name() const { return "FastMapPGMBucket"; }

  bool applicable(bool _unique, const std::string& data_filename) {
    B::data_filename_ = data_filename;
    int s = size_scale;
    params_.avg_reads = 1.0;
    params_.decay = 1.0 / (double) (size_scale % 10);
    s /= 10;
    params_.ends_to_accept = 32;
    params_.eps = double(s % 1000) / 10.0;
    s /= 1000;
    params_.lambda = double(s % 10);
    params_.block_size = s / 10;
    return true;
  }
};

template <class KeyType, int size_scale>
class FastMapApx : public FastMapApxBase<FastMap::LinApxOptBucketPile, KeyType, true, size_scale> {
  typedef FastMapApxBase<FastMap::LinApxOptBucketPile, KeyType, true, size_scale> B;
  using B::params_;
 public:
  std::string name() const { return "FastMapApx"; }

  bool applicable(bool _unique, const std::string& data_filename) {
    B::data_filename_ = data_filename;
    params_.lambda = 4.0;
    params_.avg_reads = double(size_scale % 1000) * 0.1;
    params_.block_size = size_scale / 1000;
    return true;
  }
};
