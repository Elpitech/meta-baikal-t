DEFAULTTUNE = "p5600el"

# Baikal-T(1) is based on MIPS Warrior P-class P5600 core
require conf/machine/include/tune-p5600.inc

SOC_FAMILY = "baikal"
require conf/machine/include/soc-family.inc

# Each Baikal-T is equipped with serial, pci and usb controllers
MACHINE_FEATURES = "serial pci usbhost"
SERIAL_CONSOLE = "115200;ttyS0"
SERIAL_CONSOLES = "115200;ttyS0 115200;ttyS1"

# Default kernel code parameters
PREFERRED_PROVIDER_virtual/kernel ?= "linux-baikal"

# Default u-boot code parameters
PREFERRED_PROVIDER_virtual/bootloader ?= "u-boot-baikal"
UBOOT_LOADADDRESS ??= "0x80100000"
UBOOT_ENTRYSYMBOL ??= "kernel_entry"

# Set Baikal U-boot and it mk(env)image tools provider
PREFERRED_PROVIDER_u-boot ?= "u-boot-baikal"
PREFERRED_PROVIDER_u-boot-mkimage ?= "u-boot-baikal-mk-tools"
PREFERRED_PROVIDER_u-boot-mkimage-native ?= "u-boot-baikal-mk-tools-native"
PREFERRED_PROVIDER_u-boot-mkimage-nativesdk ?= "u-boot-baikal-mk-tools-nativesdk"
PREFERRED_PROVIDER_u-boot-fw-utils ?= "u-boot-baikal-fw-utils"

# Set default kernel/bootloader output binary names and symlinks
KERNEL_IMAGE_BASE_NAME ?= "${PKGV}-${PKGR}-${MACHINE}-${DATETIME}"
KERNEL_IMAGE_SYMLINK_NAME ?= "${MACHINE}"
MODULE_TARBALL_BASE_NAME ?= "modules-${PKGV}-${PKGR}-${MACHINE}-${DATETIME}.tgz"
MODULE_TARBALL_SYMLINK_NAME ?= "modules-${MACHINE}.tgz"
DTB_IMAGE_BASE_NAME ?= "${PKGV}-${PKGR}-${MACHINE}-${DATETIME}"
DTB_IMAGE_SYMLINK_NAME ?= "${MACHINE}"
FIT_IMAGE_BASE_NAME ?= "${PKGV}-${PKGR}-${MACHINE}-${DATETIME}"
FIT_IMAGE_SYMLINK_NAME ?= "${MACHINE}"
UBOOT_BASE_NAME ?= "${PKGV}-${PKGR}-${MACHINE}-${DATETIME}"
UBOOT_SYMLINK_NAME ?= "${MACHINE}"
UBOOT_DTB_BINARY ?= "u-boot-${UBOOT_SYMLINK_NAME}.dtb"
UBOOT_ENV_BASE_NAME ?= "u-boot-env-${PKGV}-${PKGR}-${MACHINE}-${DATETIME}"
UBOOT_ENV_SYMLINK_NAME ?= "u-boot-env-${MACHINE}"
