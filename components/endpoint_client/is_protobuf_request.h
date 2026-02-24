/* Copyright (c) 2026 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_ENDPOINT_CLIENT_IS_PROTOBUF_REQUEST_H_
#define BRAVE_COMPONENTS_ENDPOINT_CLIENT_IS_PROTOBUF_REQUEST_H_

#include "brave/components/endpoint_client/is_request_body.h"
#include "brave/components/endpoint_client/request.h"

namespace endpoint_client::detail {

// Primary template: a type does not satisfy IsProtobufRequest unless
// matched by the partial specialization below.
template <typename>
inline constexpr bool kIsProtobufRequest = false;

// Partial specialization: Request<T, M> satisfies IsProtobufRequest if
// T satisfies IsProtobufRequestBody.
template <IsProtobufRequestBody T, Method M>
inline constexpr bool kIsProtobufRequest<Request<T, M>> = true;

// Concept: a type satisfies IsProtobufRequest if
// its kIsProtobufRequest specialization evaluates to true.
template <typename T>
concept IsProtobufRequest = kIsProtobufRequest<T>;

}  // namespace endpoint_client::detail

#endif  // BRAVE_COMPONENTS_ENDPOINT_CLIENT_IS_PROTOBUF_REQUEST_H_
