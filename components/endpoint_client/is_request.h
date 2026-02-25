/* Copyright (c) 2025 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_ENDPOINT_CLIENT_IS_REQUEST_H_
#define BRAVE_COMPONENTS_ENDPOINT_CLIENT_IS_REQUEST_H_

#include "brave/components/endpoint_client/is_request_body.h"
#include "brave/components/endpoint_client/request.h"

namespace endpoint_client::detail {

template <typename...>
inline constexpr bool kIsRequest = false;

template <typename BodyType, IsRequestBody<BodyType> T, Method M>
inline constexpr bool kIsRequest<BodyType, Request<T, M>> = true;

template <typename T>
inline constexpr bool kIsRequest<T> =
    kIsRequest<JSON, T> || kIsRequest<Protobuf, T>;

template <typename... Ts>
concept IsRequest = kIsRequest<Ts...>;

}  // namespace endpoint_client::detail

#endif  // BRAVE_COMPONENTS_ENDPOINT_CLIENT_IS_REQUEST_H_
