/* Copyright (c) 2025 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_ENDPOINT_CLIENT_RESPONSE_H_
#define BRAVE_COMPONENTS_ENDPOINT_CLIENT_RESPONSE_H_

#include <optional>
#include <string>

#include "base/types/expected.h"
#include "brave/components/endpoint_client/is_response.h"
#include "brave/components/endpoint_client/is_response_body.h"
#include "brave/components/endpoint_client/json.h"
#include "brave/components/endpoint_client/protobuf.h"
#include "net/base/net_errors.h"

namespace endpoint_client {

template <detail::IsResponseBody T, detail::IsResponseBody E>
struct Response {
  using SuccessBody = T;
  using ErrorBody = E;

  static auto Deserialize(bool is_2xx,
                          std::optional<std::string> response_body) {
    return Deserialize<Response>(is_2xx, std::move(response_body));
  }

  int net_error = net::ERR_IO_PENDING;
  std::optional<int> status_code;
  std::optional<base::expected<T, E>> body;

 private:
  // Deserializes a JSON response payload into the endpoint's
  // typed Response::body value. The SuccessBody or ErrorBody is
  // selected based on whether the HTTP status is 2xx.
  template <detail::IsResponse<detail::JSON> R>
  static auto Deserialize(bool is_2xx,
                          std::optional<std::string> response_body) {
    if (is_2xx ? std::is_empty_v<typename R::SuccessBody>
               : std::is_empty_v<typename R::ErrorBody>) {
      response_body = "{}";
    }

    const auto value =
        base::JSONReader::Read(response_body.value_or(""), base::JSON_PARSE_RFC)
            .value_or(base::Value());

    decltype(R().body) result;
    if (is_2xx) {
      result = R::SuccessBody::FromValue(value);
    } else {
      result = R::ErrorBody::FromValue(value).transform(
          [](auto body) { return base::unexpected(std::move(body)); });
    }

    return result;
  }

  // Deserializes a Protobuf response payload into the endpoint's
  // typed Response::body value. The SuccessBody or ErrorBody is
  // selected based on whether the HTTP status is 2xx.
  template <detail::IsResponse<detail::Protobuf> R>
  static auto Deserialize(bool is_2xx,
                          std::optional<std::string> response_body) {
    if (is_2xx ? std::is_empty_v<typename R::SuccessBody>
               : std::is_empty_v<typename R::ErrorBody>) {
      response_body = "";
    }

    const auto deserialize = [&](auto body) {
      return body.ParseFromString(response_body.value_or(""))
                 ? std::optional(std::move(body))
                 : std::nullopt;
    };

    decltype(R().body) result;
    if (is_2xx) {
      result = deserialize(typename R::SuccessBody());
    } else {
      result = deserialize(typename R::ErrorBody()).transform([](auto body) {
        return base::unexpected(std::move(body));
      });
    }

    return result;
  }
};

}  // namespace endpoint_client

#endif  // BRAVE_COMPONENTS_ENDPOINT_CLIENT_RESPONSE_H_
