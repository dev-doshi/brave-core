/* Copyright (c) 2026 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_ENDPOINT_CLIENT_JSON_H_
#define BRAVE_COMPONENTS_ENDPOINT_CLIENT_JSON_H_

#include <concepts>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>

#include "base/json/json_reader.h"
#include "base/json/json_writer.h"
#include "base/types/expected.h"
#include "base/values.h"

namespace endpoint_client::detail {

struct JSON {
  // Checks whether `T` defines a non-static, accessible member
  // function `ToValue()` such that:
  //   - `t.ToValue()` is a valid expression,
  //      and that call yields `base::DictValue`
  //   - `&T::ToValue` is a valid member function pointer (ensures it's a
  //      non-static member function)
  //
  // In short: models any type with a proper non-static `ToValue()` function
  // whose result is a `base::DictValue`.
  template <typename T>
  static constexpr bool kIsRequestBody = requires(T t) {
    { t.ToValue() } -> std::same_as<base::DictValue>;
    requires std::is_member_function_pointer_v<decltype(&T::ToValue)>;
  };

  // Checks whether `T` defines a static, accessible member
  // function `FromValue()` such that:
  //   - `T::FromValue(value)` is a valid expression,
  //      and that call yields `std::optional<T>`
  //
  // In short: models any type with a proper static `FromValue()` function
  // whose result is a `std::optional<T>`.
  template <typename T>
  static constexpr bool kIsResponseBody = requires(const base::Value& value) {
    { T::FromValue(value) } -> std::same_as<std::optional<T>>;
  };
};

}  // namespace endpoint_client::detail

#endif  // BRAVE_COMPONENTS_ENDPOINT_CLIENT_JSON_H_
