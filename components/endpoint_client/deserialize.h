/* Copyright (c) 2026 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_ENDPOINT_CLIENT_DESERIALIZE_H_
#define BRAVE_COMPONENTS_ENDPOINT_CLIENT_DESERIALIZE_H_

#include <optional>
#include <string>
#include <type_traits>

#include "base/json/json_reader.h"
#include "base/types/expected.h"
#include "base/values.h"
#include "brave/components/endpoint_client/is_response.h"

namespace endpoint_client::detail {

template <IsResponse<JSON> Response>
auto Deserialize(bool is_2xx, std::optional<std::string> response_body) {
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

template <IsResponse<Protobuf> Response>
auto Deserialize(bool is_2xx, std::optional<std::string> response_body) {
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

}  // namespace endpoint_client::detail

#endif  // BRAVE_COMPONENTS_ENDPOINT_CLIENT_DESERIALIZE_H_
