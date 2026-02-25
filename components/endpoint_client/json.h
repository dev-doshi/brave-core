/* Copyright (c) 2026 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_ENDPOINT_CLIENT_JSON_H_
#define BRAVE_COMPONENTS_ENDPOINT_CLIENT_JSON_H_

#include <concepts>
#include <optional>
#include <string>
#include <type_traits>

#include "base/json/json_writer.h"
#include "base/values.h"

namespace endpoint_client::detail {

struct JSON {
  template <typename T>
  static constexpr bool kIsRequestBody = requires(T t) {
    { t.ToValue() } -> std::same_as<base::DictValue>;
    requires std::is_member_function_pointer_v<decltype(&T::ToValue)>;
  };

  template <typename T>
  static constexpr bool kIsResponseBody = requires(const base::Value& value) {
    { T::FromValue(value) } -> std::same_as<std::optional<T>>;
  };

  template <typename RequestBody>
    requires kIsRequestBody<RequestBody>
  static std::optional<std::string> Serialize(const RequestBody& request_body) {
    const auto dict = request_body.ToValue();
    return !dict.empty() ? base::WriteJson(dict).value_or("")
                         : std::optional<std::string>();
  }
};

}  // namespace endpoint_client::detail

#endif  // BRAVE_COMPONENTS_ENDPOINT_CLIENT_JSON_H_
