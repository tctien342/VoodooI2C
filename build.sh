#!/bin/bash

sudo xcodebuild -workspace "VoodooI2C.xcworkspace" -scheme "VoodooI2C" -derivedDataPath .dist
sudo rm -rf build
mkdir build
#/XiaoMiProVoodooI2C.kext
sudo mv -f .dist/Build/Products/Release/VoodooI2C.kext build
sudo mv -f .dist/Build/Products/Release/VoodooI2CELAN.kext build
sudo mv -f .dist/Build/Products/Release/VoodooI2CHID.kext build
sudo mv -f .dist/Build/Products/Release/VoodooI2CUPDDEngine.kext build
sudo rm -rf .dist
