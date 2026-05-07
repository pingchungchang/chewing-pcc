#!/usr/bin/env bash
INSTALL_DIR="$(pwd)/dist"
rm -rf "$INSTALL_DIR"

mkdir -p build && cd build

cmake .. \
  -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR" \
  -DCMAKE_INSTALL_LIBDIR=lib \
  -DCMAKE_BUILD_TYPE=Debug

make -j$(nproc)
make install

mkdir -p "$INSTALL_DIR/lib/fcitx5/addons"

if [ -f "$INSTALL_DIR/share/fcitx5/addon/intel_chewing.conf" ]; then
    cp "$INSTALL_DIR/share/fcitx5/addon/intel_chewing.conf" "$INSTALL_DIR/lib/fcitx5/addons/"
fi

export FCITX_ADDON_DIRS="$INSTALL_DIR/lib/fcitx5"

SYSTEM_FCITX_LIB=$(dirname $(readlink -f $(which fcitx5)))/../lib/fcitx5
export FCITX_ADDON_DIRS="$FCITX_ADDON_DIRS:$SYSTEM_FCITX_LIB"

export LD_LIBRARY_PATH="$INSTALL_DIR/lib/fcitx5:$INSTALL_DIR/lib:$LD_LIBRARY_PATH"

export XDG_DATA_DIRS="$INSTALL_DIR/share:$XDG_DATA_DIRS"

fcitx5 -rd
