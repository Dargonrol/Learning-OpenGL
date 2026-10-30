import os
from conan import ConanFile
from conan.tools.cmake import cmake_layout, CMakeToolchain
from conan.tools.files import copy

class Project(ConanFile):
    name = "project"
    version = "0.1"

    settings = "os", "compiler", "build_type", "arch"

    generators = "CMakeDeps"

    def requirements(self):
        self.requires("glfw/3.4")
        self.requires("glm/1.0.1")
        self.requires("assimp/6.0.5")
        self.requires("nlohmann_json/3.11.3")
        self.requires("spdlog/1.17.0")

    def configure(self):
        self.options["assimp"].shared = False
        self.options["assimp"].with_assimp_view = False
        self.options["assimp"].with_tests = False
        self.options["assimp"].tools = False
        self.options["assimp"].with_blend = False
        self.options["assimp"].with_gltf = False

    def layout(self):
        self.folders.root = ".."

        arch_dir = "x64" if self.settings.arch == "x86_64" else "x86"
        build_type = str(self.settings.build_type)

        self.folders.source = "."
        self.folders.build = os.path.join("build", arch_dir, build_type)
        self.folders.generators = os.path.join(self.folders.build, "generators")

        self.cpp.source.components[""].includedirs = ["src"]

    def generate(self):
        tc = CMakeToolchain(self)
        tc.user_presets_path = "CMakeUserPresets.json"
        arch_tag = "x64" if self.settings.arch == "x86_64" else "x86"
        tc.presets_prefix = arch_tag

        tc.generate()