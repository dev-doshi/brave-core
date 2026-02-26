/* Copyright (c) 2025 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_ENDPOINT_CLIENT_IS_RESPONSE_H_
#define BRAVE_COMPONENTS_ENDPOINT_CLIENT_IS_RESPONSE_H_

#include "brave/components/endpoint_client/is_response_body.h"
#include "brave/components/endpoint_client/response.h"

namespace endpoint_client::detail {

// Primary template: a type does not satisfy IsResponse unless
// matched by one of the partial specializations below.
template <typename...>
inline constexpr bool kIsResponse = false;

// Partial specialization: Response<T, E> satisfies IsResponse
// for a given BodyType if both T and E model valid response bodies
// for that BodyType.
template <typename T, typename E, typename BodyType>
  requires(IsResponseBody<T, BodyType> && IsResponseBody<E, BodyType>)
inline constexpr bool kIsResponse<Response<T, E>, BodyType> = true;

// Partial specialization (single-argument form): a type T satisfies
// IsResponse if it models either a JSON response
// or a Protobuf response.
template <typename T>
inline constexpr bool kIsResponse<T> =
    kIsResponse<T, JSON> || kIsResponse<T, Protobuf>;

// Concept: a type satisfies IsResponse if
// its kIsResponse specialization evaluates to true.
template <typename... Ts>
concept IsResponse = kIsResponse<Ts...>;

}  // namespace endpoint_client::detail

#endif  // BRAVE_COMPONENTS_ENDPOINT_CLIENT_IS_RESPONSE_H_
