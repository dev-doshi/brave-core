/* Copyright (c) 2026 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_ENDPOINT_CLIENT_SERIALIZE_H_
#define BRAVE_COMPONENTS_ENDPOINT_CLIENT_SERIALIZE_H_

#include <optional>
#include <string>

#include "base/json/json_writer.h"
#include "brave/components/endpoint_client/is_request.h"
#include "brave/components/endpoint_client/maybe_strip_with_headers.h"

namespace endpoint_client::detail {

template <typename Request>
  requires IsRequest<JSON, MaybeStripWithHeaders<Request>>
std::optional<std::string> Serialize(const Request& request) {
  const auto dict = request.body.ToValue();
  return !dict.empty() ? std::optional(base::WriteJson(dict).value_or(""))
                       : std::nullopt;
}

template <typename Request>
  requires IsRequest<Protobuf, MaybeStripWithHeaders<Request>>
std::optional<std::string> Serialize(const Request& request) {
  return request.body.ByteSizeLong() > 0
             ? std::optional(request.body.SerializeAsString())
             : std::nullopt;
}

}  // namespace endpoint_client::detail

#endif  // BRAVE_COMPONENTS_ENDPOINT_CLIENT_SERIALIZE_H_
