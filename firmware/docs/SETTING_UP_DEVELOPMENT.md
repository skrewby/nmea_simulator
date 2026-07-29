# Setting up the project

## Pre Requisites

### ARM GNU Embedded Toolchain

Download the toolchain from [ARM's website](https://developer.arm.com/Tools%20and%20Software/GNU%20Toolchain). As an example, for Ubuntu 64-bit, the link under "x86_64 Linux hosted cross toolchains" with the target of `AArch32 bare-metal target (arm-none-eabi)` was chosen (get the link from the site as the one below might be outdated):

```sh
wget https://developer.arm.com/-/media/Files/downloads/gnu/14.2.rel1/binrel/arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi.tar.xz
```

Extract the folder with

```sh
tar -xf arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi.tar.xz
```

Rename it to something nicer

```sh
mv arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi arm-gnu
```

Add the folder's `bin` folder to `$PATH`

```sh
export PATH="$PATH:$HOME/.local/bin/arm-gnu/bin"
```

### OpenOCD

Ubuntu

```sh
sudo apt install openocd
```

## Serial

In Ubuntu, I use [tio](https://github.com/tio/tio).

```sh
sudo apt install tio
```

After connecting the STM32 to the PC via USB (if on WSL you will need to passthrough to the VM using the details in the WSL section), run dmesg

```sh
sudo dmesg | grep tty
[sudo] password for skrewby:
[  323.832022] cdc_acm 1-1:1.2: ttyACM0: USB ACM device
[  338.814389] cdc_acm 1-1:1.2: ttyACM0: USB ACM device
```

Then connect to this device with tio

```sh
tio -b 115200 /dev/ttyACM0
```

If on Windows, Putty is fine.

## WSL

If developing in WSL, you need to passthrough the STM32 to it so you can communicate with it. Microsoft provides details for this [here](https://learn.microsoft.com/en-us/windows/wsl/connect-usb#attach-a-usb-device). To flash the device, you must attach the device to WSL, flash it and the deattach so Windows can recognize it.

## Pre-commit

Install [Pre-Commit](https://pre-commit.com/) or [Prek](https://prek.j178.dev/installation/) and follow the instructions in the site including running the relevant line below

```sh
pre-commit install

prek install
```

In the project root directory.

## Common Errors

### `libusb_open()` failed with LIBUSB_ERROR_ACCESS

View the device's group (in this case it's `dialout`)

```sh
ls -l /dev/ttyACM0
crw-rw---- root dialout 0 B Fri May 16 14:22:25 2025  /dev/ttyACM0
```

Run `lsusb` and find the vendor and product ID for the connected micro controller (in this case vendor ID is 0483 and product ID is 374b)

```sh
❯ lsusb
Bus 001 Device 001: ID 1d6b:0002 Linux Foundation 2.0 root hub
Bus 001 Device 003: ID 0483:374b STMicroelectronics ST-LINK/V2.1
Bus 002 Device 001: ID 1d6b:0003 Linux Foundation 3.0 root hub
```

Create a file named `/etc/udev/rules.d/99-usb-permissions.rules` if it does not already exists and add the following line to it (with the IDs and device group dependant on what was returned in the previous commands)

```sh
SUBSYSTEM=="usb", ATTRS{0483}=="0403", ATTRS{374b}=="6001", MODE="0666", GROUP="dialout"
```

Reload the rules

```sh
sudo udevadm control --reload-rules
sudo udevadm trigger
```

If the error still appears, add your user to the dialout group

```sh
sudo usermod -a -G dialout $USER
```
