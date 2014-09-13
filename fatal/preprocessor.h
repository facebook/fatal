/*
 *  Copyright (c) 2014, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#pragma once

namespace fatal {

#define FATAL_CAT_IMPL(LHS, RHS) LHS##RHS
#define FATAL_CAT(LHS, RHS) FATAL_CAT_IMPL(LHS, RHS)

#define FATAL_AS_STR_IMPL(String) #String
#define FATAL_AS_STR(String) FATAL_AS_STR_IMPL(String)

#define FATAL_UID(Prefix) FATAL_CAT(Prefix, FATAL_CAT(_, __LINE__))

} // namespace fatal {
