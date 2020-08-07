#!/bin/bash

wget -N  https://github.com/Darmur/bassowl-hat/raw/master/driver/archives/modules-rpi-5.4.51-bassowl.tar.gz
sudo tar -C / -xvf modules-rpi-5.4.51-bassowl.tar.gz --strip=1 --no-same-owner
sudo depmod

echo "BassOwl-HAT driver installed"
