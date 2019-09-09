/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/container/circular_queue.h>

#include <fatal/test/driver.h>

#include <vector>
#include <unordered_map>

namespace fatal {

// TODO: properly implement tests

# define CHECK_CONTENTS(...) \
  do { \
    std::vector<int> const expected{__VA_ARGS__}; \
    FATAL_EXPECT_EQ(expected.size(), q.size()); \
    \
    for (std::size_t i = 0; i < expected.size(); ++i) { \
      FATAL_EXPECT_EQ(expected[i], q[i]); \
    } \
  } while (false)

FATAL_TEST(circular_queue, ctor_default) {
  circular_queue<int> q;
  CHECK_CONTENTS();
}

FATAL_TEST(circular_queue, ctor_capacity) {
  std::size_t size = 10;

  for (std::size_t capacity = 0; capacity <= size; ++capacity) {
    circular_queue<int> q(capacity);
    CHECK_CONTENTS();
  }
}

FATAL_TEST(circular_queue, push_front) {
  std::size_t size = 10;

  for (std::size_t capacity = 0; capacity <= size; ++capacity) {
    circular_queue<int> q(capacity);
    CHECK_CONTENTS();

    q.push_front(9);
    CHECK_CONTENTS(9);

    q.push_front(8);
    CHECK_CONTENTS(8, 9);

    q.push_front(7);
    CHECK_CONTENTS(7, 8, 9);

    q.push_front(6);
    CHECK_CONTENTS(6, 7, 8, 9);

    q.push_front(5);
    CHECK_CONTENTS(5, 6, 7, 8, 9);

    q.push_front(4);
    CHECK_CONTENTS(4, 5, 6, 7, 8, 9);

    q.push_front(3);
    CHECK_CONTENTS(3, 4, 5, 6, 7, 8, 9);

    q.push_front(2);
    CHECK_CONTENTS(2, 3, 4, 5, 6, 7, 8, 9);

    q.push_front(1);
    CHECK_CONTENTS(1, 2, 3, 4, 5, 6, 7, 8, 9);

    q.push_front(0);
    CHECK_CONTENTS(0, 1, 2, 3, 4, 5, 6, 7, 8, 9);
  }
}

FATAL_TEST(circular_queue, push_bacl) {
  std::size_t size = 10;

  for (std::size_t capacity = 0; capacity <= size; ++capacity) {
    circular_queue<int> q(capacity);
    CHECK_CONTENTS();

    q.push_back(0);
    CHECK_CONTENTS(0);

    q.push_back(1);
    CHECK_CONTENTS(0, 1);

    q.push_back(2);
    CHECK_CONTENTS(0, 1, 2);

    q.push_back(3);
    CHECK_CONTENTS(0, 1, 2, 3);

    q.push_back(4);
    CHECK_CONTENTS(0, 1, 2, 3, 4);

    q.push_back(5);
    CHECK_CONTENTS(0, 1, 2, 3, 4, 5);

    q.push_back(6);
    CHECK_CONTENTS(0, 1, 2, 3, 4, 5, 6);

    q.push_back(7);
    CHECK_CONTENTS(0, 1, 2, 3, 4, 5, 6, 7);

    q.push_back(8);
    CHECK_CONTENTS(0, 1, 2, 3, 4, 5, 6, 7, 8);

    q.push_back(9);
    CHECK_CONTENTS(0, 1, 2, 3, 4, 5, 6, 7, 8, 9);
  }
}

FATAL_TEST(circular_queue, shift_to_front) {
  std::size_t size = 10;

  for (std::size_t capacity = 0; capacity <= size; ++capacity) {
    circular_queue<int> q(capacity);
    for (int i = int(size); i--; ) {
      q.push_front(i);
    }
    CHECK_CONTENTS(0, 1, 2, 3, 4, 5, 6, 7, 8, 9);

    q.shift_to_front();
    CHECK_CONTENTS(9, 0, 1, 2, 3, 4, 5, 6, 7, 8);

    q.shift_to_front();
    CHECK_CONTENTS(8, 9, 0, 1, 2, 3, 4, 5, 6, 7);

    q.shift_to_front();
    CHECK_CONTENTS(7, 8, 9, 0, 1, 2, 3, 4, 5, 6);

    q.shift_to_front();
    CHECK_CONTENTS(6, 7, 8, 9, 0, 1, 2, 3, 4, 5);

    q.shift_to_front();
    CHECK_CONTENTS(5, 6, 7, 8, 9, 0, 1, 2, 3, 4);

    q.shift_to_front();
    CHECK_CONTENTS(4, 5, 6, 7, 8, 9, 0, 1, 2, 3);

    q.shift_to_front();
    CHECK_CONTENTS(3, 4, 5, 6, 7, 8, 9, 0, 1, 2);

    q.shift_to_front();
    CHECK_CONTENTS(2, 3, 4, 5, 6, 7, 8, 9, 0, 1);

    q.shift_to_front();
    CHECK_CONTENTS(1, 2, 3, 4, 5, 6, 7, 8, 9, 0);

    q.shift_to_front();
    CHECK_CONTENTS(0, 1, 2, 3, 4, 5, 6, 7, 8, 9);
  }
}

FATAL_TEST(circular_queue, shift_to_front_by) {
  std::size_t size = 10;

  for (std::size_t capacity = 0; capacity <= size; ++capacity) {
    circular_queue<int> q(capacity);
    for (int i = int(size); i--; ) {
      q.push_front(i);
    }
    CHECK_CONTENTS(0, 1, 2, 3, 4, 5, 6, 7, 8, 9);

    q.shift_to_front_by(0);
    CHECK_CONTENTS(0, 1, 2, 3, 4, 5, 6, 7, 8, 9);

    q.shift_to_front_by(q.size());
    CHECK_CONTENTS(0, 1, 2, 3, 4, 5, 6, 7, 8, 9);

    q.shift_to_front_by(1);
    CHECK_CONTENTS(9, 0, 1, 2, 3, 4, 5, 6, 7, 8);

    q.shift_to_front_by(2);
    CHECK_CONTENTS(7, 8, 9, 0, 1, 2, 3, 4, 5, 6);

    q.shift_to_front_by(3);
    CHECK_CONTENTS(4, 5, 6, 7, 8, 9, 0, 1, 2, 3);

    q.shift_to_front_by(4);
    CHECK_CONTENTS(0, 1, 2, 3, 4, 5, 6, 7, 8, 9);

    q.shift_to_front_by(5);
    CHECK_CONTENTS(5, 6, 7, 8, 9, 0, 1, 2, 3, 4);

    q.shift_to_front_by(6);
    CHECK_CONTENTS(9, 0, 1, 2, 3, 4, 5, 6, 7, 8);

    q.shift_to_front_by(7);
    CHECK_CONTENTS(2, 3, 4, 5, 6, 7, 8, 9, 0, 1);

    q.shift_to_front_by(8);
    CHECK_CONTENTS(4, 5, 6, 7, 8, 9, 0, 1, 2, 3);

    q.shift_to_front_by(9);
    CHECK_CONTENTS(5, 6, 7, 8, 9, 0, 1, 2, 3, 4);

    q.shift_to_front_by(0);
    CHECK_CONTENTS(5, 6, 7, 8, 9, 0, 1, 2, 3, 4);

    q.shift_to_front_by(q.size());
    CHECK_CONTENTS(5, 6, 7, 8, 9, 0, 1, 2, 3, 4);
  }
}

FATAL_TEST(circular_queue, shift_to_back) {
  std::size_t size = 10;

  for (std::size_t capacity = 0; capacity <= size; ++capacity) {
    circular_queue<int> q(capacity);
    for (int i = int(size); i--; ) {
      q.push_front(i);
    }
    CHECK_CONTENTS(0, 1, 2, 3, 4, 5, 6, 7, 8, 9);

    q.shift_to_back();
    CHECK_CONTENTS(1, 2, 3, 4, 5, 6, 7, 8, 9, 0);

    q.shift_to_back();
    CHECK_CONTENTS(2, 3, 4, 5, 6, 7, 8, 9, 0, 1);

    q.shift_to_back();
    CHECK_CONTENTS(3, 4, 5, 6, 7, 8, 9, 0, 1, 2);

    q.shift_to_back();
    CHECK_CONTENTS(4, 5, 6, 7, 8, 9, 0, 1, 2, 3);

    q.shift_to_back();
    CHECK_CONTENTS(5, 6, 7, 8, 9, 0, 1, 2, 3, 4);

    q.shift_to_back();
    CHECK_CONTENTS(6, 7, 8, 9, 0, 1, 2, 3, 4, 5);

    q.shift_to_back();
    CHECK_CONTENTS(7, 8, 9, 0, 1, 2, 3, 4, 5, 6);

    q.shift_to_back();
    CHECK_CONTENTS(8, 9, 0, 1, 2, 3, 4, 5, 6, 7);

    q.shift_to_back();
    CHECK_CONTENTS(9, 0, 1, 2, 3, 4, 5, 6, 7, 8);

    q.shift_to_back();
    CHECK_CONTENTS(0, 1, 2, 3, 4, 5, 6, 7, 8, 9);
  }
}

FATAL_TEST(circular_queue, shift_to_back_by) {
  std::size_t size = 10;

  for (std::size_t capacity = 0; capacity <= size; ++capacity) {
    circular_queue<int> q(capacity);
    for (int i = int(size); i--; ) {
      q.push_front(i);
    }
    CHECK_CONTENTS(0, 1, 2, 3, 4, 5, 6, 7, 8, 9);

    q.shift_to_back_by(0);
    CHECK_CONTENTS(0, 1, 2, 3, 4, 5, 6, 7, 8, 9);

    q.shift_to_back_by(q.size());
    CHECK_CONTENTS(0, 1, 2, 3, 4, 5, 6, 7, 8, 9);

    q.shift_to_back_by(1);
    CHECK_CONTENTS(1, 2, 3, 4, 5, 6, 7, 8, 9, 0);

    q.shift_to_back_by(2);
    CHECK_CONTENTS(3, 4, 5, 6, 7, 8, 9, 0, 1, 2);

    q.shift_to_back_by(3);
    CHECK_CONTENTS(6, 7, 8, 9, 0, 1, 2, 3, 4, 5);

    q.shift_to_back_by(4);
    CHECK_CONTENTS(0, 1, 2, 3, 4, 5, 6, 7, 8, 9);

    q.shift_to_back_by(5);
    CHECK_CONTENTS(5, 6, 7, 8, 9, 0, 1, 2, 3, 4);

    q.shift_to_back_by(6);
    CHECK_CONTENTS(1, 2, 3, 4, 5, 6, 7, 8, 9, 0);

    q.shift_to_back_by(7);
    CHECK_CONTENTS(8, 9, 0, 1, 2, 3, 4, 5, 6, 7);

    q.shift_to_back_by(8);
    CHECK_CONTENTS(6, 7, 8, 9, 0, 1, 2, 3, 4, 5);

    q.shift_to_back_by(9);
    CHECK_CONTENTS(5, 6, 7, 8, 9, 0, 1, 2, 3, 4);

    q.shift_to_back_by(0);
    CHECK_CONTENTS(5, 6, 7, 8, 9, 0, 1, 2, 3, 4);

    q.shift_to_back_by(q.size());
    CHECK_CONTENTS(5, 6, 7, 8, 9, 0, 1, 2, 3, 4);
  }
}

# undef CHECK_CONTENTS

template <typename Data, typename Factory>
struct getter {
  getter(Data &data, Factory &factory):
    data_(data),
    factory_(factory)
  {}

  using type = typename Data::mapped_type const &;

  type operator()(std::size_t key) {
    auto i = data_.find(key);
    if (i == data_.end()) {
      auto added = data_.emplace(key, factory_(key));
      FATAL_ASSERT_TRUE(added.second);
      i = added.first;
    }

    return i->second;
  }

private:
  Data &data_;
  Factory &factory_;
};

template <typename Getter>
struct creator {
  explicit creator(Getter &get): get_(get) {}

  typename Getter::type operator ()() {
    return get_(ith++);
  }

  std::size_t ith = 0;

private:
  Getter &get_;
};

template <typename Subject, typename Factory>
void check_circular_queue(Factory &&factory) {
  using subject_type = Subject;
  using capacity = std::integral_constant<std::size_t, 10>;
  circular_queue<subject_type> q(capacity::value);

  using map = std::unordered_map<std::size_t, subject_type>;
  map data(capacity::value);

  getter<map, Factory> get(data, factory);
  creator<decltype(get)> create(get);

  q.push_back(create());
  q.emplace_back(create());
  {
    auto x = create();
    q.push_back(x);
  }
  {
    auto x = create();
    q.emplace_back(x);
  }
  {
    auto x = create();
    q.push_back(std::move(x));
  }
  {
    auto x = create();
    q.emplace_back(std::move(x));
  }

  FATAL_EXPECT_FALSE(q.empty());

  FATAL_EXPECT_EQ(6, create.ith);
  FATAL_EXPECT_EQ(create.ith, q.size());
  FATAL_EXPECT_EQ(q.front(), get(0));
  FATAL_EXPECT_EQ(q.back(), get(5));

  q.pop_front();

  FATAL_EXPECT_EQ(5, q.size());
  FATAL_EXPECT_EQ(q.front(), get(1));
  FATAL_EXPECT_EQ(q.back(), get(5));

  q.clear();

  FATAL_EXPECT_TRUE(q.empty());
}

FATAL_TEST(circular_queue, regression_long) {
  using subject_type = long;
  check_circular_queue<subject_type>(
    [](std::size_t i) { return static_cast<subject_type>(i); }
  );
}

FATAL_TEST(circular_queue, regression_test_holder_long) {
  using subject_type = std::tuple<long>;
  check_circular_queue<subject_type>(
    [](std::size_t i) { return subject_type(static_cast<long>(i)); }
  );
}

} // namespace fatal {
