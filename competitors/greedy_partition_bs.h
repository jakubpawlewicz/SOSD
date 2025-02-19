#pragma once

#include <cmath>
#include <cstdint>
#include <string>
#include <vector>
#include "base.h"
#include "util.h"

double _Z_stat(const std::vector<size_t>& bucket_rank, size_t bucket_count) {
  uint64_t result = 0;
  for (size_t i = 0; i < bucket_count; ++i) {
    uint64_t bucket_size = bucket_rank[i + 1] - bucket_rank[i];
    result += bucket_size * bucket_size;
  }
  return result / double(bucket_rank[bucket_count]);
}

template <class KeyType, size_t error, typename FloatType = double>
class SimpleBucketIndex : public Competitor {
 public:
  uint64_t Build(const std::vector<KeyValue<KeyType>>& data) {
    Build(data.begin(), data.end());
    return 0;
  }

  double Build(std::vector<KeyValue<KeyType>>::const_iterator beg, std::vector<KeyValue<KeyType>>::const_iterator end) {
    n = end - beg;
    bucket_count = (n + error - 1) / error;
    min_key = beg->key;
    auto max_key = std::prev(end)->key;
    bucket_span = FloatType(max_key - min_key) / bucket_count;
    rev_bucket_span = bucket_count / FloatType(max_key - min_key);
    bucket_rank = std::vector<size_t>(bucket_count + 2);
    auto smaller_than_bucket = beg;
    for (size_t i = 0; i < bucket_count + 1; ++i) {
      FloatType bucket_lower_bound = min_key + bucket_span * i; 
      while (smaller_than_bucket != end && smaller_than_bucket->key < bucket_lower_bound) {
        ++smaller_than_bucket;
      }
      bucket_rank[i] = smaller_than_bucket - beg;
    }
    bucket_rank.back() = n;
    return _Z_stat(bucket_rank, bucket_count);
  }

  SearchBound EqualityLookup(const KeyType lookup_key) const {
    auto bucket = size_t((lookup_key - min_key) * rev_bucket_span);
    // add -1 or +1 to account for floating point errors
    return (SearchBound){std::max(size_t(bucket_rank[bucket] - 1), size_t(0)), std::min(bucket_rank[bucket + 1] + 1, n)};
  }

  std::string name() const { return "SimpleBucketIndex/" + std::to_string(error) + "/Z:" + std::to_string(_Z_stat(bucket_rank, bucket_count)); }

  std::size_t size() const { return bucket_rank.size() * sizeof(size_t); }

 private:
  size_t n, bucket_count;
  KeyType min_key, max_key;
  FloatType bucket_span, rev_bucket_span;
  std::vector<size_t> bucket_rank;
};

template <class KeyType, size_t error>
class BitwiseBucketIndex : public Competitor {
 public:
  uint64_t Build(const std::vector<KeyValue<KeyType>>& data) {
    Build(data.begin(), data.end());
    return 0;
  }

  double Build(std::vector<KeyValue<KeyType>>::const_iterator beg, std::vector<KeyValue<KeyType>>::const_iterator end) {
    n = end - beg;
    bucket_count = (n + error - 1) / error;
    min_key = beg->key;
    max_key = std::prev(end)->key;
    KeyType bucket_span;
    if (min_key == max_key)
      bucket_span = 0;
    else {
      bucket_span = 1 + (max_key - min_key - 1) / bucket_count;
      bucket_span = std::min(bucket_span, KeyType(-1) >> 2);
    }
    bit_shift = 0;
    while ((KeyType(1) << bit_shift) <= bucket_span) {
      ++bit_shift;
    }
    bucket_count = ((max_key - min_key) >> bit_shift) + 1;
    bucket_rank = std::vector<size_t>(bucket_count + 1);
    auto smaller_than_bucket = beg;
    for (size_t i = 0; i < bucket_count + 1; ++i) {
      KeyType bucket_lower_bound = min_key + (KeyType(i) << bit_shift);
      while (smaller_than_bucket != end && smaller_than_bucket->key < bucket_lower_bound) {
        ++smaller_than_bucket;
      }
      bucket_rank[i] = smaller_than_bucket - beg;
    }
    bucket_rank.back() = n;
    return _Z_stat(bucket_rank, bucket_count);
  }

  SearchBound EqualityLookup(const KeyType lookup_key) const {
    size_t bucket = (lookup_key - min_key) >> bit_shift;
    assert(lookup_key >= min_key && lookup_key <= max_key);
    return (SearchBound){bucket_rank[bucket], bucket_rank[bucket + 1]};
  }

  std::string name() const {
    return "BinaryBucketIndex/" + std::to_string(error) + "/2^" + std::to_string(bit_shift) + "/Z:" + std::to_string(_Z_stat(bucket_rank, bucket_count));
  }

  std::size_t size() const { return bucket_rank.size() * sizeof(size_t); }

 private:
  size_t n, bucket_count;
  int bit_shift;
  KeyType min_key, max_key;
  std::vector<size_t> bucket_rank;
};

template <class KeyType, size_t error, typename LeafIndex = BitwiseBucketIndex<KeyType, error>>
class GreedyPartitionIndex : public Competitor {
 public:
  uint64_t Build(const std::vector<KeyValue<KeyType>>& data) {
    double l = 1.1, r = 400.0, best = r;
    n = data.size();
    auto fill_index = [&](double threshold, size_t size_limit) -> void {
      top_level_offsets.clear();
      indices.clear();
      auto start = data.begin();
      while (start != data.end() && top_level_offsets.size() <= size_limit) {
        auto limit = data.end();
        if ((data.end() - start) > 8LL * n / error) {
          limit = start + 8LL * n / error;
        }
        auto end = cut_prefix(start, limit, threshold);
        top_level_offsets.push_back({start->key, start - data.begin()});
        indices.push_back(LeafIndex());
        indices.back().Build(start, end);
        start = end;
      }
    };
    while (r > 1.1 * l && r - l > 0.1) {
      double mid = (l + r) / 2;
      fill_index(mid, error);
      if (top_level_offsets.size() <= error) {
        best = mid;
        r = mid;
      } else {
        l = mid;
      }
    }
    z_multiplier = best;
    fill_index(z_multiplier, n);
    return 0;
  }

  std::vector<KeyValue<KeyType>>::const_iterator cut_prefix(std::vector<KeyValue<KeyType>>::const_iterator beg, std::vector<KeyValue<KeyType>>::const_iterator end, double threshold) {
    auto d = end - beg, l = 1, r = d, best = 1; // ??
    while (l <= r) {
      auto mid = (l + r) / 2;
      LeafIndex index;
      auto z_stat = index.Build(beg, beg + mid);
      if (z_stat <= error * threshold) {
        best = mid;
        l = mid + 1;
      } else {
        r = mid - 1;
      }
    }
    // if there are key duplicates for some reason
    while (beg + best != end && (beg + best)->key == (beg + best - 1)->key) {
      ++best;
    }
    return beg + best;
  }

  SearchBound EqualityLookup(const KeyType lookup_key) const {
    auto it = std::prev(std::upper_bound(top_level_offsets.begin(), top_level_offsets.end(), std::pair<KeyType, size_t>{lookup_key, n}));
    auto bottom_level_result = indices[it - top_level_offsets.begin()].EqualityLookup(lookup_key);
    return SearchBound{bottom_level_result.start + it->second, bottom_level_result.stop + it->second};
  }

  std::string name() const { return "GreedyPartitionIndex/error: " + std::to_string(error) + "/parts: " + std::to_string(top_level_offsets.size()) + "/z_mult: " + std::to_string(z_multiplier); }

  std::size_t size() const { 
    size_t total = top_level_offsets.size() * sizeof(std::pair<KeyType, size_t>);
    for (const auto &ind : indices) {
      total += ind.size();
    }
    return total;
  }

 private:
  size_t n;
  std::vector<std::pair<KeyType, size_t>> top_level_offsets;
  std::vector<LeafIndex> indices;
  double z_multiplier;
};


