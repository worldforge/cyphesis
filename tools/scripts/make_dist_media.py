#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Requires Python 3 or higher

import fileinput
import fnmatch
import os
import re
import shutil
import subprocess

import sys


def find_matches(files, patterns):
    matches = []
    for pattern in patterns:
        for file in fnmatch.filter(files, pattern):
            matches.append(file)
    return matches


def collect_licenses(src_assets_path, licenses, assets):
    """Walk upwards looking for any license or author files"""
    for asset in assets:
        asset_path = os.path.join(src_assets_path, asset)
        if os.path.isfile(asset_path):
            asset_path_tokens = os.path.dirname(asset).split(os.sep)
            while len(asset_path_tokens) > 0:
                directory = os.sep.join(asset_path_tokens)
                for file in find_matches(os.listdir(os.path.join(src_assets_path, directory)),
                                         ["*license*", "*LICENSE*", "*authors*", "*AUTHORS*", "*COPYING*"]):
                    licenses.add(os.path.join(directory, file))
                del asset_path_tokens[-1]


def copy_assets(src_assets_path, dest_assets_path, assets, assets_no_conversion, image_max_size=None):
    original_size = 0
    destination_size = 0
    copied = 0
    skipped = 0
    converted = 0
    errors = 0
    for asset in assets:
        asset_path = os.path.join(src_assets_path, asset)
        if os.path.isfile(asset_path):

            png_to_dds_conversion_needed = False

            dest_asset_path = os.path.join(dest_assets_path, asset)

            dest_asset_dir_path = os.path.dirname(dest_asset_path)
            if not os.path.exists(dest_asset_dir_path):
                os.makedirs(dest_asset_dir_path)

            if dest_asset_path.endswith(".png") and not asset.startswith(
                    "common/ui") and asset not in assets_no_conversion:
                (dest_asset_path, _) = re.subn(r'.png$', '.dds', dest_asset_path)
                png_to_dds_conversion_needed = True

            # Check if the destination file exists, and if so is it older than the source
            if os.path.exists(dest_asset_path) and os.path.getmtime(dest_asset_path) >= os.path.getmtime(asset_path):
                # destination file is newer or of the same date as the source file
                skipped = skipped + 1
            else:
                if asset_path.endswith((".png", ".dds", ".jpg")) and not asset.startswith("common/ui"):
                    compression_format = ""

                    convert_cmd = ["convert"]

                    if image_max_size:
                        convert_cmd.append('-resize "{0}x{0}>"'.format(image_max_size))

                    if png_to_dds_conversion_needed:
                        imagemetadata = subprocess.check_output(["file", asset_path])
                        # TODO: use bc3n or bc1n for normal maps, with support in shaders
                        if "RGBA" in imagemetadata.decode("utf-8"):
                            compression_format = "dxt5"
                        else:
                            compression_format = "dxt1"

                        convert_cmd.append('-define dds:compression={0}'.format(compression_format))

                    convert_cmd.append(asset_path)
                    convert_cmd.append(dest_asset_path)

                    print("converting image asset {0} to {1} ({2})".format(asset, dest_asset_path, compression_format))
                    if os.system(" ".join(convert_cmd)) == 0:
                        converted = converted + 1
                    else:
                        errors = errors + 1
                else:
                    print("copying asset {0} to {1}".format(asset, dest_asset_path))
                    shutil.copy(asset_path, dest_asset_path)
                    copied = copied + 1
                    if asset.endswith(".material") and not asset.startswith("common/ui"):
                        print("processing material to replace 'png' with 'dds'")
                        for line in fileinput.input(dest_asset_path, inplace=True):
                            print(line.replace(".png", ".dds"), end='')

            original_size = original_size + os.path.getsize(asset_path)
            destination_size = destination_size + os.path.getsize(dest_asset_path)

        else:
            errors = errors + 1
            print("referenced file {0} does not exist".format(asset_path))

    return copied, converted, skipped, errors, original_size, destination_size


def copytree(src, dst):
    if not os.path.exists(dst):
        os.makedirs(dst)
    for item in os.listdir(src):
        s = os.path.join(src, item)
        d = os.path.join(dst, item)
        if os.path.isdir(s):
            # Skip "source" directories.
            if item == "source":
                continue
            copytree(s, d)
        else:
            if not os.path.exists(d) or os.stat(s).st_mtime - os.stat(d).st_mtime > 1:
                print("Copying " + s)
                shutil.copy2(s, d)


def main():
    """The main entry point."""
    usage = """usage: {0} <MEDIA_DIRECTORY_TRUNK> <OUTPUT_DIRECTORY> <IMAGE_MAX_SIZE>

This script will automatically scan the media source repo and create a repository suitable for distributions.
This process mainly involves copying assets and resizing textures.


MEDIA_DIRECTORY_TRUNK is the path to the root of the media directory, named "trunk"

OUTPUT_DIRECTORY is where the processed assets will be copied

IMAGE_MAX_SIZE (optional) the max size in pixels of any image. Any larger image will be scaled

OPTIONS
    -h Show this help text
""".format(sys.argv[0])

    if len(sys.argv) == 1:
        print("ERROR: MEDIA_DIRECTORY_TRUNK must be specified!")
        print(usage)
        sys.exit(1)
    elif len(sys.argv) == 2:
        print("ERROR: OUTPUT_DIRECTORY must be specified!")
        print(usage)
        sys.exit(1)

    if sys.argv[1] == "-h":
        print(usage)
        sys.exit(0)

    src_media_dir = sys.argv[1]
    # Some extra precaution.
    if os.path.basename(src_media_dir) != "trunk":
        print(
            "The first parameter should be a directory names 'trunk', as that's the root of the Subversion media repository.")
        sys.exit(1)

    src_assets_dir = os.path.join(src_media_dir, "assets")
    dest_assets_dir = os.path.abspath(sys.argv[2])

    if not os.path.exists(dest_assets_dir):
        os.makedirs(dest_assets_dir)

    max_size = None
    if len(sys.argv) > 3:
        max_size = int(sys.argv[3])

    # Copy license files
    shutil.copy(os.path.join(src_media_dir, "LICENSING.txt"), dest_assets_dir)
    shutil.copy(os.path.join(src_media_dir, "COPYING.txt"), dest_assets_dir)

    # Put all assets here, as paths relative to the "assets" directory in the media repo.
    assets = set()
    assets_no_conversion = set()

    # Copy all files found in the "assets" directory. Skip "source" directories.
    for dirpath, dirnames, files in os.walk(src_assets_dir):
        if "source" in dirnames:
            dirnames.remove("source")
        for filename in files:
            if filename.endswith(('.modeldef', '.entitymap', '.jpg', '.png', '.dds', '.skeleton', '.mesh', '.ttf',
                                  '.material', '.program', '.cg', '.glsl', '.vert', '.frag', '.hlsl', '.overlay',
                                  '.compositor', '.fontdef')):
                assets.add(os.path.relpath(os.path.join(dirpath, filename), src_assets_dir))

    licenses = set()
    collect_licenses(src_assets_dir, licenses, assets)

    assets.update(licenses)

    copied, converted, skipped, errors, original_size, destination_size = copy_assets(src_assets_dir, dest_assets_dir,
                                                                                      assets, assets_no_conversion,
                                                                                      max_size)
    print(
        "Media conversion completed.\n{0} files copied, {1} images converted, {2} files skipped, {3} with errors".format(
            copied, converted, skipped, errors))
    print("Original size:    {0} MB".format(original_size / 1000000))
    print("Destination size: {0} MB".format(destination_size / 1000000))

    pass


if __name__ == '__main__':
    main()
