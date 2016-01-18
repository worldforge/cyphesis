/*
 * ModeProperty.h
 *
 *  Created on: 10 jul 2015
 *      Author: erik
 */

#ifndef RULESETS_MODEPROPERTY_H_
#define RULESETS_MODEPROPERTY_H_

#include "common/Property.h"

class ModeProperty : public Property<std::string> {
    public:
        ModeProperty();
        virtual ~ModeProperty();
        virtual void apply(LocatedEntity *);

};

#endif /* RULESETS_MODEPROPERTY_H_ */
