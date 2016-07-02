/*
 * TransformsProperty.cpp
 *
 *  Created on: 2 nov 2015
 *      Author: erik
 */

#include "rulesets/TransformsProperty.h"

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Objects::Entity::RootEntity;

const std::string TransformsProperty::property_name = "transforms";

TransformsProperty::TransformsProperty() {
}

WFMath::Vector<3>& TransformsProperty::getTranslate() {
    return mTransform.translate;
}

const WFMath::Vector<3>& TransformsProperty::getTranslate() const {
    return mTransform.translate;
}

WFMath::Quaternion& TransformsProperty::getRotate() {
    return mTransform.rotate;
}
const WFMath::Quaternion& TransformsProperty::getRotate() const {
    return mTransform.rotate;
}

WFMath::Vector<3>& TransformsProperty::getTranslateScaled() {
    return mTransform.translateScaled;
}

const WFMath::Vector<3>& TransformsProperty::getTranslateScaled() const {
    return mTransform.translateScaled;
}

void TransformsProperty::apply(LocatedEntity *entity) {

}

int TransformsProperty::get(Element & val) const {
    return 0;
}

void TransformsProperty::set(const Element & val) {
}

TransformsProperty * TransformsProperty::copy() const {
    return new TransformsProperty(*this);
}

std::map<std::string, Transform>& TransformsProperty::external() {
    return mExternal;
}

const std::map<std::string, Transform>& TransformsProperty::external() const {
    return mExternal;
}

