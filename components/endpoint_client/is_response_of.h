/* Copyright (c) 2026 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_ENDPOINT_CLIENT_IS_RESPONSE_OF_H_
#define BRAVE_COMPONENTS_ENDPOINT_CLIENT_IS_RESPONSE_OF_H_

#include "brave/components/endpoint_client/is_response_body.h"
#include "brave/components/endpoint_client/response.h"

namespace endpoint_client::detail {

template <typename, typename>
inline constexpr bool kIsResponseOf = false;

template <typename BodyType,
          IsResponseBodyOf<BodyType> T,
          IsResponseBodyOf<BodyType> E>
inline constexpr bool kIsResponseOf<BodyType, Response<T, E>> = true;

template <typename T, typename BodyType>
concept IsResponseOf = kIsResponseOf<BodyType, T>;

}  // namespace endpoint_client::detail

#endif  // BRAVE_COMPONENTS_ENDPOINT_CLIENT_IS_RESPONSE_OF_H_
