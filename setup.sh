#!/usr/bin/env bash
set -e

rm -f CMakeUserPresets.json CMakePresets.json
rm -rf build

REQUIRED_TOOLS=("conan" "cmake" "ninja" "gcc" "pkg-config")
MISSING_TOOLS=()

for tool in "${REQUIRED_TOOLS[@]}"; do
    if ! command -v "$tool" &> /dev/null; then
        MISSING_TOOLS+=("$tool")
    fi
done

if command -v pkg-config &> /dev/null; then
    if ! pkg-config --exists xres; then
        MISSING_PACKAGES+=("libxres")
    fi
fi

if [ ${#MISSING_TOOLS[@]} -ne 0 ]; then
    echo "Error: The following required tool(s) are missing: ${MISSING_TOOLS[*]}" >&2
    echo "Please install them." >&2
    echo "On Arch: sudo pacman -S conan cmake ninja base-devel pkgconf libxres" >&2
    exit 1
fi

OPTIONAL_TOOLS=("mold" "ccache")
for tool in "${OPTIONAL_TOOLS[@]}"; do
    if command -v "$tool" &> /dev/null; then
        echo "--> Optional tool found: $tool"
    else
        echo "--> Note: $tool not found (continuing without it)."
    fi
done

sleep 2

if ! conan profile path default &>/dev/null; then
  conan profile detect --force
fi

CONAN_DIR="conan"

# Linux 64-bit Debug
echo "--> [1/4] x64 Debug..."
conan install ./$CONAN_DIR -pr:h=./$CONAN_DIR/profiles/linux_x64_gcc -s build_type=Debug -b=missing --update

# Linux 64-bit Release
echo "--> [2/4] x64 Release..."
conan install ./$CONAN_DIR -pr:h=./$CONAN_DIR/profiles/linux_x64_gcc -s build_type=Release -b=missing --update

# Linux 32-bit Debug
echo "--> [3/4] x84 Debug..."
conan install ./$CONAN_DIR -pr:h=./$CONAN_DIR/profiles/linux_x86_gcc -s arch=x86 -s build_type=Debug -b=missing --update

# Linux 32-bit Release
echo "--> [4/4] x84 Release..."
conan install ./$CONAN_DIR -pr:h=./$CONAN_DIR/profiles/linux_x86_gcc -s arch=x86 -s build_type=Release -b=missing --update

echo "=== Done! All build trees generated. ==="
echo "You can now reload the CMakeLists.txt in your IDE or configure and build manually:"
echo "  - cmake --list-presets              | to list all presets"
echo "  - cmake --preset x64-debug          | to configure the preset"
echo "  - cmake --build --preset x64-debug  | to build the preset"