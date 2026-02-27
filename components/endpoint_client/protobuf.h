/* Copyright (c) 2026 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_ENDPOINT_CLIENT_PROTOBUF_H_
#define BRAVE_COMPONENTS_ENDPOINT_CLIENT_PROTOBUF_H_

#include <concepts>
#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>

#include "base/types/expected.h"

namespace endpoint_client::detail {

struct Protobuf {
  // Checks whether `T` defines non-static, accessible member
  // functions `ByteSizeLong()` and `SerializeAsString()` such that:
  //   - `t.ByteSizeLong()` is a valid expression,
  //      and that call yields `std::size_t`
  //   - `&T::ByteSizeLong` is a valid member function pointer (ensures it's a
  //      non-static member function)
  //   - `t.SerializeAsString()` is a valid expression,
  //      and that call yields `std::string`
  //   - `&T::SerializeAsString` is a valid member function pointer (ensures
  //      it's a non-static member function)
  //
  // In short: models any type that can report its serialized size and serialize
  // itself into a `std::string`.
  template <typename T>
  static constexpr bool kIsRequestBody = requires(const T t) {
    { t.ByteSizeLong() } -> std::same_as<std::size_t>;
    requires std::is_member_function_pointer_v<decltype(&T::ByteSizeLong)>;
    { t.SerializeAsString() } -> std::same_as<std::string>;
    requires std::is_member_function_pointer_v<decltype(&T::SerializeAsString)>;
  };

  // Checks whether `T` defines a non-static, accessible member
  // function `ParseFromString(std::string_view)` such that:
  //   - `t.ParseFromString(data)` is a valid expression,
  //      and that call yields `bool`
  //   - `&T::ParseFromString` is a valid member function pointer (ensures it's
  //     a non-static member function; disambiguates overloads)
  //
  // In short: models any type that can parse itself from a `std::string_view`.
  template <typename T>
  static constexpr bool kIsResponseBody = requires(T t, std::string_view data) {
    { t.ParseFromString(data) } -> std::same_as<bool>;
    requires std::is_member_function_pointer_v<
        decltype(static_cast<bool (T::*)(std::string_view)>(
            &T::ParseFromString))>;
  };
};

}  // namespace endpoint_client::detail

#endif  // BRAVE_COMPONENTS_ENDPOINT_CLIENT_PROTOBUF_H_
