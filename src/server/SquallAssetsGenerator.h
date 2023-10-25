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

#ifndef CYPHESIS_SQUALLASSETSGENERATOR_H
#define CYPHESIS_SQUALLASSETSGENERATOR_H

#include <squall/core/Repository.h>

class SquallAssetsGenerator {
public:
    SquallAssetsGenerator(Squall::Repository repository, std::filesystem::path assetsPath);

    std::optional<Squall::Signature> generateFromAssets(const std::string& rootName);

private:
    Squall::Repository mRepository;
    std::filesystem::path mAssetsPath;
};


#endif //CYPHESIS_SQUALLASSETSGENERATOR_H
