DEFAULTTUNE = "p5600el"

# Baikal-T(1) is based on MIPS Warrior P-class P5600 core
require conf/machine/include/tune-p5600.inc

SOC_FAMILY = "baikal"
require conf/machine/include/soc-family.inc

# Each Baikal-T is equipped with serial, pci and usb controllers
MACHINE_FEATURES = "serial pci usbhost"
SERIAL_CONSOLE = "115200 ttyS0"

# External sources settings
INHERIT += "externalsrc"
EXTERNAL_KERNEL_SRC ?= ""
EXTERNAL_UBOOT_SRC ?= ""

# Default kernel code parameters
PREFERRED_VERSION_linux-yocto ?= "4.4%"
PREFERRED_PROVIDER_virtual/kernel ?= "linux-yocto"
LINUXLIBCVERSION ?= "${PREFERRED_VERSION_linux-yocto}"

PREFERRED_VERSION_u-boot ?= "2021.07"
PREFERRED_VERSION_u-boot-tools ?= "2021.07"
PREFERRED_RPROVIDER_u-boot-fw-utils = "libubootenv"
PREFERRED_VERSION_libubootenv ?= "0.3.2"
PREFERRED_PROVIDER_virtual/bootloader ?= "u-boot"
UBOOT_LOADADDRESS ??= "${UBOOT_VMLINUX_LOADADDRESS}"
UBOOT_ENTRYSYMBOL ??= "kernel_entry"
UBOOT_VMLINUZ_LOADADDRESS ??= "0x86100000"
UBOOT_VMLINUX_LOADADDRESS ??= "0x80100000"
UBOOT_FDT_LOADADDRESS ??= "0x86000000"
UBOOT_RD_LOADADDRESS ??= "0x84000000"

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
