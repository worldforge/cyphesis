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

#include "SquallAssetsGenerator.h"
#include "squall/core/Generator.h"

SquallAssetsGenerator::SquallAssetsGenerator(Squall::Repository repository, std::filesystem::path assetsPath)
        : mRepository(repository), mAssetsPath(assetsPath) {

}

std::optional<Squall::Signature> SquallAssetsGenerator::generateFromAssets(const std::string& rootName) {
    auto root = mRepository.readRoot(rootName);
    std::map<std::filesystem::path, Squall::Generator::ExistingEntry> existingEntries;
    if (root) {
        existingEntries = Squall::Generator::readExistingEntries(mRepository, root->signature);
    }

    Squall::Generator generator(mRepository, mAssetsPath, {.exclude={std::regex{"source"}}, .existingEntries=std::move(existingEntries)});

    Squall::GenerateResult result;
    do {
        result = generator.process(10);
    } while (!result.complete);

    if (result.processedFiles.empty()) {
        return std::optional<Squall::Signature>();
    } else {
        auto& lastEntry = *(--result.processedFiles.end());
        auto signature = lastEntry.fileEntry.signature;
        mRepository.storeRoot(rootName, Squall::Root{.signature = signature});
        return std::optional<Squall::Signature>(signature);
    }
}
