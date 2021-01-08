#!/bin/bash

CPU=2

#KERNEL_VERSION=5.10.3
#VOLUMIO_HASH=da59cb1161dc7c75727ec5c7636f632c52170961
#PATCH=bassowl-5.10.x.patch

#KERNEL_VERSION=5.4.79
#VOLUMIO_HASH=0642816ed05d31fb37fc8fbbba9e1774b475113f
#PATCH=bassowl-5.4.x.patch

KERNEL_VERSION=5.4.72
VOLUMIO_HASH=b3b238cf1e64d0cc272732e77ae6002c75184495
PATCH=bassowl-5.4.x.patch

#KERNEL_VERSION=5.4.59
#VOLUMIO_HASH=caf7070cd6cece7e810e6f2661fc65899c58e297
#PATCH=bassowl-5.4.x.patch

#KERNEL_VERSION=5.4.51
#VOLUMIO_HASH=8382ece2b30be0beb87cac7f3b36824f194d01e9
#PATCH=bassowl-5.4.x.patch

#KERNEL_VERSION=4.19.118
#VOLUMIO_HASH=e1050e94821a70b2e4c72b318d6c6c968552e9a2
#PATCH=bassowl-4.19.x.patch

echo "!!!  Download kernel hash info  !!!"
wget -N https://raw.githubusercontent.com/Hexxeh/rpi-firmware/${VOLUMIO_HASH}/git_hash
GIT_HASH="$(cat git_hash)"

echo "!!!  Download kernel source  !!!"
wget https://github.com/raspberrypi/linux/archive/${GIT_HASH}.tar.gz

echo "!!!  Download kernel patch  !!!"
wget -N https://raw.githubusercontent.com/Darmur/bassowl-hat/master/driver/patch_rpi/${PATCH}

echo "!!!  Extract kernel source  !!!"
rm -rf linux-${KERNEL_VERSION}+/
tar xvzf ${GIT_HASH}.tar.gz
mv linux-${GIT_HASH}/ linux-${KERNEL_VERSION}+/

echo "!!! Create git repo and apply patch  !!!"
cd linux-${KERNEL_VERSION}+/
git init
git add --all
git commit -m "extracted files"
cp ../${PATCH} ${PATCH}
git apply ${PATCH}
git status
cd ..

echo "!!! Copy source files for other variants  !!!"
cp -r linux-${KERNEL_VERSION}+/ linux-${KERNEL_VERSION}-v7+/
cp -r linux-${KERNEL_VERSION}+/ linux-${KERNEL_VERSION}-v7l+/

echo "!!! Build RPi0 kernel and modules !!!"
cd linux-${KERNEL_VERSION}+/
KERNEL=kernel
make -j${CPU} ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- bcmrpi_defconfig
make -j${CPU} ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- zImage modules dtbs
cd ..
echo "!!!  RPi0 build done  !!!"
echo "-------------------------"

echo "!!!  Build RPi3 kernel and modules  !!!"
cd linux-${KERNEL_VERSION}-v7+/
KERNEL=kernel7
make -j${CPU} ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- bcm2709_defconfig
make -j${CPU} ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- zImage modules dtbs
cd ..
echo "!!!  RPi3 build done  !!!"
echo "-------------------------"

echo "!!!  Build RPi4 kernel and modules  !!!"
cd linux-${KERNEL_VERSION}-v7l+/
KERNEL=kernel7l
make -j${CPU} ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- bcm2711_defconfig
make -j${CPU} ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- zImage modules dtbs
cd ..
echo "!!!  RPi4 build done  !!!"
echo "-------------------------"

echo "!!!  Creating archive  !!!"
mkdir -p modules-rpi-${KERNEL_VERSION}-bassowl/boot/overlays
mkdir -p modules-rpi-${KERNEL_VERSION}-bassowl/lib/modules/${KERNEL_VERSION}+/kernel/sound/soc/codecs/
mkdir -p modules-rpi-${KERNEL_VERSION}-bassowl/lib/modules/${KERNEL_VERSION}-v7+/kernel/sound/soc/codecs/
mkdir -p modules-rpi-${KERNEL_VERSION}-bassowl/lib/modules/${KERNEL_VERSION}-v7l+/kernel/sound/soc/codecs/
cp linux-${KERNEL_VERSION}+/arch/arm/boot/dts/overlays/bassowl.dtbo modules-rpi-${KERNEL_VERSION}-bassowl/boot/overlays
cp linux-${KERNEL_VERSION}+/sound/soc/codecs/snd-soc-tas5825m.ko modules-rpi-${KERNEL_VERSION}-bassowl//lib/modules/${KERNEL_VERSION}+/kernel/sound/soc/codecs/
cp linux-${KERNEL_VERSION}-v7+/sound/soc/codecs/snd-soc-tas5825m.ko modules-rpi-${KERNEL_VERSION}-bassowl//lib/modules/${KERNEL_VERSION}-v7+/kernel/sound/soc/codecs/
cp linux-${KERNEL_VERSION}-v7l+/sound/soc/codecs/snd-soc-tas5825m.ko modules-rpi-${KERNEL_VERSION}-bassowl//lib/modules/${KERNEL_VERSION}-v7l+/kernel/sound/soc/codecs/
tar -czvf modules-rpi-${KERNEL_VERSION}-bassowl.tar.gz modules-rpi-${KERNEL_VERSION}-bassowl/ --owner=0 --group=0
md5sum modules-rpi-${KERNEL_VERSION}-bassowl.tar.gz > modules-rpi-${KERNEL_VERSION}-bassowl.md5sum.txt
sha1sum modules-rpi-${KERNEL_VERSION}-bassowl.tar.gz > modules-rpi-${KERNEL_VERSION}-bassowl.sha1sum.txt

echo "!!!  Done  !!!"
