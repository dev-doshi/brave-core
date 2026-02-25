/* Copyright (c) 2026 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_ENDPOINT_CLIENT_DESERIALIZE_H_
#define BRAVE_COMPONENTS_ENDPOINT_CLIENT_DESERIALIZE_H_

#include <optional>
#include <string>
#include <type_traits>

#include "base/check_deref.h"
#include "base/json/json_reader.h"
#include "base/types/expected.h"
#include "base/values.h"
#include "brave/components/endpoint_client/is_response.h"
#include "services/network/public/cpp/header_util.h"

namespace endpoint_client::detail {

template <typename Response>
  requires IsResponse<JSON, Response>
void Deserialize(Response& response, std::optional<std::string> response_body) {
  const bool is_2xx =
      network::IsSuccessfulStatus(CHECK_DEREF(response.status_code));

  if (is_2xx ? std::is_empty_v<typename Response::SuccessBody>
             : std::is_empty_v<typename Response::ErrorBody>) {
    response_body = "{}";
  }

  const auto value =
      base::JSONReader::Read(response_body.value_or(""), base::JSON_PARSE_RFC)
          .value_or(base::Value());
  if (is_2xx) {
    response.body = Response::SuccessBody::FromValue(value);
  } else {
    response.body = Response::ErrorBody::FromValue(value).transform(
        [](auto body) { return base::unexpected(std::move(body)); });
  }
}

template <typename Response>
  requires IsResponse<Protobuf, Response>
void Deserialize(Response& response, std::optional<std::string> response_body) {
  const bool is_2xx =
      network::IsSuccessfulStatus(CHECK_DEREF(response.status_code));

  if (is_2xx ? std::is_empty_v<typename Response::SuccessBody>
             : std::is_empty_v<typename Response::ErrorBody>) {
    response_body = "";
  }

  const auto deserialize = [&](auto body) {
    return body.ParseFromString(response_body.value_or(""))
               ? std::optional(std::move(body))
               : std::nullopt;
  };

  if (is_2xx) {
    response.body = deserialize(typename Response::SuccessBody());
  } else {
    response.body =
        deserialize(typename Response::ErrorBody()).transform([](auto body) {
          return base::unexpected(std::move(body));
        });
  }
}

}  // namespace endpoint_client::detail

#endif  // BRAVE_COMPONENTS_ENDPOINT_CLIENT_DESERIALIZE_H_
