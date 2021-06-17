// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "third_party/blink/renderer/platform/fonts/opentype/open_type_math_support.h"
#include "base/memory/scoped_refptr.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/blink/renderer/platform/fonts/font.h"
#include "third_party/blink/renderer/platform/fonts/opentype/open_type_types.h"
#include "third_party/blink/renderer/platform/testing/font_test_helpers.h"
#include "third_party/blink/renderer/platform/testing/unit_test_helpers.h"

namespace {
const UChar32 kLeftBraceCodePoint = '{';
const UChar32 kOverBraceCodePoint = 0x23DE;
const UChar32 kArabicMathOperatorHahWithDalCodePoint = 0x1EEF1;
const UChar32 kNAryWhiteVerticalBarCodePoint = 0x2AFF;
}  // namespace

namespace blink {

class OpenTypeMathSupportTest : public testing::Test {
 protected:
  void SetUp() override {
    font_description.SetComputedSize(10.0);
    font = Font(font_description);
  }

  void TearDown() override {}

  Font CreateMathFont(const String& name, float size = 1000) {
    FontDescription::VariantLigatures ligatures;
    return blink::test::CreateTestFont(
        "MathTestFont",
        blink::test::BlinkWebTestsFontsTestDataPath(String("math/") + name),
        size, &ligatures);
  }

  bool HasMathData(const String& name) {
    return OpenTypeMathSupport::HasMathData(
        CreateMathFont(name).PrimaryFont()->PlatformData().GetHarfBuzzFace());
  }

  base::Optional<float> MathConstant(
      const String& name,
      OpenTypeMathSupport::MathConstants constant) {
    Font math = CreateMathFont(name);
    return OpenTypeMathSupport::MathConstant(
        math.PrimaryFont()->PlatformData().GetHarfBuzzFace(), constant);
  }

  FontDescription font_description;
  Font font;
};

TEST_F(OpenTypeMathSupportTest, HasMathData) {
  // Null parameter.
  EXPECT_FALSE(OpenTypeMathSupport::HasMathData(nullptr));

  // Font without a MATH table.
  EXPECT_FALSE(HasMathData("math-text.woff"));

  // Font with a MATH table.
  EXPECT_TRUE(HasMathData("axisheight5000-verticalarrow14000.woff"));
}

TEST_F(OpenTypeMathSupportTest, MathConstantNullOpt) {
  Font math_text = CreateMathFont("math-text.woff");

  for (int i = OpenTypeMathSupport::MathConstants::kScriptPercentScaleDown;
       i <=
       OpenTypeMathSupport::MathConstants::kRadicalDegreeBottomRaisePercent;
       i++) {
    auto math_constant = static_cast<OpenTypeMathSupport::MathConstants>(i);

    // Null parameter.
    EXPECT_FALSE(OpenTypeMathSupport::MathConstant(nullptr, math_constant));

    // Font without a MATH table.
    EXPECT_FALSE(OpenTypeMathSupport::MathConstant(
        math_text.PrimaryFont()->PlatformData().GetHarfBuzzFace(),
        math_constant));
  }
}

// See third_party/blink/web_tests/external/wpt/mathml/tools/percentscaledown.py
TEST_F(OpenTypeMathSupportTest, MathConstantPercentScaleDown) {
  {
    auto result = MathConstant(
        "scriptpercentscaledown80-scriptscriptpercentscaledown0.woff",
        OpenTypeMathSupport::MathConstants::kScriptPercentScaleDown);
    EXPECT_TRUE(result);
    EXPECT_FLOAT_EQ(*result, .8);
  }

  {
    auto result = MathConstant(
        "scriptpercentscaledown0-scriptscriptpercentscaledown40.woff",
        OpenTypeMathSupport::MathConstants::kScriptScriptPercentScaleDown);
    EXPECT_TRUE(result);
    EXPECT_FLOAT_EQ(*result, .4);
  }
}

// See third_party/blink/web_tests/external/wpt/mathml/tools/fractions.py
TEST_F(OpenTypeMathSupportTest, MathConstantFractions) {
  {
    auto result = MathConstant(
        "fraction-numeratorshiftup11000-axisheight1000-rulethickness1000.woff",
        OpenTypeMathSupport::MathConstants::kFractionNumeratorShiftUp);
    EXPECT_TRUE(result);
    EXPECT_FLOAT_EQ(*result, 11000);
  }

  {
    auto result = MathConstant(
        "fraction-numeratordisplaystyleshiftup2000-axisheight1000-"
        "rulethickness1000.woff",
        OpenTypeMathSupport::MathConstants::
            kFractionNumeratorDisplayStyleShiftUp);
    EXPECT_TRUE(result);
    EXPECT_FLOAT_EQ(*result, 2000);
  }

  {
    auto result = MathConstant(
        "fraction-denominatorshiftdown3000-axisheight1000-rulethickness1000."
        "woff",
        OpenTypeMathSupport::MathConstants::kFractionDenominatorShiftDown);
    EXPECT_TRUE(result);
    EXPECT_FLOAT_EQ(*result, 3000);
  }

  {
    auto result = MathConstant(
        "fraction-denominatordisplaystyleshiftdown6000-axisheight1000-"
        "rulethickness1000.woff",
        OpenTypeMathSupport::MathConstants::
            kFractionDenominatorDisplayStyleShiftDown);
    EXPECT_TRUE(result);
    EXPECT_FLOAT_EQ(*result, 6000);
  }

  {
    auto result = MathConstant(
        "fraction-numeratorgapmin9000-rulethickness1000.woff",
        OpenTypeMathSupport::MathConstants::kFractionNumeratorGapMin);
    EXPECT_TRUE(result);
    EXPECT_FLOAT_EQ(*result, 9000);
  }

  {
    auto result = MathConstant(
        "fraction-numeratordisplaystylegapmin8000-rulethickness1000.woff",
        OpenTypeMathSupport::MathConstants::kFractionNumDisplayStyleGapMin);
    EXPECT_TRUE(result);
    EXPECT_FLOAT_EQ(*result, 8000);
  }

  {
    auto result = MathConstant(
        "fraction-rulethickness10000.woff",
        OpenTypeMathSupport::MathConstants::kFractionRuleThickness);
    EXPECT_TRUE(result);
    EXPECT_FLOAT_EQ(*result, 10000);
  }

  {
    auto result = MathConstant(
        "fraction-denominatorgapmin4000-rulethickness1000.woff",
        OpenTypeMathSupport::MathConstants::kFractionDenominatorGapMin);
    EXPECT_TRUE(result);
    EXPECT_FLOAT_EQ(*result, 4000);
  }

  {
    auto result = MathConstant(
        "fraction-denominatordisplaystylegapmin5000-rulethickness1000.woff",
        OpenTypeMathSupport::MathConstants::kFractionDenomDisplayStyleGapMin);
    EXPECT_TRUE(result);
    EXPECT_FLOAT_EQ(*result, 5000);
  }
}

// See third_party/blink/web_tests/external/wpt/mathml/tools/radicals.py
TEST_F(OpenTypeMathSupportTest, MathConstantRadicals) {
  {
    auto result = MathConstant(
        "radical-degreebottomraisepercent25-rulethickness1000.woff",
        OpenTypeMathSupport::MathConstants::kRadicalDegreeBottomRaisePercent);
    EXPECT_TRUE(result);
    EXPECT_FLOAT_EQ(*result, .25);
  }

  {
    auto result =
        MathConstant("radical-verticalgap6000-rulethickness1000.woff",
                     OpenTypeMathSupport::MathConstants::kRadicalVerticalGap);
    EXPECT_TRUE(result);
    EXPECT_FLOAT_EQ(*result, 6000);
  }

  {
    auto result = MathConstant(
        "radical-displaystyleverticalgap7000-rulethickness1000.woff",
        OpenTypeMathSupport::MathConstants::kRadicalDisplayStyleVerticalGap);
    EXPECT_TRUE(result);
    EXPECT_FLOAT_EQ(*result, 7000);
  }

  {
    auto result =
        MathConstant("radical-rulethickness8000.woff",
                     OpenTypeMathSupport::MathConstants::kRadicalRuleThickness);
    EXPECT_TRUE(result);
    EXPECT_FLOAT_EQ(*result, 8000);
  }

  {
    auto result =
        MathConstant("radical-extraascender3000-rulethickness1000.woff",
                     OpenTypeMathSupport::MathConstants::kRadicalExtraAscender);
    EXPECT_TRUE(result);
    EXPECT_FLOAT_EQ(*result, 3000);
  }

  {
    auto result = MathConstant(
        "radical-kernbeforedegree4000-rulethickness1000.woff",
        OpenTypeMathSupport::MathConstants::kRadicalKernBeforeDegree);
    EXPECT_TRUE(result);
    EXPECT_FLOAT_EQ(*result, 4000);
  }

  {
    auto result = MathConstant(
        "radical-kernafterdegreeminus5000-rulethickness1000.woff",
        OpenTypeMathSupport::MathConstants::kRadicalKernAfterDegree);
    EXPECT_TRUE(result);
    EXPECT_FLOAT_EQ(*result, -5000);
  }
}

TEST_F(OpenTypeMathSupportTest, MathVariantsWithoutTable) {
  Font math = CreateMathFont("math-text.woff");
  auto glyph = math.PrimaryFont()->GlyphForCharacter('A');

  // Horizontal variants.
  {
    auto variants = OpenTypeMathSupport::GetGlyphVariantRecords(
        math.PrimaryFont()->PlatformData().GetHarfBuzzFace(), glyph,
        OpenTypeMathStretchData::StretchAxis::Horizontal);
    EXPECT_EQ(variants.size(), 1u);
    EXPECT_EQ(variants[0], glyph);
  }

  // Vertical variants.
  {
    auto variants = OpenTypeMathSupport::GetGlyphVariantRecords(
        math.PrimaryFont()->PlatformData().GetHarfBuzzFace(), glyph,
        OpenTypeMathStretchData::StretchAxis::Vertical);
    EXPECT_EQ(variants.size(), 1u);
    EXPECT_EQ(variants[0], glyph);
  }

  // Horizontal parts.
  {
    auto parts = OpenTypeMathSupport::GetGlyphPartRecords(
        math.PrimaryFont()->PlatformData().GetHarfBuzzFace(), glyph,
        OpenTypeMathStretchData::StretchAxis::Horizontal);
    EXPECT_TRUE(parts.IsEmpty());
  }

  // // Vertical parts.
  {
    auto parts = OpenTypeMathSupport::GetGlyphPartRecords(
        math.PrimaryFont()->PlatformData().GetHarfBuzzFace(), glyph,
        OpenTypeMathStretchData::StretchAxis::Vertical);
    EXPECT_TRUE(parts.IsEmpty());
  }
}

// Broken on all platforms by updated to 'operators.woff'. crbug.com/1082250
TEST_F(OpenTypeMathSupportTest, DISABLED_MathVariantsWithTable) {
  // operators.woff contains stretchy operators from the MathML operator
  // dictionary (including left and over braces) represented by squares.
  // It also contains glyphs h0, h1, h2, h3 and v0, v1, v2, v3 that are
  // respectively horizontal and vertical rectangles of increasing size.
  // The MathVariants table contains the following data for horizontal
  // (respectively vertical) operators:
  // - Glyph variants: h0, h1, h2, h3 (respectively v0, v1, v2, v3).
  // - Glyph parts: non-extender h2 and extender h1 (respectively v2 and v1).
  // For details, see createSizeVariants() and createStretchy() from
  // third_party/blink/web_tests/external/wpt/mathml/tools/operator-dictionary.py

  Font math = CreateMathFont("operators.woff");
  auto left_brace = math.PrimaryFont()->GlyphForCharacter(kLeftBraceCodePoint);
  auto over_brace = math.PrimaryFont()->GlyphForCharacter(kOverBraceCodePoint);

  // Calculate glyph indices from the last unicode character in the font.
  // TODO(https://crbug.com/1057596): Find a better way to access these glyph
  // indices.
  auto v0 = math.PrimaryFont()->GlyphForCharacter(
                kArabicMathOperatorHahWithDalCodePoint) +
            1;
  auto h0 = v0 + 1;
  auto v1 = h0 + 1;
  auto h1 = v1 + 1;
  auto v2 = h1 + 1;
  auto h2 = v2 + 1;
  auto v3 = h2 + 1;
  auto h3 = v3 + 1;

  // Vertical variants for vertical operator.
  {
    auto variants = OpenTypeMathSupport::GetGlyphVariantRecords(
        math.PrimaryFont()->PlatformData().GetHarfBuzzFace(), left_brace,
        OpenTypeMathStretchData::StretchAxis::Vertical);
    EXPECT_EQ(variants.size(), 5u);
    EXPECT_EQ(variants[0], left_brace);
    EXPECT_EQ(variants[1], v0);
    EXPECT_EQ(variants[2], v1);
    EXPECT_EQ(variants[3], v2);
    EXPECT_EQ(variants[4], v3);
  }

  // Horizontal variants for vertical operator.
  {
    auto variants = OpenTypeMathSupport::GetGlyphVariantRecords(
        math.PrimaryFont()->PlatformData().GetHarfBuzzFace(), left_brace,
        OpenTypeMathStretchData::StretchAxis::Horizontal);
    EXPECT_EQ(variants.size(), 1u);
    EXPECT_EQ(variants[0], left_brace);
  }

  // Horizontal variants for horizontal operator.
  {
    auto variants = OpenTypeMathSupport::GetGlyphVariantRecords(
        math.PrimaryFont()->PlatformData().GetHarfBuzzFace(), over_brace,
        OpenTypeMathStretchData::StretchAxis::Horizontal);
    EXPECT_EQ(variants.size(), 5u);
    EXPECT_EQ(variants[0], over_brace);
    EXPECT_EQ(variants[1], h0);
    EXPECT_EQ(variants[2], h1);
    EXPECT_EQ(variants[3], h2);
    EXPECT_EQ(variants[4], h3);
  }

  // Vertical variants for horizontal operator.
  {
    auto variants = OpenTypeMathSupport::GetGlyphVariantRecords(
        math.PrimaryFont()->PlatformData().GetHarfBuzzFace(), over_brace,
        OpenTypeMathStretchData::StretchAxis::Vertical);
    EXPECT_EQ(variants.size(), 1u);
    EXPECT_EQ(variants[0], over_brace);
  }

  // Vertical parts for vertical operator.
  {
    auto parts = OpenTypeMathSupport::GetGlyphPartRecords(
        math.PrimaryFont()->PlatformData().GetHarfBuzzFace(), left_brace,
        OpenTypeMathStretchData::StretchAxis::Vertical);
    EXPECT_EQ(parts.size(), 2u);
    EXPECT_EQ(parts[0].glyph, v2);
    EXPECT_FLOAT_EQ(parts[0].start_connector_length, 0);
    EXPECT_FLOAT_EQ(parts[0].end_connector_length, 1000);
    EXPECT_FLOAT_EQ(parts[0].full_advance, 3000);
    EXPECT_EQ(parts[0].is_extender, false);
    EXPECT_EQ(parts[1].glyph, v1);
    EXPECT_FLOAT_EQ(parts[1].start_connector_length, 1000);
    EXPECT_FLOAT_EQ(parts[1].end_connector_length, 1000);
    EXPECT_FLOAT_EQ(parts[1].full_advance, 2000);
    EXPECT_EQ(parts[1].is_extender, true);
  }

  // Horizontal parts for vertical operator.
  {
    auto parts = OpenTypeMathSupport::GetGlyphPartRecords(
        math.PrimaryFont()->PlatformData().GetHarfBuzzFace(), left_brace,
        OpenTypeMathStretchData::StretchAxis::Horizontal);
    EXPECT_TRUE(parts.IsEmpty());
  }

  // Horizontal parts for horizontal operator.
  {
    auto parts = OpenTypeMathSupport::GetGlyphPartRecords(
        math.PrimaryFont()->PlatformData().GetHarfBuzzFace(), over_brace,
        OpenTypeMathStretchData::StretchAxis::Horizontal);

    EXPECT_EQ(parts.size(), 2u);
    EXPECT_EQ(parts[0].glyph, h2);
    EXPECT_FLOAT_EQ(parts[0].start_connector_length, 0);
    EXPECT_FLOAT_EQ(parts[0].end_connector_length, 1000);
    EXPECT_FLOAT_EQ(parts[0].full_advance, 3000);
    EXPECT_EQ(parts[0].is_extender, false);

    EXPECT_EQ(parts[1].glyph, h1);
    EXPECT_FLOAT_EQ(parts[1].start_connector_length, 1000);
    EXPECT_FLOAT_EQ(parts[1].end_connector_length, 1000);
    EXPECT_FLOAT_EQ(parts[1].full_advance, 2000);
    EXPECT_EQ(parts[1].is_extender, true);
  }

  // Vertical parts for horizontal operator.
  {
    auto parts = OpenTypeMathSupport::GetGlyphPartRecords(
        math.PrimaryFont()->PlatformData().GetHarfBuzzFace(), over_brace,
        OpenTypeMathStretchData::StretchAxis::Vertical);
    EXPECT_TRUE(parts.IsEmpty());
  }
}

// See third_party/blink/web_tests/external/wpt/mathml/tools/largeop.py
TEST_F(OpenTypeMathSupportTest, MathItalicCorrection) {
  {
    Font math = CreateMathFont(
        "largeop-displayoperatorminheight2000-2AFF-italiccorrection3000.woff");
    Glyph base_glyph =
        math.PrimaryFont()->GlyphForCharacter(kNAryWhiteVerticalBarCodePoint);

    // Retrieve the glyph with italic correction.
    Vector<OpenTypeMathStretchData::GlyphVariantRecord> variants =
        OpenTypeMathSupport::GetGlyphVariantRecords(
            math.PrimaryFont()->PlatformData().GetHarfBuzzFace(), base_glyph,
            OpenTypeMathStretchData::StretchAxis::Vertical);
    EXPECT_EQ(variants.size(), 3u);
    EXPECT_EQ(variants[0], base_glyph);
    EXPECT_EQ(variants[1], base_glyph);
    Glyph glyph_with_italic_correction = variants[2];

    // MathItalicCorrection with a value.
    base::Optional<float> glyph_with_italic_correction_value =
        OpenTypeMathSupport::MathItalicCorrection(
            math.PrimaryFont()->PlatformData().GetHarfBuzzFace(),
            glyph_with_italic_correction);
    EXPECT_TRUE(glyph_with_italic_correction_value);
    EXPECT_FLOAT_EQ(*glyph_with_italic_correction_value, 3000);

    // GetGlyphPartRecords does not set italic correction when there is no
    // construction available.
    float italic_correction = -1000;
    Vector<OpenTypeMathStretchData::GlyphPartRecord> parts =
        OpenTypeMathSupport::GetGlyphPartRecords(
            math.PrimaryFont()->PlatformData().GetHarfBuzzFace(), base_glyph,
            OpenTypeMathStretchData::StretchAxis::Vertical, &italic_correction);
    EXPECT_TRUE(parts.IsEmpty());
    EXPECT_FLOAT_EQ(italic_correction, -1000);
  }

  {
    Font math = CreateMathFont(
        "largeop-displayoperatorminheight7000-2AFF-italiccorrection5000.woff");
    Glyph base_glyph =
        math.PrimaryFont()->GlyphForCharacter(kNAryWhiteVerticalBarCodePoint);

    // OpenTypeMathSupport::GetGlyphPartRecords sets italic correction.
    float italic_correction = -1000;
    Vector<OpenTypeMathStretchData::GlyphPartRecord> parts =
        OpenTypeMathSupport::GetGlyphPartRecords(
            math.PrimaryFont()->PlatformData().GetHarfBuzzFace(), base_glyph,
            OpenTypeMathStretchData::StretchAxis::Vertical, &italic_correction);
    EXPECT_EQ(parts.size(), 3u);
    EXPECT_FLOAT_EQ(italic_correction, 5000);
  }
}

}  // namespace blink
