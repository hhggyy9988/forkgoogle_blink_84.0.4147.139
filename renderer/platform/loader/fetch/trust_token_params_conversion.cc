// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "third_party/blink/renderer/platform/loader/fetch/trust_token_params_conversion.h"
#include "services/network/public/cpp/optional_trust_token_params.h"
#include "services/network/public/mojom/trust_tokens.mojom-blink.h"

namespace blink {

network::OptionalTrustTokenParams ConvertTrustTokenParams(
    const base::Optional<network::mojom::blink::TrustTokenParams>& maybe_in) {
  if (!maybe_in)
    return base::nullopt;
  const network::mojom::blink::TrustTokenParams& in = *maybe_in;

  network::mojom::TrustTokenParamsPtr out =
      network::mojom::TrustTokenParams::New();
  out->type = in.type;
  out->refresh_policy = in.refresh_policy;
  out->sign_request_data = in.sign_request_data;
  out->include_timestamp_header = in.include_timestamp_header;
  // Optional value:
  if (in.issuer)
    out->issuer = in.issuer->ToUrlOrigin();
  for (const String& additional_header : in.additional_signed_headers) {
    out->additional_signed_headers.push_back(additional_header.Latin1());
  }

  return network::OptionalTrustTokenParams(std::move(out));
}

}  // namespace blink
