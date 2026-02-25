/* Copyright (c) 2025 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_ENDPOINT_CLIENT_IS_ENDPOINT_H_
#define BRAVE_COMPONENTS_ENDPOINT_CLIENT_IS_ENDPOINT_H_

#include <concepts>

#include "brave/components/endpoint_client/is_request_body.h"
#include "brave/components/endpoint_client/is_request_of.h"
#include "brave/components/endpoint_client/is_response_of.h"
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
    HasEndpointStructure<T> && IsRequestOf<typename T::Request, JSON> &&
    IsResponseOf<typename T::Response, JSON>;

template <typename T>
concept IsProtobufEndpoint =
    HasEndpointStructure<T> && IsRequestOf<typename T::Request, Protobuf> &&
    IsResponseOf<typename T::Response, Protobuf>;

}  // namespace detail

template <typename T>
concept IsEndpoint = detail::IsJSONEndpoint<T> || detail::IsProtobufEndpoint<T>;

}  // namespace endpoint_client

#endif  // BRAVE_COMPONENTS_ENDPOINT_CLIENT_IS_ENDPOINT_H_
