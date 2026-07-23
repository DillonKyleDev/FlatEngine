#!/bin/bash

echo "Installing FlatEngine 3D dependencies..."

# Update first
sudo dnf update -y

# SDL2
sudo dnf install -y SDL2-devel SDL2_image-devel SDL2_ttf-devel SDL2_mixer-devel

# Vulkan
sudo dnf install -y vulkan-devel vulkan-loader-devel vulkan-validation-layers

# GLM (header only)
sudo dnf install -y glm-devel

# Lua
sudo dnf install -y lua-devel

# Build tools
sudo dnf install -y make cmake premake5 clang clang-tools-extra bear

# Box2D - build from source fork
git clone https://github.com/DillonKyleDev/box2d.git /tmp/box2d
cd /tmp/box2d
cmake -B build -DCMAKE_BUILD_TYPE=Release -DBOX2D_SAMPLES=OFF -DBOX2D_UNIT_TESTS=OFF
cmake --build build
sudo cmake --install build
cd -
rm -rf /tmp/box2d

echo "Done! Run 'premake5 gmake' to generate makefiles."
