/*
 *  Copyright (c) 2014, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#pragma once

namespace ftl {

#define FTL_CAT_IMPL(LHS, RHS) LHS##RHS
#define FTL_CAT(LHS, RHS) FTL_CAT_IMPL(LHS, RHS)

#define FTL_AS_STR_IMPL(String) #String
#define FTL_AS_STR(String) FTL_AS_STR_IMPL(String)

#define FTL_UID(Prefix) FTL_CAT(Prefix, FTL_CAT(_, __LINE__))

} // namespace ftl {
