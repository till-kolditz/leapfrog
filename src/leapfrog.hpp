#include <algorithm>
#include <cassert>
#include <cstddef>
#include <type_traits>
#include <vector>

using std::vector;

/**
 * @brief The paper requires the linear iterator to have certain time complexity
 * bounds on the interface methods, which are not really satisfied here, but
 * then, this is just a toy implementation using vectors.
 *
 * As a result, key() and at_end() have the desired time complexity O(1), while,
 * next() and seek() have time complexity O(N) instead of O(log N).
 */
template <typename T> class LinearIterator {
  vector<T> const *source;
  typename vector<T>::const_iterator iter;

public:
  LinearIterator(const vector<T> &source)
      : source{&source}, iter{source.begin()} {}
  LinearIterator(const LinearIterator &) noexcept = default;
  LinearIterator(LinearIterator &&) noexcept = delete;
  LinearIterator &operator=(const LinearIterator &) noexcept = default;
  LinearIterator &operator=(LinearIterator &&) noexcept = delete;
  ~LinearIterator() = default;

  T const &key() const noexcept {
    assert(iter != source->end());
    return *iter;
  }

  void next() {
    assert(iter != source->end());
    std::advance(iter, 1);
  }

  void seek(T seekKey)
    requires(std::is_arithmetic_v<T>)
  {
    assert(iter != source->end());
    assert(seekKey >= *iter);
    while (iter != source->end() && *iter < seekKey) {
      next();
    }
  }

  bool at_end() const noexcept { return iter == source->end(); }

  std::strong_ordering
  operator<=>(LinearIterator<T> const &other) const noexcept {
    assert(!at_end());
    assert(!other.at_end());
    return key() <=> other.key();
  }
};

template <typename T, typename... Ts>
concept are_same = std::conjunction_v<std::is_same<T, Ts>...>;

template <typename T> class LeapFrogJoin {
  using iterator_t = LinearIterator<T>;
  vector<iterator_t> iters;
  vector<iterator_t *> iters_p;
  bool atEnd{true};
  size_t pos{0};

  void init() {
    atEnd = true;
    iters_p.reserve(iters.size());
    for (iterator_t &iter : iters) {
      atEnd &= iter.at_end();
      iters_p.push_back(&iter);
    }
    if (!atEnd) {
      std::ranges::sort(
          iters_p,
          [](iterator_t const *iter1, iterator_t const *iter2) -> bool {
            return *iter1 < *iter2;
          });
      pos = 0;
      search();
    }
  }

  size_t prev_pos() const noexcept {
    return (pos + iters.size() - 1) % iters.size();
  }

  size_t next_pos() const noexcept { return (pos + 1) % iters.size(); }

public:
  template <typename... Args>
    requires(are_same<vector<T>, Args...>)
  LeapFrogJoin(Args &...sources) {
    constexpr size_t num_sources = sizeof...(sources);
    if (num_sources > 0) {
      iters.reserve(num_sources);
      for (const auto *source : {&sources...}) {
        iters.emplace_back(*source);
      }
    }
    init();
  }

  T key() {
    assert(!atEnd);
    return iters[0].key();
  }

  void search() {
    assert(!atEnd);
    T max_key = iters_p.at(prev_pos())->key();
    while (true) {
      iterator_t *cur_iter = iters_p.at(pos);
      T cur_key = cur_iter->key();
      if (cur_key == max_key) {
        break;
      } else {
        cur_iter->seek(max_key);
        if (cur_iter->at_end()) {
          atEnd = true;
          break;
        } else {
          max_key = cur_iter->key();
          pos = (pos + 1) % iters.size();
        }
      }
    }
  }

  void next() {
    assert(!atEnd);
    iterator_t *cur_iter = iters_p.at(pos);
    cur_iter->next();
    if (cur_iter->at_end()) {
      atEnd = true;
    } else {
      pos = next_pos();
      search();
    }
  }

  void seek(int seekKey) {
    assert(!atEnd);
    iterator_t *cur_iter = iters_p.at(pos);
    cur_iter->seek(seekKey);
    if (cur_iter->at_end()) {
      atEnd = true;
    } else {
      pos = next_pos();
      search();
    }
  }

  bool at_end() const noexcept { return atEnd; }
};
