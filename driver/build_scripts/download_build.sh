#!/bin/bash

CPU=4
KERNEL_VERSION="5.10.90"

case $KERNEL_VERSION in
    "5.10.90")
      KERNEL_COMMIT="9a09c1dcd4fae55422085ab6a87cc650e68c4181"
      PATCH="bassowl-5.10.y.patch"
      ;;
    "5.10.73")
      KERNEL_COMMIT="1597995e94e7ba3cd8866d249e6df1cf9a790e49"
      PATCH="bassowl-5.10.y.patch"
      ;;
    "5.10.59")
      KERNEL_COMMIT="cb70bb6d7dd3327c539dd90090c1ec88006dbcef"
      PATCH="bassowl-5.10.y.patch"
      ;;
    "5.10.3")
      KERNEL_COMMIT="da59cb1161dc7c75727ec5c7636f632c52170961"
      PATCH="bassowl-5.10.x.patch"
      ;;
    "5.4.83")
      KERNEL_COMMIT="b7c8ef64ea24435519f05c38a2238658908c038e"
      PATCH="bassowl-5.4.x.patch"
      ;;
    "5.4.81")
      KERNEL_COMMIT="453e49bdd87325369b462b40e809d5f3187df21d"
      PATCH="bassowl-5.4.x.patch"
      ;;
    "5.4.79")
      KERNEL_COMMIT="0642816ed05d31fb37fc8fbbba9e1774b475113f"
      PATCH="bassowl-5.4.x.patch"
      ;;
    "5.4.72")
      KERNEL_COMMIT="b3b238cf1e64d0cc272732e77ae6002c75184495"
      PATCH="bassowl-5.4.x.patch"
      ;;
    "5.4.59")
      KERNEL_COMMIT="caf7070cd6cece7e810e6f2661fc65899c58e297"
      PATCH="bassowl-5.4.x.patch"
      ;;
    "5.4.51")
      KERNEL_COMMIT="8382ece2b30be0beb87cac7f3b36824f194d01e9"
      PATCH="bassowl-5.4.x.patch"
      ;;
    "4.19.118")
      KERNEL_COMMIT="e1050e94821a70b2e4c72b318d6c6c968552e9a2"
      PATCH="bassowl-4.19.x.patch"
      ;;
    "4.14.92")
      KERNEL_COMMIT="6aec73ed5547e09bea3e20aa2803343872c254b6"
      PATCH="bassowl-4.14.x.patch"
      ;;
esac

echo "!!!  Build modules for kernel ${KERNEL_VERSION}  !!!"
echo "!!!  Download kernel hash info  !!!"
wget -N https://raw.githubusercontent.com/raspberrypi/rpi-firmware/${KERNEL_COMMIT}/git_hash
GIT_HASH="$(cat git_hash)"
rm git_hash

echo "!!!  Download kernel source  !!!"
wget https://github.com/raspberrypi/linux/archive/${GIT_HASH}.tar.gz

echo "!!!  Download kernel patch  !!!"
wget -N https://raw.githubusercontent.com/Darmur/bassowl-hat/master/driver/patch_rpi/${PATCH}

echo "!!!  Extract kernel source  !!!"
rm -rf linux-${KERNEL_VERSION}+/
tar xvzf ${GIT_HASH}.tar.gz
rm ${GIT_HASH}.tar.gz
mv linux-${GIT_HASH}/ linux-${KERNEL_VERSION}+/

echo "!!!  Create git repo and apply patch  !!!"
cd linux-${KERNEL_VERSION}+/
git init
git add --all
git commit -m "extracted files"
cp ../${PATCH} ${PATCH}
git apply ${PATCH}
git status
cd ..

echo "!!!  Copy source files for other variants  !!!"
rm -rf linux-${KERNEL_VERSION}-v7+/
rm -rf linux-${KERNEL_VERSION}-v7l+/
cp -r linux-${KERNEL_VERSION}+/ linux-${KERNEL_VERSION}-v7+/
cp -r linux-${KERNEL_VERSION}+/ linux-${KERNEL_VERSION}-v7l+/

echo "!!!  Build RPi0 kernel and modules  !!!"
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
rm -rf modules-rpi-${KERNEL_VERSION}-bassowl/
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
