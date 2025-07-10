#!/bin/bash
clear
mkdir -p $(pwd)/build && \
cd $(pwd)/build && \
cmake .. -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Debug && \
make && \
sudo make install && \
fcitx5 -rd
