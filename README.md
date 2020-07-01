# BassOwl-HAT
Hardware-On-Top addon for Raspberry Pi / ASUS ThinkerBoard with TAS5825M stereo amplifier, 30W RMS per channel.

## Product description

![BassOwl-HAT](https://github.com/Darmur/bassowl-hat/blob/master/pictures/pict01.jpg)

BassOwl-HAT is an expansion board for Raspberry Pi and ASUS ThinkerBoard, designed for Hi-Fi music playback.

It is powered by the [TAS5825M](https://www.ti.com/product/TAS5825M) from Texas Instruments, a stereo Class-D Smart Audio amplifier able to deliver up to 30W RMS per channel.
BassOwl-HAT does not require any external device or amplifier to play music, two loudspeakers can be connected directly to the screw-terminal connectors.

BassOwl-HAT requires an external power supply with volgage between 12V and 24V to function properly. It is not necessary to connect additional power sources, because a dedicated 5V/3A DCDC converter will back-power the Raspberry Pi.

Two optional MEMS Microphone modules can be connected to the BassOwl-HAT, for adding voice-control capabilities to the system.

<br />
<br />

## Power Supply Selection

BassOwl-HAT can used with a broad range of power supply.

* Voltage: 12V minimum, 24V maximum
* Current: 3A minimum, 5A recommended
* Connector: Barrel Jack 5.5x2.1mm
* Polarity: positive

![polarity](https://itp.nyu.edu/physcomp/wp-content/uploads/power_supply_polarity_011.jpg)

For achieving higher output power, please select a power supply with higher voltage / current capabilities.

<br />
<br />

## How to use

Proper driver integration is still work-in-progress, as a partial workaround a generic dt-overlay and a script launched at startup can be used. BassFly-uHAT will work fine, but no Hardware volume-control will be available (Software volume control will work fine).

<br />

#### Install script for Volumio

SSH needs to be [enabled](https://volumio.github.io/docs/User_Manual/SSH.html).
Open a SSH session and type following commands:

* With 12V Power supply
```
wget https://raw.githubusercontent.com/Darmur/bassowl-hat/master/scripts/install_tas5825m_12V_volumio.sh
chmod a+x install_tas5825m_12V_volumio.sh
sudo ./install_tas5825m_12V_volumio.sh
```
* With 15V Power supply
```
wget https://raw.githubusercontent.com/Darmur/bassowl-hat/master/scripts/install_tas5825m_15V_volumio.sh
chmod a+x install_tas5825m_15V_volumio.sh
sudo ./install_tas5825m_15V_volumio.sh
```
* With 20V Power supply
```
wget https://raw.githubusercontent.com/Darmur/bassowl-hat/master/scripts/install_tas5825m_20V_volumio.sh
chmod a+x install_tas5825m_20V_volumio.sh
sudo ./install_tas5825m_20V_volumio.sh
```
* With 24V Power supply
```
wget https://raw.githubusercontent.com/Darmur/bassowl-hat/master/scripts/install_tas5825m_24V_volumio.sh
chmod a+x install_tas5825m_24V_volumio.sh
sudo ./install_tas5825m_24V_volumio.sh
```
Please select "Generic I2S DAC" under Volumio playback options, then reboot.

<br />

#### Install script for Raspbian/Raspios

Please type following commands (from terminal or SSH session):

* With 12V Power supply
```
wget https://raw.githubusercontent.com/Darmur/bassowl-hat/master/scripts/install_tas5825m_12V_raspios.sh
chmod a+x install_tas5825m_12V_raspios.sh
sudo ./install_tas5825m_12V_raspios.sh
```
* With 15V Power supply
```
wget https://raw.githubusercontent.com/Darmur/bassowl-hat/master/scripts/install_tas5825m_15V_raspios.sh
chmod a+x install_tas5825m_15V_raspios.sh
sudo ./install_tas5825m_15V_raspios.sh
```
* With 20V Power supply
```
wget https://raw.githubusercontent.com/Darmur/bassowl-hat/master/scripts/install_tas5825m_20V_raspios.sh
chmod a+x install_tas5825m_20V_raspios.sh
sudo ./install_tas5825m_20V_raspios.sh
```
* With 24V Power supply
```
wget https://raw.githubusercontent.com/Darmur/bassowl-hat/master/scripts/install_tas5825m_24V_raspios.sh
chmod a+x install_tas5825m_24V_raspios.sh
sudo ./install_tas5825m_24V_raspios.sh
```
After reboot BassFly-uHAT will be up and running
