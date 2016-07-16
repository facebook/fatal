/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/type/registry.h>

#include <fatal/test/driver.h>

namespace fatal {

struct tag1 {};
struct tag2 {};
struct tag3 {};

struct tagO0 {};

struct key1 {};
struct key2 {};
struct key3 {};
struct key4 {};

struct keyO0 {};
struct keyO1 {};
struct keyO2 {};
struct keyO3 {};

struct m1 {};
struct m2 {};
struct m3 {};
struct m4 {};
struct m6 {};

struct mO0 {};
struct mO1 {};
struct mO2 {};
struct mO3 {};
struct mO4 {};

struct nf {};

} // namespace fatal {

namespace other {

struct tag0 {};

struct key0 {};
struct key1 {};
struct key2 {};
struct key3 {};

struct m0 {};
struct m1 {};
struct m2 {};

FATAL_REGISTER_TYPE(other::tag0, other::key0, other::m0);
FATAL_REGISTER_TYPE(other::tag0, other::key1, fatal::mO0);

FATAL_REGISTER_TYPE(other::tag0, fatal::keyO0, other::m1);
FATAL_REGISTER_TYPE(other::tag0, fatal::keyO2, fatal::mO1);
FATAL_REGISTER_TYPE(fatal::tagO0, other::key2, fatal::mO3);

} // namespace other {

namespace fatal {

FATAL_REGISTER_TYPE(tag1, key1, m1);
FATAL_REGISTER_TYPE(tag1, key2, m2);
FATAL_REGISTER_TYPE(tag1, key3, m3);

FATAL_REGISTER_TYPE(tag2, key1, m4);
FATAL_REGISTER_TYPE(tag2, key3, m6);

FATAL_REGISTER_TYPE(other::tag0, keyO1, other::m2);
FATAL_REGISTER_TYPE(other::tag0, keyO3, mO2);
FATAL_REGISTER_TYPE(tagO0, other::key3, mO4);

FATAL_TEST(registry, try_registry_lookup) {
  FATAL_EXPECT_SAME<m1, try_registry_lookup<tag1, key1, nf>>();
  FATAL_EXPECT_SAME<m2, try_registry_lookup<tag1, key2, nf>>();
  FATAL_EXPECT_SAME<m3, try_registry_lookup<tag1, key3, nf>>();
  FATAL_EXPECT_SAME<nf, try_registry_lookup<tag1, key4, nf>>();

  FATAL_EXPECT_SAME<m4, try_registry_lookup<tag2, key1, nf>>();
  FATAL_EXPECT_SAME<nf, try_registry_lookup<tag2, key2, nf>>();
  FATAL_EXPECT_SAME<m6, try_registry_lookup<tag2, key3, nf>>();
  FATAL_EXPECT_SAME<nf, try_registry_lookup<tag2, key4, nf>>();

  FATAL_EXPECT_SAME<nf, try_registry_lookup<tag3, key1, nf>>();
  FATAL_EXPECT_SAME<nf, try_registry_lookup<tag3, key2, nf>>();
  FATAL_EXPECT_SAME<nf, try_registry_lookup<tag3, key3, nf>>();
  FATAL_EXPECT_SAME<nf, try_registry_lookup<tag3, key4, nf>>();

  FATAL_EXPECT_SAME<
    other::m0,
    try_registry_lookup<other::tag0, other::key0, nf>
  >();
  FATAL_EXPECT_SAME<mO0, try_registry_lookup<other::tag0, other::key1, nf>>();
  FATAL_EXPECT_SAME<nf, try_registry_lookup<other::tag0, other::key2, nf>>();
  FATAL_EXPECT_SAME<nf, try_registry_lookup<other::tag0, other::key3, nf>>();

  FATAL_EXPECT_SAME<other::m1, try_registry_lookup<other::tag0, keyO0, nf>>();
  FATAL_EXPECT_SAME<other::m2, try_registry_lookup<other::tag0, keyO1, nf>>();
  FATAL_EXPECT_SAME<mO1,  try_registry_lookup<other::tag0, keyO2, nf>>();
  FATAL_EXPECT_SAME<mO2,  try_registry_lookup<other::tag0, keyO3, nf>>();

  FATAL_EXPECT_SAME<nf, try_registry_lookup<other::tag0, key1, nf>>();
  FATAL_EXPECT_SAME<nf, try_registry_lookup<other::tag0, key2, nf>>();
  FATAL_EXPECT_SAME<nf, try_registry_lookup<other::tag0, key3, nf>>();

  FATAL_EXPECT_SAME<nf,  try_registry_lookup<tagO0, other::key0, nf>>();
  FATAL_EXPECT_SAME<nf,  try_registry_lookup<tagO0, other::key1, nf>>();
  FATAL_EXPECT_SAME<mO3,  try_registry_lookup<tagO0, other::key2, nf>>();
  FATAL_EXPECT_SAME<mO4,  try_registry_lookup<tagO0, other::key3, nf>>();
}

FATAL_TEST(registry, registry_lookup) {
  FATAL_EXPECT_SAME<m1, registry_lookup<tag1, key1>>();
  FATAL_EXPECT_SAME<m2, registry_lookup<tag1, key2>>();
  FATAL_EXPECT_SAME<m3, registry_lookup<tag1, key3>>();

  FATAL_EXPECT_SAME<m4, registry_lookup<tag2, key1>>();
  FATAL_EXPECT_SAME<m6, registry_lookup<tag2, key3>>();

  FATAL_EXPECT_SAME<other::m0, registry_lookup<other::tag0, other::key0>>();
  FATAL_EXPECT_SAME<mO0, registry_lookup<other::tag0, other::key1>>();
  FATAL_EXPECT_SAME<other::m1, registry_lookup<other::tag0, keyO0>>();
  FATAL_EXPECT_SAME<other::m2, registry_lookup<other::tag0, keyO1>>();
  FATAL_EXPECT_SAME<mO1,  registry_lookup<other::tag0, keyO2>>();
  FATAL_EXPECT_SAME<mO2,  registry_lookup<other::tag0, keyO3>>();
  FATAL_EXPECT_SAME<mO3,  registry_lookup<tagO0, other::key2>>();
  FATAL_EXPECT_SAME<mO4,  registry_lookup<tagO0, other::key3>>();
}

} // namespace fatal {
