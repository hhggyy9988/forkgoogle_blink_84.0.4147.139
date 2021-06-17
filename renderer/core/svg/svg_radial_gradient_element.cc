/*
 * Copyright (C) 2004, 2005, 2006, 2008 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2006, 2007 Rob Buis <buis@kde.org>
 * Copyright (C) 2008 Eric Seidel <eric@webkit.org>
 * Copyright (C) 2008 Dirk Schulze <krit@webkit.org>
 * Copyright (C) Research In Motion Limited 2010. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "third_party/blink/renderer/core/svg/svg_radial_gradient_element.h"

#include "third_party/blink/renderer/core/layout/svg/layout_svg_resource_radial_gradient.h"
#include "third_party/blink/renderer/core/svg/radial_gradient_attributes.h"
#include "third_party/blink/renderer/platform/heap/heap.h"

namespace blink {

SVGRadialGradientElement::SVGRadialGradientElement(Document& document)
    : SVGGradientElement(svg_names::kRadialGradientTag, document),
      // Spec: If the cx/cy/r attribute is not specified, the effect is as if a
      // value of "50%" were specified.
      cx_(MakeGarbageCollected<SVGAnimatedLength>(
          this,
          svg_names::kCxAttr,
          SVGLengthMode::kWidth,
          SVGLength::Initial::kPercent50)),
      cy_(MakeGarbageCollected<SVGAnimatedLength>(
          this,
          svg_names::kCyAttr,
          SVGLengthMode::kHeight,
          SVGLength::Initial::kPercent50)),
      r_(MakeGarbageCollected<SVGAnimatedLength>(
          this,
          svg_names::kRAttr,
          SVGLengthMode::kOther,
          SVGLength::Initial::kPercent50)),
      fx_(MakeGarbageCollected<SVGAnimatedLength>(
          this,
          svg_names::kFxAttr,
          SVGLengthMode::kWidth,
          SVGLength::Initial::kPercent50)),
      fy_(MakeGarbageCollected<SVGAnimatedLength>(
          this,
          svg_names::kFyAttr,
          SVGLengthMode::kHeight,
          SVGLength::Initial::kPercent50)),
      // SVG2-Draft Spec: If the fr attribute is not specified, the effect is as
      // if a value of "0%" were specified.
      fr_(MakeGarbageCollected<SVGAnimatedLength>(
          this,
          svg_names::kFrAttr,
          SVGLengthMode::kOther,
          SVGLength::Initial::kPercent0)) {
  AddToPropertyMap(cx_);
  AddToPropertyMap(cy_);
  AddToPropertyMap(r_);
  AddToPropertyMap(fx_);
  AddToPropertyMap(fy_);
  AddToPropertyMap(fr_);
}

void SVGRadialGradientElement::Trace(Visitor* visitor) {
  visitor->Trace(cx_);
  visitor->Trace(cy_);
  visitor->Trace(r_);
  visitor->Trace(fx_);
  visitor->Trace(fy_);
  visitor->Trace(fr_);
  SVGGradientElement::Trace(visitor);
}

void SVGRadialGradientElement::SvgAttributeChanged(
    const QualifiedName& attr_name) {
  if (attr_name == svg_names::kCxAttr || attr_name == svg_names::kCyAttr ||
      attr_name == svg_names::kFxAttr || attr_name == svg_names::kFyAttr ||
      attr_name == svg_names::kRAttr || attr_name == svg_names::kFrAttr) {
    SVGElement::InvalidationGuard invalidation_guard(this);
    UpdateRelativeLengthsInformation();
    InvalidateGradient(layout_invalidation_reason::kAttributeChanged);
    return;
  }

  SVGGradientElement::SvgAttributeChanged(attr_name);
}

LayoutObject* SVGRadialGradientElement::CreateLayoutObject(const ComputedStyle&,
                                                           LegacyLayout) {
  return new LayoutSVGResourceRadialGradient(this);
}

static void SetGradientAttributes(const SVGGradientElement& element,
                                  RadialGradientAttributes& attributes,
                                  bool is_radial) {
  element.CollectCommonAttributes(attributes);

  if (!is_radial)
    return;
  const auto& radial = To<SVGRadialGradientElement>(element);

  if (!attributes.HasCx() && radial.cx()->IsSpecified())
    attributes.SetCx(radial.cx()->CurrentValue());

  if (!attributes.HasCy() && radial.cy()->IsSpecified())
    attributes.SetCy(radial.cy()->CurrentValue());

  if (!attributes.HasR() && radial.r()->IsSpecified())
    attributes.SetR(radial.r()->CurrentValue());

  if (!attributes.HasFx() && radial.fx()->IsSpecified())
    attributes.SetFx(radial.fx()->CurrentValue());

  if (!attributes.HasFy() && radial.fy()->IsSpecified())
    attributes.SetFy(radial.fy()->CurrentValue());

  if (!attributes.HasFr() && radial.fr()->IsSpecified())
    attributes.SetFr(radial.fr()->CurrentValue());
}

void SVGRadialGradientElement::CollectGradientAttributes(
    RadialGradientAttributes& attributes) const {
  DCHECK(GetLayoutObject());

  VisitedSet visited;
  const SVGGradientElement* current = this;

  while (true) {
    SetGradientAttributes(*current, attributes,
                          IsA<SVGRadialGradientElement>(*current));
    visited.insert(current);

    current = current->ReferencedElement();
    // Ignore the referenced gradient element if it is not attached.
    if (!current || !current->GetLayoutObject())
      break;
    // Cycle detection.
    if (visited.Contains(current))
      break;
  }

  // Handle default values for fx/fy
  if (!attributes.HasFx())
    attributes.SetFx(attributes.Cx());

  if (!attributes.HasFy())
    attributes.SetFy(attributes.Cy());
}

bool SVGRadialGradientElement::SelfHasRelativeLengths() const {
  return cx_->CurrentValue()->IsRelative() ||
         cy_->CurrentValue()->IsRelative() ||
         r_->CurrentValue()->IsRelative() ||
         fx_->CurrentValue()->IsRelative() ||
         fy_->CurrentValue()->IsRelative() || fr_->CurrentValue()->IsRelative();
}

}  // namespace blink
