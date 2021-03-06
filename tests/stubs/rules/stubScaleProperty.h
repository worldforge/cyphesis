// AUTOGENERATED file, created by the tool generate_stub.py, don't edit!
// If you want to add your own functionality, instead edit the stubScaleProperty_custom.h file.

#ifndef STUB_RULES_SCALEPROPERTY_H
#define STUB_RULES_SCALEPROPERTY_H

#include "rules/ScaleProperty.h"
#include "stubScaleProperty_custom.h"

#ifndef STUB_ScaleProperty_apply
//#define STUB_ScaleProperty_apply
  void ScaleProperty::apply(LocatedEntity&)
  {
    
  }
#endif //STUB_ScaleProperty_apply

#ifndef STUB_ScaleProperty_get
//#define STUB_ScaleProperty_get
  int ScaleProperty::get(Atlas::Message::Element& val) const
  {
    return 0;
  }
#endif //STUB_ScaleProperty_get

#ifndef STUB_ScaleProperty_set
//#define STUB_ScaleProperty_set
  void ScaleProperty::set(const Atlas::Message::Element&)
  {
    
  }
#endif //STUB_ScaleProperty_set

#ifndef STUB_ScaleProperty_copy
//#define STUB_ScaleProperty_copy
  ScaleProperty* ScaleProperty::copy() const
  {
    return nullptr;
  }
#endif //STUB_ScaleProperty_copy

#ifndef STUB_ScaleProperty_scaledBbox
//#define STUB_ScaleProperty_scaledBbox
   WFMath::AxisBox<3> ScaleProperty::scaledBbox(const LocatedEntity& entity)
  {
    return *static_cast< WFMath::AxisBox<3>*>(nullptr);
  }
#endif //STUB_ScaleProperty_scaledBbox

#ifndef STUB_ScaleProperty_scaledBbox
//#define STUB_ScaleProperty_scaledBbox
   WFMath::AxisBox<3> ScaleProperty::scaledBbox(const LocatedEntity& entity, const BBoxProperty& bboxProperty)
  {
    return *static_cast< WFMath::AxisBox<3>*>(nullptr);
  }
#endif //STUB_ScaleProperty_scaledBbox

#ifndef STUB_ScaleProperty_scaledBbox
//#define STUB_ScaleProperty_scaledBbox
   WFMath::AxisBox<3> ScaleProperty::scaledBbox(const WFMath::AxisBox<3>& bbox, const WFMath::Vector<3>& scale)
  {
    return *static_cast< WFMath::AxisBox<3>*>(nullptr);
  }
#endif //STUB_ScaleProperty_scaledBbox


#endif