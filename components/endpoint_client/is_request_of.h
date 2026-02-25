/* Copyright (c) 2026 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_ENDPOINT_CLIENT_IS_REQUEST_OF_H_
#define BRAVE_COMPONENTS_ENDPOINT_CLIENT_IS_REQUEST_OF_H_

#include "brave/components/endpoint_client/is_request_body.h"
#include "brave/components/endpoint_client/request.h"

namespace endpoint_client::detail {

template <typename, typename>
inline constexpr bool kIsRequestOf = false;

template <typename BodyType, IsRequestBodyOf<BodyType> T, Method M>
inline constexpr bool kIsRequestOf<BodyType, Request<T, M>> = true;

template <typename T, typename BodyType>
concept IsRequestOf = kIsRequestOf<BodyType, T>;

}  // namespace endpoint_client::detail

#endif  // BRAVE_COMPONENTS_ENDPOINT_CLIENT_IS_REQUEST_OF_H_
