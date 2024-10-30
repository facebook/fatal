/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <fatal/container/random_access_iterator.h>

#include <fatal/debug/assume.h>
#include <fatal/functional/functional.h>
#include <fatal/portability.h>

#include <algorithm>
#include <array>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

#include <cassert>

FATAL_DIAGNOSTIC_PUSH
FATAL_GCC_DIAGNOSTIC_IGNORED_SHADOW_IF_BROKEN

namespace fatal {

// TODO: shrink_to_fit, pop_back, prevent growth (T4534263)
template <typename T>
class circular_queue {
  static_assert(!std::is_reference<T>::value, "can't store references");

  union item_t {
    item_t() noexcept {}
    item_t(item_t const &) noexcept {}
    item_t(item_t &&) noexcept {}
    ~item_t() noexcept {}
    T value;
  };

  // TODO: make it a customizable container
  using queue_type = std::vector<item_t>;

public:
  using size_type = typename queue_type::size_type;
  using value_type = T;
  using const_reference = typename std::add_lvalue_reference<
    typename std::add_const<T>::type
  >::type;
  using reference = typename std::add_lvalue_reference<T>::type;
  using const_pointer = typename std::add_pointer<
    typename std::add_const<T>::type
  >::type;
  using pointer = typename std::add_pointer<T>::type;

private:
  size_type loose_real_index(fatal::fast_pass<size_type> i) const {
    FATAL_ASSUME_LE(i, size_);
    auto const index = offset_ + i;
    auto const size = queue_.size();
    auto result = index < size ? index : index - size;
    FATAL_ASSUME_LT(result, size);
    return result;
  }

  size_type real_index(fatal::fast_pass<size_type> i) const {
    FATAL_ASSUME_LT(i, size_);
    return loose_real_index(i);
  }

  size_type append_index() const {
    auto const index = offset_ + size_;
    auto const size = queue_.size();
    auto result = index < size ? index : index - size;
    FATAL_ASSUME_ONE(
      FATAL_LT(result, size),
      FATAL_EQ(size, static_cast<size_type>(0))
    );
    return result;
  }

  size_type prepend_index() const {
    if (queue_.empty()) {
      return 0;
    }

    auto const result = offset_ ? offset_ - 1 : queue_.size() - 1;
    FATAL_ASSUME_LT(result, queue_.size());
    return result;
  }

  using growth_rate = std::integral_constant<std::size_t, 2>;
  using chunk_range = std::pair<size_type, size_type>;
  using chunk_pair = std::array<chunk_range, 2>;

  chunk_pair chunks() const {
    FATAL_ASSUME_LE(offset_, queue_.size());
    auto const gap = queue_.size() - offset_;

    chunk_pair chunk;

    chunk[0].first = offset_;
    chunk[1].first = 0;

    if (gap < size_) {
      // elements both in [offset_, queue_.size() - 1] and [0, offset_)
      chunk[0].second = queue_.size();
      chunk[1].second = size_ - gap;
    } else {
      // all elements in [offset_, queue_.size())
      chunk[0].second = offset_ + size_;
      chunk[1].second = 0;
    }

    FATAL_ASSUME_LE(chunk[0].first, chunk[0].second);
    FATAL_ASSUME_LE(chunk[1].first, chunk[1].second);
    return chunk;
  }

  void grow() {
    FATAL_ASSUME_EQ(size_, queue_.size());
    queue_type grown;
    grown.resize(queue_.empty() ? 1 : queue_.size() * growth_rate::value);
    FATAL_ASSUME_LT(size_, grown.size());


    auto destination = grown.begin();

    for (auto chunk: chunks()) {
      for(auto i = chunk.first; i < chunk.second; ++i, ++destination) {
        new (std::addressof(destination->value)) value_type(
          std::move(queue_[i].value)
        );

        queue_[i].value.~value_type();
      }
    }

    FATAL_ASSUME_NOT_NEGATIVE(std::distance(grown.begin(), destination));
    FATAL_ASSUME_EQ(
      size_,
      static_cast<size_type>(std::distance(grown.begin(), destination))
    );
    queue_ = std::move(grown);
    offset_ = 0;

    FATAL_ASSUME_LT(size_, queue_.size());
  }

public:
  circular_queue() = default;

  // TODO: PROPERLY DECLARE noexcept
  explicit circular_queue(size_type capacity) {
    queue_.reserve(capacity);
  }

  // TODO: PROPERLY DECLARE noexcept
  circular_queue(circular_queue const &other):
    queue_(other.queue_.size()),
    size_(other.size_)
  {
    auto j = offset_;
    for (auto chunk: other.chunks()) {
      for(auto i = chunk.first; i < chunk.second; ++i, ++j) {
        auto p = new (std::addressof(queue_[j].value)) value_type(
          other.queue_[i].value
        );
      }
    }
    assert(*this == other);
  }

  // TODO: PROPERLY DECLARE noexcept
  circular_queue(circular_queue &&other):
    queue_(std::move(other.queue_)),
    offset_(other.offset_),
    size_(other.size_)
  {
    other.size_ = 0;
    other.offset_ = 0;
    assert(other.queue_.empty());
  }

  ~circular_queue() noexcept { clear(); }

  reference push_front(value_type const &value) { return emplace_front(value); }
  reference push_front(value_type &&value) {
    return emplace_front(std::move(value));
  }

  reference push_back(value_type const &value) { return emplace_back(value); }
  reference push_back(value_type &&value) {
    return emplace_back(std::move(value));
  }

  template <typename... UArgs>
  reference emplace_front(UArgs &&...args) {
    FATAL_ASSUME_LE(size_, queue_.size());
    if (size_ == queue_.size()) {
      grow();
    }
    FATAL_ASSUME_LT(size_, queue_.size());

    auto offset = offset_ > 0 ? offset_ - 1 : offset_ = queue_.size() - 1;
    FATAL_ASSUME_LT(offset, queue_.size());

    auto p = new (std::addressof(queue_[offset].value)) value_type(
      std::forward<UArgs>(args)...
    );

    offset_ = offset;
    ++size_;
    return *p;
  }

  void pop_front() {
    queue_[offset_].value.~value_type();

    if (++offset_ == queue_.size()) {
      offset_ = 0;
    }

    --size_;
  }

  template <typename... UArgs>
  reference emplace_back(UArgs &&...args) {
    FATAL_ASSUME_LE(size_, queue_.size());
    if (size_ == queue_.size()) {
      grow();
    }
    FATAL_ASSUME_LT(size_, queue_.size());

    auto p = new (std::addressof(queue_[append_index()].value)) value_type(
      std::forward<UArgs>(args)...
    );

    ++size_;
    return *p;
  }

  void pop_front(size_type count) {
    FATAL_ASSUME_LE(count, size_);

    auto chunk = std::min(count, queue_.size() - offset_);
    for (auto const end = offset_ + chunk; offset_ < end; ++offset_) {
      queue_[offset_].value.~value_type();
    }

    FATAL_ASSUME_LE(offset_, queue_.size());
    if (offset_ == queue_.size()) {
      offset_ = 0;
    }
    FATAL_ASSUME_LT(offset_, queue_.size());

    FATAL_ASSUME_LE(chunk, count);
    FATAL_ASSUME_EQ(offset_ == 0, count == chunk);
    for (auto const end = count - chunk; offset_ < end; ++offset_) {
      queue_[offset_].~value_type();
    }

    size_ -= count;
  }

  // shifts one element from the back to the front of the queue
  // `shifter` receives an rvalue reference to the element to be moved
  //  and the address to the uninitialized destination memory which
  //  must be initialized with a placement new
  // default shifter std::move()s the elements to the destination position
  template <typename Shifter = fn::placement_move>
  void shift_to_front(Shifter &&shifter = Shifter()) {
    assert(!queue_.empty());

    FATAL_ASSUME_POSITIVE(size_);
    auto const original = real_index(size_ - 1);
    FATAL_ASSUME_LT(original, queue_.size());

    if (offset_) {
      --offset_;
    } else {
      offset_ = queue_.size() - 1;
    }
    FATAL_ASSUME_LT(offset_, queue_.size());

    FATAL_ASSUME_LE(size_, queue_.size());
    if (size_ < queue_.size()) {
      auto &from = queue_[original].value;
      shifter(
        std::addressof(queue_[offset_].value),
        std::move(from)
      );
      from.~value_type();
    }
  }

  // shifts `count` element from the back to the front of the queue, preserving
  //  their relative order
  // `shifter` receives an rvalue reference to each element to be moved
  //  and the address to each correspondent uninitialized destination
  //  memory which must be initialized with a placement new
  // default shifter std::move()s the elements to the destination position
  template <typename Shifter = fn::placement_move>
  void shift_to_front_by(size_type count, Shifter &&shifter = Shifter()) {
    assert(!queue_.empty());

    FATAL_ASSUME_LE(count, size_);
    if (count == size_) {
      return;
    }
    FATAL_ASSUME_LT(count, size_);

    FATAL_ASSUME_LE(size_, queue_.size());
    if (size_ == queue_.size()) {
      // no empty slots, just need to adjust the offset
      if (offset_ < count) {
        FATAL_ASSUME_LT(count, queue_.size());
        offset_ += queue_.size() - count;
      } else {
        offset_ -= count;
      }
      FATAL_ASSUME_LT(offset_, queue_.size());
    } else {
      // empty slots, move front chunk to back
      while (count--) {
        if (offset_) {
          --offset_;
        } else {
          offset_ = queue_.size() - 1;
        }
        FATAL_ASSUME_LT(offset_, queue_.size());

        auto &from = queue_[loose_real_index(size_)].value;
        shifter(
          std::addressof(queue_[offset_].value),
          std::move(from)
        );
        from.~value_type();
      }
      FATAL_ASSUME_LT(offset_, queue_.size());
//*/
    }
  }

  // shifts one element from the front to the back of the queue
  // `shifter` receives an rvalue reference to the element to be moved
  //  and the address to the uninitialized destination memory which
  //  must be initialized with a placement new
  // default shifter std::move()s the elements to the destination position
  template <typename Shifter = fn::placement_move>
  void shift_to_back(Shifter &&shifter = Shifter()) {
    assert(!queue_.empty());

    auto const original = offset_;
    auto const destination = loose_real_index(size_);

    FATAL_ASSUME_LT(offset_, queue_.size());
    if (++offset_ == queue_.size()) {
      offset_ = 0;
    }
    FATAL_ASSUME_LT(offset_, queue_.size());

    FATAL_ASSUME_LE(size_, queue_.size());
    if (size_ < queue_.size()) {
      auto &from = queue_[original].value;
      shifter(
        std::addressof(queue_[destination].value),
        std::move(from)
      );
      from.~value_type();
    }
  }

  // shifts `count` element from the front to the back of the queue, preserving
  //  their relative order
  // `shifter` receives an rvalue reference to each element to be moved
  //  and the address to each correspondent uninitialized destination
  //  memory which must be initialized with a placement new
  // default shifter std::move()s the elements to the destination position
  template <typename Shifter = fn::placement_move>
  void shift_to_back_by(size_type count, Shifter &&shifter = Shifter()) {
    assert(!queue_.empty());

    FATAL_ASSUME_LE(count, size_);
    if (count == size_) {
      return;
    }
    FATAL_ASSUME_LT(count, size_);

    FATAL_ASSUME_LE(size_, queue_.size());
    if (size_ == queue_.size()) {
      // no empty slots, just need to adjust the offset
      FATAL_ASSUME_LT(offset_, queue_.size());
      auto const gap = queue_.size() - offset_;

      if (count < gap) {
        offset_ += count;
      } else {
        offset_ = count - gap;
      }

      FATAL_ASSUME_LT(offset_, queue_.size());
    } else {
      // empty slots, move back chunk to front
      FATAL_ASSUME_LT(size_, queue_.size());

      while (count--) {
        FATAL_ASSUME_LT(offset_, queue_.size());
        auto &from = queue_[offset_].value;
        shifter(
          std::addressof(queue_[append_index()].value),
          std::move(from)
        );
        from.~value_type();

        if (++offset_ == queue_.size()) {
          offset_ = 0;
        }
        FATAL_ASSUME_LT(offset_, queue_.size());
      }
    }
  }

  const_reference front() const {
    assert(!empty());
    return queue_[offset_].value;
  }
  reference front() {
    assert(!empty());
    return queue_[offset_].value;
  }

  const_reference back() const {
    FATAL_ASSUME_POSITIVE(size_);
    return queue_[real_index(size_ - 1)].value;
  }

  reference back() {
    FATAL_ASSUME_POSITIVE(size_);
    return queue_[real_index(size_ - 1)].value;
  }

  const_reference operator [](fatal::fast_pass<size_type> index) const {
    return queue_[real_index(index)].value;
  }

  reference operator [](fatal::fast_pass<size_type> index) {
    return queue_[real_index(index)].value;
  }

  void clear() noexcept {
    for (auto chunk: chunks()) {
      for(auto i = chunk.first; i < chunk.second; ++i) {
        static_assert(
          std::is_nothrow_destructible<value_type>::value,
          "value_type must provide a noexcept destructor"
        );
        queue_[i].value.~value_type();
      }
    }

    offset_ = 0;
    size_ = 0;

    static_assert(
      noexcept(queue_.clear()),
      "underlying container must provide a noexcept clear()"
    );
    queue_.clear();
  }

  fatal::fast_pass<size_type> size() const noexcept { return size_; }

  bool empty() const noexcept {
    static_assert(
      noexcept(queue_.empty()),
      "underlying container must provide a noexcept empty()"
    );
    return queue_.empty();
  }

  using const_iterator = random_access_iterator<circular_queue, true>;
  using iterator = random_access_iterator<circular_queue, false>;

  const_iterator cbegin() const { return const_iterator(this, 0); }
  const_iterator cend() const { return const_iterator(this, size_); }

  const_iterator begin() const { return cbegin(); }
  const_iterator end() const { return cend(); }

  iterator begin() { return iterator(this, 0); }
  iterator end() { return iterator(this, size_); }

  // TODO: OVERLOAD FOR ANY CONTAINER
  bool operator ==(circular_queue const &rhs) const {
    if (size_ != rhs.size_) {
      return false;
    }

    auto r = rhs.cbegin();
    auto const rend = rhs.cend();
    for (auto const &l: *this) {
      if (r == rend || l != *r) {
        return false;
      }
      ++r;
    }

    return r == rend;
  }

  // TODO: OVERLOAD FOR ANY CONTAINER
  bool operator !=(circular_queue const &rhs) const {
    return !(*this == rhs);
  }

private:
  queue_type queue_;
  size_type offset_ = 0;
  size_type size_ = 0;
};

} // namespace fatal {

FATAL_DIAGNOSTIC_POP
