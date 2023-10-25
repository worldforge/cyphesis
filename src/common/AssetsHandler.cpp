/*
 Copyright (C) 2023 Erik Ogenvik

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

#include "AssetsHandler.h"

AssetsHandler::AssetsHandler(std::string squallSignature)
        : mSquallSignature(squallSignature) {

}

std::string AssetsHandler::resolveAssetsUrl() const {
    //By omitting host we're telling the client to use the same host as the current connection.
//    return std::string("http://:6780/squall/" + mSquallSignature.substr(0, 2) + "/" + mSquallSignature.substr(2));
    return std::string("squall://:6780/squall#" + mSquallSignature);
}

