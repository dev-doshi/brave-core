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

  // Returns the Content-Type value associated with JSON payloads.
  static constexpr std::string_view ContentType() { return "application/json"; }

  // Serializes a JSON request body to a JSON string.
  // Returns std::nullopt if the request body produces an empty object.
  template <typename RequestBody>
    requires kIsRequestBody<RequestBody>
  static std::optional<std::string> Serialize(const RequestBody& request_body) {
    const auto dict = request_body.ToValue();
    return !dict.empty() ? base::WriteJson(dict).value_or("")
                         : std::optional<std::string>();
  }

  // Deserializes a JSON response payload into the endpoint's
  // typed Response::body value. The SuccessBody or ErrorBody is
  // selected based on whether the HTTP status is 2xx.
  template <typename Response>
    requires(kIsResponseBody<typename Response::SuccessBody> &&
             kIsResponseBody<typename Response::ErrorBody>)
  static auto Deserialize(bool is_2xx,
                          std::optional<std::string> response_body) {
    if (is_2xx ? std::is_empty_v<typename Response::SuccessBody>
               : std::is_empty_v<typename Response::ErrorBody>) {
      response_body = "{}";
    }

    const auto value =
        base::JSONReader::Read(response_body.value_or(""), base::JSON_PARSE_RFC)
            .value_or(base::Value());

    decltype(Response().body) result;
    if (is_2xx) {
      result = Response::SuccessBody::FromValue(value);
    } else {
      result = Response::ErrorBody::FromValue(value).transform(
          [](auto body) { return base::unexpected(std::move(body)); });
    }

    return result;
  }
};

}  // namespace endpoint_client::detail

#endif  // BRAVE_COMPONENTS_ENDPOINT_CLIENT_JSON_H_
