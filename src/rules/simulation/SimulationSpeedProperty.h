/*
 Copyright (C) 2019 Erik Ogenvik

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef CYPHESIS_SIMULATIONSPEEDPROPERTY_H
#define CYPHESIS_SIMULATIONSPEEDPROPERTY_H

#include "common/Property.h"

/**
 * Determines the speed of the simulation.
 * \ingroup PropertyClasses
 */
class SimulationSpeedProperty : public Property<double>
{
    public:
        static constexpr const char* property_name = "simulation_speed";

        /// \brief Constructor
        SimulationSpeedProperty() = default;

        SimulationSpeedProperty* copy() const override;

    protected:
        SimulationSpeedProperty(const SimulationSpeedProperty& rhs) = default;

};


#endif //CYPHESIS_SIMULATIONSPEEDPROPERTY_H
