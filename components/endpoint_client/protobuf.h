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

  // Returns the Content-Type value associated with Protobuf payloads.
  static constexpr std::string_view ContentType() {
    return "application/x-protobuf";
  }

  // Serializes a Protobuf request body to a binary string.
  // Returns std::nullopt if the request body is empty (ByteSizeLong() == 0).
  template <typename RequestBody>
    requires kIsRequestBody<RequestBody>
  static std::optional<std::string> Serialize(const RequestBody& request_body) {
    return request_body.ByteSizeLong() ? request_body.SerializeAsString()
                                       : std::optional<std::string>();
  }

  // Deserializes a Protobuf response payload into the endpoint's
  // typed Response::body value. The SuccessBody or ErrorBody is
  // selected based on whether the HTTP status is 2xx.
  template <typename Response>
    requires(kIsResponseBody<typename Response::SuccessBody> &&
             kIsResponseBody<typename Response::ErrorBody>)
  static auto Deserialize(bool is_2xx,
                          std::optional<std::string> response_body) {
    if (is_2xx ? std::is_empty_v<typename Response::SuccessBody>
               : std::is_empty_v<typename Response::ErrorBody>) {
      response_body = "";
    }

    const auto deserialize = [&](auto body) {
      return body.ParseFromString(response_body.value_or(""))
                 ? std::optional(std::move(body))
                 : std::nullopt;
    };

    decltype(Response().body) result;
    if (is_2xx) {
      result = deserialize(typename Response::SuccessBody());
    } else {
      result =
          deserialize(typename Response::ErrorBody()).transform([](auto body) {
            return base::unexpected(std::move(body));
          });
    }

    return result;
  }
};

}  // namespace endpoint_client::detail

#endif  // BRAVE_COMPONENTS_ENDPOINT_CLIENT_PROTOBUF_H_
