// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "third_party/blink/renderer/platform/graphics/color_space_profile_data.h"

#include "base/check.h"
#include "third_party/blink/renderer/platform/wtf/wtf.h"

struct bt709ColorProfile {
  static char* data()  // BT.709 HDTV ITU
  {
    static unsigned char colorProfile[] = {
        0x00, 0x00, 0x01, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
        0x6d, 0x6e, 0x74, 0x72, 0x52, 0x47, 0x42, 0x20, 0x58, 0x59, 0x5a, 0x20,
        0x07, 0xdf, 0x00, 0x0c, 0x00, 0x1f, 0x00, 0x17, 0x00, 0x3b, 0x00, 0x3b,
        0x61, 0x63, 0x73, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf6, 0xd6,
        0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0xd3, 0x2d, 0x67, 0x6f, 0x6f, 0x67,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08,
        0x64, 0x65, 0x73, 0x63, 0x00, 0x00, 0x00, 0xe4, 0x00, 0x00, 0x00, 0x25,
        0x77, 0x74, 0x70, 0x74, 0x00, 0x00, 0x01, 0x0c, 0x00, 0x00, 0x00, 0x14,
        0x72, 0x58, 0x59, 0x5a, 0x00, 0x00, 0x01, 0x20, 0x00, 0x00, 0x00, 0x14,
        0x67, 0x58, 0x59, 0x5a, 0x00, 0x00, 0x01, 0x34, 0x00, 0x00, 0x00, 0x14,
        0x62, 0x58, 0x59, 0x5a, 0x00, 0x00, 0x01, 0x48, 0x00, 0x00, 0x00, 0x14,
        0x72, 0x54, 0x52, 0x43, 0x00, 0x00, 0x01, 0x5c, 0x00, 0x00, 0x00, 0x20,
        0x62, 0x54, 0x52, 0x43, 0x00, 0x00, 0x01, 0x5c, 0x00, 0x00, 0x00, 0x20,
        0x67, 0x54, 0x52, 0x43, 0x00, 0x00, 0x01, 0x5c, 0x00, 0x00, 0x00, 0x20,
        0x64, 0x65, 0x73, 0x63, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11,
        0x42, 0x54, 0x2e, 0x37, 0x30, 0x39, 0x20, 0x48, 0x44, 0x54, 0x56, 0x20,
        0x49, 0x54, 0x55, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x58, 0x59, 0x5a, 0x20, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xf3, 0x51, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x16, 0xcc,
        0x58, 0x59, 0x5a, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6f, 0xa2,
        0x00, 0x00, 0x38, 0xf5, 0x00, 0x00, 0x03, 0x90, 0x58, 0x59, 0x5a, 0x20,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x62, 0x99, 0x00, 0x00, 0xb7, 0x85,
        0x00, 0x00, 0x18, 0xda, 0x58, 0x59, 0x5a, 0x20, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x24, 0xa0, 0x00, 0x00, 0x0f, 0x84, 0x00, 0x00, 0xb6, 0xcf,
        0x70, 0x61, 0x72, 0x61, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00,
        0x00, 0x02, 0x38, 0xe4, 0x00, 0x00, 0xe8, 0xf0, 0x00, 0x00, 0x17, 0x10,
        0x00, 0x00, 0x38, 0xe3, 0x00, 0x00, 0x14, 0xbc};

    return reinterpret_cast<char*>(colorProfile);
  }

  static size_t size() { return 380u; }
};

struct bt601ColorProfile {
  static char* data()  // BT.601 SMPTE-C
  {
    static unsigned char colorProfile[] = {
        0x00, 0x00, 0x01, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
        0x6d, 0x6e, 0x74, 0x72, 0x52, 0x47, 0x42, 0x20, 0x58, 0x59, 0x5a, 0x20,
        0x07, 0xdf, 0x00, 0x0c, 0x00, 0x1f, 0x00, 0x17, 0x00, 0x3b, 0x00, 0x3b,
        0x61, 0x63, 0x73, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf6, 0xd6,
        0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0xd3, 0x2d, 0x67, 0x6f, 0x6f, 0x67,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08,
        0x64, 0x65, 0x73, 0x63, 0x00, 0x00, 0x00, 0xe4, 0x00, 0x00, 0x00, 0x25,
        0x77, 0x74, 0x70, 0x74, 0x00, 0x00, 0x01, 0x0c, 0x00, 0x00, 0x00, 0x14,
        0x72, 0x58, 0x59, 0x5a, 0x00, 0x00, 0x01, 0x20, 0x00, 0x00, 0x00, 0x14,
        0x67, 0x58, 0x59, 0x5a, 0x00, 0x00, 0x01, 0x34, 0x00, 0x00, 0x00, 0x14,
        0x62, 0x58, 0x59, 0x5a, 0x00, 0x00, 0x01, 0x48, 0x00, 0x00, 0x00, 0x14,
        0x72, 0x54, 0x52, 0x43, 0x00, 0x00, 0x01, 0x5c, 0x00, 0x00, 0x00, 0x20,
        0x62, 0x54, 0x52, 0x43, 0x00, 0x00, 0x01, 0x5c, 0x00, 0x00, 0x00, 0x20,
        0x67, 0x54, 0x52, 0x43, 0x00, 0x00, 0x01, 0x5c, 0x00, 0x00, 0x00, 0x20,
        0x64, 0x65, 0x73, 0x63, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f,
        0x42, 0x54, 0x2e, 0x36, 0x30, 0x31, 0x20, 0x53, 0x4d, 0x50, 0x54, 0x45,
        0x2d, 0x43, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x58, 0x59, 0x5a, 0x20, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xf3, 0x51, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x16, 0xcc,
        0x58, 0x59, 0x5a, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6a, 0x95,
        0x00, 0x00, 0x38, 0xc1, 0x00, 0x00, 0x03, 0x7f, 0x58, 0x59, 0x5a, 0x20,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x64, 0xa5, 0x00, 0x00, 0xb4, 0x09,
        0x00, 0x00, 0x17, 0x63, 0x58, 0x59, 0x5a, 0x20, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x27, 0x9d, 0x00, 0x00, 0x13, 0x36, 0x00, 0x00, 0xb8, 0x5f,
        0x70, 0x61, 0x72, 0x61, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00,
        0x00, 0x02, 0x38, 0xe4, 0x00, 0x00, 0xe8, 0xf0, 0x00, 0x00, 0x17, 0x10,
        0x00, 0x00, 0x38, 0xe3, 0x00, 0x00, 0x14, 0xbc};

    return reinterpret_cast<char*>(colorProfile);
  }

  static size_t size() { return 380u; }
};

namespace blink {

void Bt709ColorProfileData(Vector<char>& data) {
  DCHECK(WTF::IsMainThread());
  DCHECK(data.IsEmpty());

  data.Append(bt709ColorProfile::data(), bt709ColorProfile::size());
}

void Bt601ColorProfileData(Vector<char>& data) {
  DCHECK(WTF::IsMainThread());
  DCHECK(data.IsEmpty());

  data.Append(bt601ColorProfile::data(), bt601ColorProfile::size());
}

}  // namespace blink
