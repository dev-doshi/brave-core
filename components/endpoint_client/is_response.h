/* Copyright (c) 2025 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_ENDPOINT_CLIENT_IS_RESPONSE_H_
#define BRAVE_COMPONENTS_ENDPOINT_CLIENT_IS_RESPONSE_H_

#include "brave/components/endpoint_client/is_response_body.h"
#include "brave/components/endpoint_client/response.h"

namespace endpoint_client::detail {

template <typename...>
inline constexpr bool kIsResponse = false;

template <typename BodyType,
          IsResponseBodyOf<BodyType> T,
          IsResponseBodyOf<BodyType> E>
inline constexpr bool kIsResponse<BodyType, Response<T, E>> = true;

template <typename T>
inline constexpr bool kIsResponse<T> =
    kIsResponse<JSON, T> || kIsResponse<Protobuf, T>;

template <typename... Ts>
concept IsResponse = kIsResponse<Ts...>;

}  // namespace endpoint_client::detail

#endif  // BRAVE_COMPONENTS_ENDPOINT_CLIENT_IS_RESPONSE_H_
