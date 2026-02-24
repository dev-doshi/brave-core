/* Copyright (c) 2026 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_ENDPOINT_CLIENT_IS_PROTOBUF_REQUEST_BODY_H_
#define BRAVE_COMPONENTS_ENDPOINT_CLIENT_IS_PROTOBUF_REQUEST_BODY_H_

#include <concepts>
#include <string>
#include <type_traits>

namespace endpoint_client::detail {

template <typename T>
concept IsProtobufRequestBody =
    requires(const T t) {
      { t.ByteSizeLong() } -> std::same_as<std::size_t>;
      { t.SerializeAsString() } -> std::same_as<std::string>;
    } && std::is_member_function_pointer_v<decltype(&T::ByteSizeLong)> &&
    std::is_member_function_pointer_v<decltype(&T::SerializeAsString)>;

}  // namespace endpoint_client::detail

#endif  // BRAVE_COMPONENTS_ENDPOINT_CLIENT_IS_PROTOBUF_REQUEST_BODY_H_
