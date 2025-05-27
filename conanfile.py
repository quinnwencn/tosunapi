from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps, cmake_layout
from conan.tools.files import copy


class TosunApiConan(ConanFile):
    name = "tosunapi"
    version = "0.5.0"
    package_type = "library"
    license = "MIT"
    url = "https://github.com/quinnwencn/tosunapi.git"
    author = "Quinn"
    description = "Tosun CAN API is a wrapper library, which wrap Tosun CAN device library"

    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": True, "fPIC": True}
    generator = "CMakeDeps", "CMakeToolchain"

    exports_sources = "CMakeLists.txt", "include/*", "src/*"

    def requirements(self):
        pass

    def build_requirements(self):
        self.build_requires("cmake/3.26.3")

    def layout(self):
        cmake_layout(self)

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.variables["TOSUN_LIB_PATH"] = f"{self.source_folder}/third_party/tosun/lib"
        tc.variables["TOSUN_INCLUDE_PATH"] = f"{self.source_folder}/third_party/tosun/include"
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        copy(self, "*.h", f"{self.source_folder}/include", f"{self.package_folder}/include")
        copy(self, "TSCANDef.h", f"{self.source_folder}/third_party/tosun/include", f"{self.package_folder}/include")

        cmake = CMake(self)
        cmake.install()

        copy(self, "*.so", f"{self.source_folder}/third_party/tosun/lib", f"{self.package_folder}/lib")

    def package_info(self):
        self.cpp_info.libs = ["tosunapi"]
        self.cpp_info.includedirs = ["include"]
        self.cpp_info.set_property("cmake_target_name", "tosunapi::tosunapi")