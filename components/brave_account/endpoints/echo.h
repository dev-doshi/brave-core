/* Copyright (c) 2026 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_BRAVE_ACCOUNT_ENDPOINTS_ECHO_H_
#define BRAVE_COMPONENTS_BRAVE_ACCOUNT_ENDPOINTS_ECHO_H_

#include "brave/components/brave_account/endpoints/echo_bodies.pb.h"
#include "brave/components/endpoint_client/request_types.h"
#include "brave/components/endpoint_client/response.h"

namespace brave_account::endpoints {

struct Echo {
  using Request = endpoint_client::POST<EchoBody>;
  using Response = endpoint_client::Response<EchoBody, EchoBody>;

  static GURL URL() { return GURL("http://127.0.0.1:8000/echo"); }
};

}  // namespace brave_account::endpoints

#endif  // BRAVE_COMPONENTS_BRAVE_ACCOUNT_ENDPOINTS_ECHO_H_
