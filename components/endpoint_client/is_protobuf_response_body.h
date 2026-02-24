/* Copyright (c) 2026 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_ENDPOINT_CLIENT_IS_PROTOBUF_RESPONSE_BODY_H_
#define BRAVE_COMPONENTS_ENDPOINT_CLIENT_IS_PROTOBUF_RESPONSE_BODY_H_

#include <concepts>
#include <string_view>
#include <type_traits>

namespace endpoint_client::detail {

template <typename T>
concept IsProtobufResponseBody =
    requires(T t, std::string_view data) {
      { t.ParseFromString(data) } -> std::same_as<bool>;
    } &&
    std::is_member_function_pointer_v<
        decltype(static_cast<bool (T::*)(std::string_view)>(
            &T::ParseFromString))>;

}  // namespace endpoint_client::detail

#endif  // BRAVE_COMPONENTS_ENDPOINT_CLIENT_IS_PROTOBUF_RESPONSE_BODY_H_
