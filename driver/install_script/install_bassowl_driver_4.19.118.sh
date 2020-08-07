#!/bin/bash

wget -N  https://github.com/Darmur/bassowl-hat/raw/master/driver/archives/modules-rpi-4.19.118-bassowl.tar.gz
sudo tar -C / -xvf modules-rpi-4.19.118-bassowl.tar.gz --strip=1 --no-same-owner
sudo depmod

echo "BassOwl-HAT driver installed"
