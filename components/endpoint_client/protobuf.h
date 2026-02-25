/* Copyright (c) 2026 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_ENDPOINT_CLIENT_PROTOBUF_H_
#define BRAVE_COMPONENTS_ENDPOINT_CLIENT_PROTOBUF_H_

#include <concepts>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>

namespace endpoint_client::detail {

struct Protobuf {
  template <typename T>
  static constexpr bool kIsRequestBody = requires(const T t) {
    { t.ByteSizeLong() } -> std::same_as<std::size_t>;
    { t.SerializeAsString() } -> std::same_as<std::string>;
    requires std::is_member_function_pointer_v<decltype(&T::ByteSizeLong)>;
    requires std::is_member_function_pointer_v<decltype(&T::SerializeAsString)>;
  };

  template <typename T>
  static constexpr bool kIsResponseBody = requires(T t, std::string_view data) {
    { t.ParseFromString(data) } -> std::same_as<bool>;
    requires std::is_member_function_pointer_v<
        decltype(static_cast<bool (T::*)(std::string_view)>(
            &T::ParseFromString))>;
  };

  template <typename RequestBody>
    requires kIsRequestBody<RequestBody>
  static std::optional<std::string> Serialize(const RequestBody& request_body) {
    return request_body.ByteSizeLong() ? request_body.SerializeAsString()
                                       : std::optional<std::string>();
  }
};

}  // namespace endpoint_client::detail

#endif  // BRAVE_COMPONENTS_ENDPOINT_CLIENT_PROTOBUF_H_
