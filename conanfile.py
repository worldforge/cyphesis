import os

from conan import ConanFile
from conan.tools.cmake import cmake_layout, CMakeDeps, CMakeToolchain


class Conan(ConanFile):
    package_type = "application"
    settings = "os", "arch", "compiler", "build_type"
    url = "https://github.com/worldforge/cyphesis"
    homepage = "https://www.worldforge.org"
    description = "Cyphesis - Worldforge server"
    license = "GPL-2.0-or-later"
    author = "Erik Ogenvik <erik@ogenvik.org>"
    deprecated = "A server for the Worldforge MMORPG project."

    def requirements(self):
        self.requires("atlas/0.7.0@worldforge")
        self.requires("mercator/0.4.0@worldforge")
        self.requires("varconf/1.0.3@worldforge")
        self.requires("wfmath/1.0.3@worldforge")
        self.requires("libxdg-basedir/1.2.3@worldforge")
        self.requires("squall/0.1.0@worldforge")
        self.requires("worldforge-worlds/0.1.0@worldforge")
        self.requires("libsigcpp/3.0.7")
        self.requires("libgcrypt/1.8.4")
        self.requires("zlib/1.2.13")
        self.requires("bzip2/1.0.8")
        self.requires("sqlite3/3.42.0")
        self.requires("readline/8.1.2@worldforge")
        self.requires("cpython/3.10.0@worldforge")
        self.requires("boost/1.81.0")
        self.requires("bullet3/2.89")

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()

        tc = CMakeToolchain(self)
        tc.variables["PYTHON_IS_STATIC"] = "TRUE"
        # The default CMake FindPython3 component will set the Python3_EXECUTABLE, which is then used in Cyphesis. Therefore do this here.
        tc.variables["Python3_EXECUTABLE"] = os.path.join(self.dependencies["cpython"].package_folder, "bin/python")
        tc.variables["WORLDFORGE_WORLDS_SOURCE_PATH"] = os.path.join(
            self.dependencies["worldforge-worlds"].package_folder, "worlds")
        tc.preprocessor_definitions["PYTHONHOME"] = "\"{}\"".format(self.dependencies["cpython"].package_folder)
        tc.generate()

    def layout(self):
        cmake_layout(self)
