/* Copyright (c) 2025 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_ENDPOINT_CLIENT_IS_REQUEST_BODY_H_
#define BRAVE_COMPONENTS_ENDPOINT_CLIENT_IS_REQUEST_BODY_H_

#include "brave/components/endpoint_client/json.h"
#include "brave/components/endpoint_client/protobuf.h"

namespace endpoint_client::detail {

// Primary template: a type does not satisfy IsRequestBody unless
// matched by one of the partial specializations below.
template <typename...>
inline constexpr bool kIsRequestBody = false;

// Partial specialization: a type T satisfies IsRequestBody
// for a given BodyType if BodyType declares T as a valid
// request body via BodyType::kIsRequestBody<T>.
template <typename T, typename BodyType>
inline constexpr bool kIsRequestBody<T, BodyType> =
    BodyType::template kIsRequestBody<T>;

// Partial specialization (single-argument form): a type T satisfies
// IsRequestBody if it models either a JSON request body
// or a Protobuf request body.
template <typename T>
inline constexpr bool kIsRequestBody<T> =
    kIsRequestBody<T, JSON> || kIsRequestBody<T, Protobuf>;

// Concept: a type satisfies IsRequestBody if
// its kIsRequestBody specialization evaluates to true.
template <typename... Ts>
concept IsRequestBody = kIsRequestBody<Ts...>;

}  // namespace endpoint_client::detail

#endif  // BRAVE_COMPONENTS_ENDPOINT_CLIENT_IS_REQUEST_BODY_H_
