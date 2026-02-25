/* Copyright (c) 2025 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_ENDPOINT_CLIENT_IS_REQUEST_BODY_H_
#define BRAVE_COMPONENTS_ENDPOINT_CLIENT_IS_REQUEST_BODY_H_

#include "brave/components/endpoint_client/json.h"
#include "brave/components/endpoint_client/protobuf.h"

namespace endpoint_client::detail {

template <typename...>
inline constexpr bool kIsRequestBody = false;

template <typename T, typename BodyType>
inline constexpr bool kIsRequestBody<T, BodyType> =
    BodyType::template kIsRequestBody<T>;

template <typename T>
inline constexpr bool kIsRequestBody<T> =
    kIsRequestBody<T, JSON> || kIsRequestBody<T, Protobuf>;

template <typename... Ts>
concept IsRequestBody = kIsRequestBody<Ts...>;

}  // namespace endpoint_client::detail

#endif  // BRAVE_COMPONENTS_ENDPOINT_CLIENT_IS_REQUEST_BODY_H_
