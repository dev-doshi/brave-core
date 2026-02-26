/* Copyright (c) 2025 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_ENDPOINT_CLIENT_IS_RESPONSE_BODY_H_
#define BRAVE_COMPONENTS_ENDPOINT_CLIENT_IS_RESPONSE_BODY_H_

#include "brave/components/endpoint_client/json.h"
#include "brave/components/endpoint_client/protobuf.h"

namespace endpoint_client::detail {

// Primary template: a type does not satisfy IsResponseBody unless
// matched by one of the partial specializations below.
template <typename...>
inline constexpr bool kIsResponseBody = false;

// Partial specialization: a type T satisfies IsResponseBody
// for a given BodyType if BodyType declares T as a valid
// response body via BodyType::kIsResponseBody<T>.
template <typename T, typename BodyType>
inline constexpr bool kIsResponseBody<T, BodyType> =
    BodyType::template kIsResponseBody<T>;

// Partial specialization (single-argument form): a type T satisfies
// IsResponseBody if it models either a JSON response body
// or a Protobuf response body.
template <typename T>
inline constexpr bool kIsResponseBody<T> =
    kIsResponseBody<T, JSON> || kIsResponseBody<T, Protobuf>;

// Concept: a type satisfies IsResponseBody if
// its kIsResponseBody specialization evaluates to true.
template <typename... Ts>
concept IsResponseBody = kIsResponseBody<Ts...>;

}  // namespace endpoint_client::detail

#endif  // BRAVE_COMPONENTS_ENDPOINT_CLIENT_IS_RESPONSE_BODY_H_
