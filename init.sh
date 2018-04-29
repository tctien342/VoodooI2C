#!/bin/bash

pip install cpplint
git submodule init && git submodule update
cd /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs && sudo curl -O -L https://github.com/phracker/MacOSX-SDKs/releases/download/10.13/MacOSX10.11.sdk.tar.xz && sudo tar -xJf MacOSX10.11.sdk.tar.xz