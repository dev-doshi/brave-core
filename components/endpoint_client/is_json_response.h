/* Copyright (c) 2026 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_ENDPOINT_CLIENT_IS_JSON_RESPONSE_H_
#define BRAVE_COMPONENTS_ENDPOINT_CLIENT_IS_JSON_RESPONSE_H_

#include "brave/components/endpoint_client/is_response_body.h"
#include "brave/components/endpoint_client/response.h"

namespace endpoint_client::detail {

// Primary template: a type does not satisfy IsJSONResponse unless
// matched by the partial specialization below.
template <typename>
inline constexpr bool kIsJSONResponse = false;

// Partial specialization: Response<T, E> satisfies IsJSONResponse if
// T and E satisfy IsJSONResponseBody.
template <IsJSONResponseBody T, IsJSONResponseBody E>
inline constexpr bool kIsJSONResponse<Response<T, E>> = true;

// Concept: a type satisfies IsJSONResponse if
// its kIsJSONResponse specialization evaluates to true.
template <typename T>
concept IsJSONResponse = kIsJSONResponse<T>;

}  // namespace endpoint_client::detail

#endif  // BRAVE_COMPONENTS_ENDPOINT_CLIENT_IS_JSON_RESPONSE_H_
