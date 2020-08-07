#!/bin/bash

BASSOWL_STRING='    {"id":"bassowl","name":"BassOwl-HAT","overlay":"bassowl","alsanum":"2","mixer":"Master","modules":"","script":"","needsreboot":"yes"},'

mkdir -p ~/.bassowl/
[[ -e ~/.bassowl/dacs.json.orig ]] && cp ~/.bassowl/dacs.json.orig /volumio/app/plugins/system_controller/i2s_dacs/dacs.json
cp /volumio/app/plugins/system_controller/i2s_dacs/dacs.json ~/.bassowl/dacs.json.orig

head -n 11 ~/.bassowl/dacs.json.orig > ~/.bassowl/temp.txt
echo "$BASSOWL_STRING" >> ~/.bassowl/temp.txt
tail -n +12 ~/.bassowl/dacs.json.orig >> ~/.bassowl/temp.txt

cp ~/.bassowl/temp.txt /volumio/app/plugins/system_controller/i2s_dacs/dacs.json
rm ~/.bassowl/temp.txt

echo "BassOwl-HAT added to Volumio DAC list. Please reboot"
