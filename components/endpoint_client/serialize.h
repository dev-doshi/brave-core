/* Copyright (c) 2026 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_ENDPOINT_CLIENT_SERIALIZE_H_
#define BRAVE_COMPONENTS_ENDPOINT_CLIENT_SERIALIZE_H_

#include <optional>
#include <string>

#include "base/json/json_writer.h"
#include "brave/components/endpoint_client/is_request_body.h"

namespace endpoint_client::detail {

template <IsRequestBody<JSON> RequestBody>
std::optional<std::string> Serialize(const RequestBody& request_body) {
  const auto dict = request_body.ToValue();
  return !dict.empty() ? base::WriteJson(dict).value_or("")
                       : std::optional<std::string>();
}

template <IsRequestBody<Protobuf> RequestBody>
std::optional<std::string> Serialize(const RequestBody& request_body) {
  return request_body.ByteSizeLong() ? request_body.SerializeAsString()
                                     : std::optional<std::string>();
}

}  // namespace endpoint_client::detail

#endif  // BRAVE_COMPONENTS_ENDPOINT_CLIENT_SERIALIZE_H_
