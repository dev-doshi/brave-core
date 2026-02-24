/* Copyright (c) 2025 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_ENDPOINT_CLIENT_IS_REQUEST_BODY_H_
#define BRAVE_COMPONENTS_ENDPOINT_CLIENT_IS_REQUEST_BODY_H_

#include "brave/components/endpoint_client/is_json_request_body.h"
#include "brave/components/endpoint_client/is_protobuf_request_body.h"

namespace endpoint_client::detail {

template <typename T>
concept IsRequestBody = IsJSONRequestBody<T> || IsProtobufRequestBody<T>;

}  // namespace endpoint_client::detail

#endif  // BRAVE_COMPONENTS_ENDPOINT_CLIENT_IS_REQUEST_BODY_H_
