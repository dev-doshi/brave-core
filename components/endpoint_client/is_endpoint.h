/* Copyright (c) 2025 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_ENDPOINT_CLIENT_IS_ENDPOINT_H_
#define BRAVE_COMPONENTS_ENDPOINT_CLIENT_IS_ENDPOINT_H_

#include <concepts>

#include "brave/components/endpoint_client/is_request.h"
#include "brave/components/endpoint_client/is_response.h"
#include "brave/components/endpoint_client/json.h"
#include "brave/components/endpoint_client/protobuf.h"
#include "url/gurl.h"

namespace endpoint_client {

namespace detail {

// Concept that checks whether `T` models a well-formed endpoint type.
// Specifically, it requires that:
//   - `T` defines a nested `Request` type,
//   - `T` defines a nested `Response` type,
//   - `T::URL()` is a valid expression,
//      and that call yields `GURL`
//
// In short: models any type that provides the structural
// components required to represent an endpoint.
template <typename T>
concept HasEndpointStructure = requires {
  typename T::Request;
  typename T::Response;
  { T::URL() } -> std::same_as<GURL>;
};

// Concept that checks whether `T` models a JSON-backed endpoint.
// Specifically, it requires that:
//   - `T` satisfies `HasEndpointStructure`,
//   - `T::Request` models a JSON request,
//   - `T::Response` models a JSON response
//
// In short: models any endpoint whose request and response
// bodies are encoded using JSON.
template <typename T>
concept IsJSONEndpoint =
    HasEndpointStructure<T> && IsRequest<typename T::Request, JSON> &&
    IsResponse<typename T::Response, JSON>;

// Concept that checks whether `T` models a Protobuf-backed endpoint.
// Specifically, it requires that:
//   - `T` satisfies `HasEndpointStructure`,
//   - `T::Request` models a Protobuf request,
//   - `T::Response` models a Protobuf response
//
// In short: models any endpoint whose request and response
// bodies are encoded using Protobuf.
template <typename T>
concept IsProtobufEndpoint =
    HasEndpointStructure<T> && IsRequest<typename T::Request, Protobuf> &&
    IsResponse<typename T::Response, Protobuf>;

}  // namespace detail

// Concept that checks whether `T` models a supported endpoint type.
template <typename T>
concept IsEndpoint = detail::IsJSONEndpoint<T> || detail::IsProtobufEndpoint<T>;

}  // namespace endpoint_client

#endif  // BRAVE_COMPONENTS_ENDPOINT_CLIENT_IS_ENDPOINT_H_
