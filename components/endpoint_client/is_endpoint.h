/* Copyright (c) 2025 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_ENDPOINT_CLIENT_IS_ENDPOINT_H_
#define BRAVE_COMPONENTS_ENDPOINT_CLIENT_IS_ENDPOINT_H_

#include <concepts>

#include "brave/components/endpoint_client/is_request.h"
#include "brave/components/endpoint_client/is_response.h"
#include "url/gurl.h"

namespace endpoint_client {

namespace detail {

template <typename T>
concept HasEndpointStructure = requires {
  typename T::Request;
  typename T::Response;
  { T::URL() } -> std::same_as<GURL>;
};

template <typename T>
concept IsJSONEndpoint =
    HasEndpointStructure<T> && IsJSONRequest<typename T::Request> &&
    IsJSONResponse<typename T::Response>;

template <typename T>
concept IsProtobufEndpoint =
    HasEndpointStructure<T> && IsProtobufRequest<typename T::Request> &&
    IsProtobufResponse<typename T::Response>;

}  // namespace detail

template <typename T>
concept IsEndpoint = detail::IsJSONEndpoint<T> || detail::IsProtobufEndpoint<T>;

}  // namespace endpoint_client

#endif  // BRAVE_COMPONENTS_ENDPOINT_CLIENT_IS_ENDPOINT_H_
