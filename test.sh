#!/bin/bash

sudo chmod -R 755 build
sudo chown -R root:wheel build
sudo kextutil build/XiaoMiProVoodooI2C.kext -no-load -t