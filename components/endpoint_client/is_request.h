/* Copyright (c) 2025 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_ENDPOINT_CLIENT_IS_REQUEST_H_
#define BRAVE_COMPONENTS_ENDPOINT_CLIENT_IS_REQUEST_H_

#include "brave/components/endpoint_client/is_request_body.h"
#include "brave/components/endpoint_client/request.h"

namespace endpoint_client::detail {

// Primary template: a type does not satisfy IsRequest unless
// matched by one of the partial specializations below.
template <typename...>
inline constexpr bool kIsRequest = false;

// Partial specialization: Request<T, M> satisfies IsRequest
// for a given BodyType if T models a valid request body
// for that BodyType.
template <typename T, Method M, typename BodyType>
  requires IsRequestBody<T, BodyType>
inline constexpr bool kIsRequest<Request<T, M>, BodyType> = true;

// Partial specialization (single-argument form): a type T satisfies IsRequest
// if it models either a JSON-backed or Protobuf-backed request.
template <typename T>
inline constexpr bool kIsRequest<T> =
    kIsRequest<T, JSON> || kIsRequest<T, Protobuf>;

// Concept: a type satisfies IsRequest if
// its kIsRequest specialization evaluates to true.
template <typename... Ts>
concept IsRequest = kIsRequest<Ts...>;

}  // namespace endpoint_client::detail

#endif  // BRAVE_COMPONENTS_ENDPOINT_CLIENT_IS_REQUEST_H_
