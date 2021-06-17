// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "third_party/blink/renderer/modules/push_messaging/push_subscription_options.h"

#include "third_party/blink/renderer/bindings/modules/v8/array_buffer_or_array_buffer_view_or_string.h"
#include "third_party/blink/renderer/bindings/modules/v8/v8_push_subscription_options_init.h"
#include "third_party/blink/renderer/core/typed_arrays/dom_array_buffer.h"
#include "third_party/blink/renderer/platform/bindings/exception_code.h"
#include "third_party/blink/renderer/platform/bindings/exception_state.h"
#include "third_party/blink/renderer/platform/wtf/assertions.h"
#include "third_party/blink/renderer/platform/wtf/std_lib_extras.h"
#include "third_party/blink/renderer/platform/wtf/text/ascii_ctype.h"
#include "third_party/blink/renderer/platform/wtf/text/base64.h"

namespace blink {
namespace {

const int kMaxApplicationServerKeyLength = 255;

Vector<uint8_t> BufferSourceToVector(
    const ArrayBufferOrArrayBufferViewOrString& application_server_key,
    ExceptionState& exception_state) {
  char* input;
  size_t length;
  Vector<char> decoded_application_server_key;
  Vector<uint8_t> result;

  // Convert the input array into a string of bytes.
  if (application_server_key.IsArrayBuffer()) {
    input =
        static_cast<char*>(application_server_key.GetAsArrayBuffer()->Data());
    length = application_server_key.GetAsArrayBuffer()->ByteLengthAsSizeT();
  } else if (application_server_key.IsArrayBufferView()) {
    input = static_cast<char*>(
        application_server_key.GetAsArrayBufferView().View()->buffer()->Data());
    length = application_server_key.GetAsArrayBufferView()
                 .View()
                 ->buffer()
                 ->ByteLengthAsSizeT();
  } else if (application_server_key.IsString()) {
    if (!Base64UnpaddedURLDecode(application_server_key.GetAsString(),
                                 decoded_application_server_key)) {
      exception_state.ThrowDOMException(
          DOMExceptionCode::kInvalidCharacterError,
          "The provided applicationServerKey is not encoded as base64url "
          "without padding.");
      return result;
    }
    input = reinterpret_cast<char*>(decoded_application_server_key.data());
    length = decoded_application_server_key.size();
  } else {
    NOTREACHED();
    return result;
  }

  // Check the validity of the sender info. It must either be a 65-byte
  // uncompressed VAPID key, which has the byte 0x04 as the first byte or a
  // numeric sender ID.
  const bool is_vapid = length == 65 && *input == 0x04;
  const bool is_sender_id =
      length > 0 && length < kMaxApplicationServerKeyLength &&
      (std::find_if_not(input, input + length, &WTF::IsASCIIDigit<char>) ==
       input + length);

  if (is_vapid || is_sender_id) {
    result.Append(input, static_cast<wtf_size_t>(length));
  } else {
    exception_state.ThrowDOMException(
        DOMExceptionCode::kInvalidAccessError,
        "The provided applicationServerKey is not valid.");
  }

  return result;
}

}  // namespace

// static
PushSubscriptionOptions* PushSubscriptionOptions::FromOptionsInit(
    const PushSubscriptionOptionsInit* options_init,
    ExceptionState& exception_state) {
  Vector<uint8_t> application_server_key;
  // TODO(crbug.com/1070871): PushSubscriptionOptionsInit.applicationServerKey
  // has a default value, but we check |hasApplicationServerKey()| here for
  // backward compatibility.
  if (options_init->hasApplicationServerKey() &&
      !options_init->applicationServerKey().IsNull()) {
    application_server_key.AppendVector(BufferSourceToVector(
        options_init->applicationServerKey(), exception_state));
  }

  return MakeGarbageCollected<PushSubscriptionOptions>(
      options_init->userVisibleOnly(), application_server_key);
}

PushSubscriptionOptions::PushSubscriptionOptions(
    bool user_visible_only,
    const Vector<uint8_t>& application_server_key)
    : user_visible_only_(user_visible_only),
      application_server_key_(DOMArrayBuffer::Create(
          application_server_key.data(),
          SafeCast<unsigned>(application_server_key.size()))) {}

void PushSubscriptionOptions::Trace(Visitor* visitor) {
  visitor->Trace(application_server_key_);
  ScriptWrappable::Trace(visitor);
}

}  // namespace blink
