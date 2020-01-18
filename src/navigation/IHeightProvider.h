/*
 Copyright (C) 2011 Erik Ogenvik

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

#ifndef EMBER_DOMAIN_IHEIGHTPROVIDER_H_
#define EMBER_DOMAIN_IHEIGHTPROVIDER_H_

#include <vector>


struct IHeightProvider
{
	virtual ~IHeightProvider() = default;

	/**
     * @brief Performs a fast copy of the raw height data for the supplied area.
     * @param xMin Minimum x coord of the area.
     * @param xMax Maximum x coord of the area.
     * @param yMin Minimum y coord of the area.
     * @param yMax Maximum y coord of the area.
     * @param heights A vector into which height data will be placed. This should preferably already have a capacity reserved.
     */
	virtual void blitHeights(int xMin, int xMax, int yMin, int yMax, std::vector<float>& heights) const = 0;

};



#endif /* EMBER_DOMAIN_IHEIGHTPROVIDER_H_ */
