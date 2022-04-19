# Copyright (C) 2018, T-platforms, Inc.  All Rights Reserved
# Released under the GPLv2 license (see packages/COPYING)

# This class provides and interface to generate an environment file for
# U-boot use. At the final stage the file is handled by mkenvimage
# utility supplied by U-boot source tree.

#
# End result is:
#
# 1. Text file with U-boot environment description
# 2. Binary blob compiled from the text file by U-boot native mkenvimage

#
# External variables
# ${UBOOT_ENV_SIZE} - maximum size of the environmetn binary
# ${UBOOT_ENV_BUILD_IMAGE}  - image name saved in the environment build_image variable
# ${UBOOT_ENV_BUILD_VERSION} - image version saved to the build_version variable
# ${UBOOT_ENV_CONSOLE} - kernel primary console
# ${UBOOT_ENV_BAUDRATE} - kernel primary console baudrate
# ${UBOOT_ENV_NUM_CORES} - number of CPU cores the kernel should start
# ${UBOOT_ENV_CPUFREQ} - CPU frequency
# ${UBOOT_ENV_HOSTNAME} - hostname provided within U-boot
# ${UBOOT_ENV_KERNEL_FILE_NAME} - kernel name to load over tftp
# ${UBOOT_ENV_INITRD_FILE_NAME} - initramfs name to load over tftp
# ${UBOOT_ENV_FDT_FILE_NAME} - fdt binary name to load over tftp
# ${UBOOT_ENV_KERNEL_ADDR_LD} - kernel address to load (can't be zero, since kernel isn't relocatable)
# ${UBOOT_ENV_INITRD_ADDR_LD} - initramfs address to load (can be zero)
# ${UBOOT_ENV_FDT_ADDR_LD} - fdt binary name to load (can be zero)
# ${UBOOT_ENV_FITIMAGE_ADDR_FW} - base address of the fitImage within the SoC address space
# ${UBOOT_ENV_FITIMAGE_CONFIG} - Config of Fit-image to boot (if empty default option from fit-image itself used)
# ${UBOOT_ENV_FITIMAGE_VERIFY} - verify sha/crc/etc of items loaded from FIT
# ${UBOOT_ENV_ETHADDR} - ethernet address of the first interface
# ${UBOOT_ENV_ETH1ADDR} - ethernet address of the second interface
# ${UBOOT_ENV_ETH2ADDR - ethernet address of the third interface}
# ${UBOOT_ENV_INITRD_HIGH} - initrd_high value - "no" for u-boot-2014 but "ffffffff" for 2019 version
# ${UBOOT_ENV_MTDID} - flash-device name to assign the partitions listed in mtdparts
# ${UBOOT_ENV_MTDPARTS} - list of mtd partitions available on SPI-boot flash
# ${UBOOT_ENV_BOOTCMD} - default kernel boot command
# ${UBOOT_ENV_BOOTDELAY} - kernel boot delay
# ${UBOOT_ENV_BOOTUNITS} - boot units in the format "unique_id=<Menu Text>;interface=<rootdev>;<params>;;"
# While rootdevice can be any supported by the kernel (like /dev/sda, /dev/ramX, initfs, etc), following interfaces
# are supported by this option:
# - rom - boot kernel from SPI-flash recovery image. The kernel will be extracted from fitImage
#         and supplied with dtb/initramfs from there
# - sata - load kernel from sata drive and execute it. Options: fs - filesystem of the device (fat,ext),
#          part - device:partition number (ex, 0:1), dir - kernel/initramfs/dtb files root directory
# - usb - load kernel from usb driver and execute it. Options are the same as for sata.
# - tftp - load kernel over tftp and start it. Options: ip - either dhcp to retrieve IP-address over dhcp
#          or real IP-address in full form, srvip - server IP-address to load the data, gwip - gateway
#          IP-address, mask - network mask, dnsip - DNS-server IP-address, dnsip2 - second DNS-server
#          IP-address.
# - fdt - FDT-name to select via bootmenu item.
# - reset - just reset the CPU
# - menu - select menu page (main or recovery)
# ${UBOOT_ENV_MENU_MAIN} - main menu items list
# ${UBOOT_ENV_MENU_RCVR} - recovery menu items list
# ${UBOOT_ENV_EXTRA_VARS} - extra variables printed to the env-file
# ${UBOOT_ENV_EXTRA_ARGS} - extra args to the kernel

DEPENDS:append = " u-boot-tools-native"

UBOOT_ENV_BASE_NAME ??= "u-boot-env-${PKGE}-${PKGV}-${PKGR}-${MACHINE}-${DATETIME}"
UBOOT_ENV_SYMLINK_NAME ??= "u-boot-env-${MACHINE}"
UBOOT_ENV_BASE_NAME[vardepsexclude] += "DATETIME"

UBOOT_ENV_SIZE ??= "0x00010000"
UBOOT_ENV_BUILD_IMAGE ??= "Yocto ${MACHINE} ${DISTRO} ${DISTRO_VERSION}"
UBOOT_ENV_BUILD_VERSION ??= "0.1"
UBOOT_ENV_CONSOLE ??= "ttyS0"
UBOOT_ENV_BAUDRATE ??= "115200"
UBOOT_ENV_NUM_CORES ??= "2"
UBOOT_ENV_CPUFREQ ??= "850"
UBOOT_ENV_HOSTNAME ??= "baikal"
UBOOT_ENV_VMLINUZ_FILE_NAME ??= "vmlinuz.bin"
UBOOT_ENV_VMLINUX_FILE_NAME ??= "vmlinux.bin"
UBOOT_ENV_INITRD_FILE_NAME ??= "initramfs.gz"
UBOOT_ENV_FDT_FILE_NAME ??= "machine.dtb"
UBOOT_ENV_VMLINUZ_ADDR_LD ??= "0x86100000"
UBOOT_ENV_VMLINUX_ADDR_LD ??= "0x80100000"
UBOOT_ENV_INITRD_ADDR_LD ??= "0x87000000"
UBOOT_ENV_FDT_ADDR_LD ??= "0x86000000"
UBOOT_ENV_FITIMAGE_ADDR_FW ??= "0x9C100000"
UBOOT_ENV_FITIMAGE_CONFIG ??= ""
UBOOT_ENV_FITIMAGE_VERIFY ??= "y"
UBOOT_ENV_ETHADDR ??= "7a:72:6c:4a:7a:ee"
UBOOT_ENV_ETH1ADDR ??= "7a:72:6c:4a:7b:ee"
UBOOT_ENV_ETH2ADDR ??= "7a:72:6c:4a:7c:ee"
UBOOT_ENV_INITRD_HIGH ??= "no"
UBOOT_ENV_MTDID ??= "boot_flash"
UBOOT_ENV_MTDPARTS ??= "bootloader;0x0;0x000E0000;ro;; \
			environment;0x000E0000;0x00010000;; \
			information;0x000F0000;0x00010000;ro;; \
			fitimage;0x00100000;0x00F00000;ro;; \
			firmware;0x0;0x01000000"
UBOOT_ENV_BOOTCMD ??= "run boot_rec_ram"
UBOOT_ENV_BOOTDELAY ??= "5"
UBOOT_ENV_BOOTUNITS ??= "rec_ram=1. Boot recovery kernel and RFS;rom=initfs;; \
			 rec_sda1=2. Boot recovery kernel to /dev/sda1;rom=/dev/sda1;; \
			 rec_sdb1=3. Boot recovery kernel to /dev/sdb1;rom=/dev/sdb1;; \
			 reset=Reset board;reset"
UBOOT_ENV_MENU_MAIN ??= "rec_ram;;rec_sda1;;rec_sdb1;;reset"
UBOOT_ENV_MENU_RCVR ??= ""
UBOOT_ENV_EXTRA_VARS ??= ""
UBOOT_ENV_EXTRA_ARGS ??= ""

#
# Emit the image build information
#
# $1 ... .env filename
uboot_env_emit_build_info() {
	cat << EOF >> ${1}
build_image=${UBOOT_ENV_BUILD_IMAGE}
build_version=${UBOOT_ENV_BUILD_VERSION}
build_date=${DATE}
build_target=${MACHINE}
EOF
}
uboot_env_emit_build_info[vardepsexclude] = "DATE"

#
# Emit the SoC parameters
#
# $1 ... .env filename
uboot_env_emit_soc_params() {
	cat << EOF >> ${1}
num_cores=${UBOOT_ENV_NUM_CORES}
cpufreq=${UBOOT_ENV_CPUFREQ}
EOF
}

#
# Emit the serial port parameters
#
# $1 ... .env filename
uboot_env_emit_serial_params() {
	cat << EOF >> ${1}
console=${UBOOT_ENV_CONSOLE}
baudrate=${UBOOT_ENV_BAUDRATE}
stderr=serial
stdin=serial
stdout=serial
EOF
}

#
# Emit the devices interface parameters
#
# $1 ... .env filename
uboot_env_emit_dev_params() {
	cat << EOF >> ${1}
autoload=no
hostname=${UBOOT_ENV_HOSTNAME}
ethact=eth0
ethaddr=${UBOOT_ENV_ETHADDR}
eth1addr=${UBOOT_ENV_ETH1ADDR}
eth2addr=${UBOOT_ENV_ETH2ADDR}
sataphy=100
EOF
}

#
# Emit the mtdids/mtdparts env variables
#
# $1 ... .env filename
uboot_env_emit_mtd_vars() {
	# UBOOT_ENV_MTDPARTS consists of mtd partitions separated by the double semicolon
	# patterns. We'll walk over all the items collecting the mtdparts variable.
	parts=$(echo "${UBOOT_ENV_MTDPARTS}" | sed 's/^\s*//g;s/\s*$//g')
	delim=","
	while [ -n "$parts" ]; do
		# Extract leading partitem and discard it from the bootmenu handling the
		# last case when ;; delimiter is absent
		partitem=$(echo "${parts%%;;*}" | sed 's/^\s*//g;s/\s*$//g')
		parts=$(echo "${parts#*;;}" | sed 's/^\s*//g;s/\s*$//g')
		[ "${partitem}" = "${parts}" ] && parts="" && delim=""

		# Detach name;offset;size[;ro] sections simultaniously checking whether
		# the mandatory sections aren't empty
		partname=$(echo "$partitem" | cut -d";" -f1 -s)
		partoff=$(echo "$partitem" | cut -d";" -f2 -s)
		partsize=$(echo "$partitem" | cut -d";" -f3 -s)
		if [ -z "${partname}" -o -z "${partoff}" -o -z "${partsize}" ]; then
			echo "MTD item name/offset/size is inconsistent ($partitem)"
			continue
		fi
		partro=$(echo "$partitem" | cut -d";" -f4 -s)

		# Collect mtdparts environment variable
		partsize=$(expr $(printf "%d" ${partsize}) / 1024)
		mtdparts="${mtdparts}${partsize}k@${partoff}(${partname})${partro}${delim}"
	done

	# Dump the mtdparts variable out to the env-file
	cat << EOF >> ${1}
partition=nor0,1
mtdids=nor0=${UBOOT_ENV_MTDID}
mtdparts=mtdparts=${UBOOT_ENV_MTDID}:${mtdparts}
EOF
}

#
# Emit the system boot files and addresses
#
# $1 ... .env filename
uboot_env_emit_boot_files() {
	cat << EOF >> ${1}
vmlinuz_addr_ld=${UBOOT_ENV_VMLINUZ_ADDR_LD}
vmlinuz_file_name=${UBOOT_ENV_VMLINUZ_FILE_NAME}
vmlinux_addr_ld=${UBOOT_ENV_VMLINUX_ADDR_LD}
vmlinux_file_name=${UBOOT_ENV_VMLINUX_FILE_NAME}
kernel_addr_ld=${UBOOT_ENV_VMLINUZ_ADDR_LD}
kernel_file_name=${UBOOT_ENV_VMLINUZ_FILE_NAME}
fdt_addr_ld=${UBOOT_ENV_FDT_ADDR_LD}
fdt_file_name=${UBOOT_ENV_FDT_FILE_NAME}
fdt_high=no
fdt_len=0x00040000
initrd_addr_ld=${UBOOT_ENV_INITRD_ADDR_LD}
initrd_file_name=${UBOOT_ENV_INITRD_FILE_NAME}
initrd_high=${UBOOT_ENV_INITRD_HIGH}
initrd_start=${UBOOT_ENV_INITRD_ADDR_LD}
initrd_len=0x01000000
multi_addr_fw=${UBOOT_ENV_FITIMAGE_ADDR_FW}
multi_conf=${@'#conf-${UBOOT_ENV_FITIMAGE_CONFIG}'.replace('/', '_') if '${UBOOT_ENV_FITIMAGE_CONFIG}' else ''}
verify=${UBOOT_ENV_FITIMAGE_VERIFY}
select_vmlinuz=echo Select vmlinuz.bin; setenv kernel_addr_ld \${vmlinuz_addr_ld}; setenv kernel_file_name \${vmlinuz_file_name}; setenv initrd_start \${initrd_addr_ld}
select_vmlinux=echo Select vmlinux.bin; setenv kernel_addr_ld \${vmlinux_addr_ld}; setenv kernel_file_name \${vmlinux_file_name}; setenv initrd_start \${initrd_addr_ld}
EOF
}

#
# Emit the kernel boot argument collectors
#
# $1 ... .env filename
uboot_env_emit_boot_args() {
	cat << EOF >> ${1}
addroot=setenv bootargs \${bootargs} root=\${root_dev} rw rootwait
addtty=setenv bootargs \${bootargs} console=\${console},\${baudrate}n8
addhw=setenv bootargs \${bootargs} nohtw ieee754=relaxed nocoherentio maxcpus=\${num_cores}
addmisc=setenv bootargs \${bootargs} earlyprintk=uart8250,mmio32,0x1F04A000,\${baudrate} ${UBOOT_ENV_EXTRA_ARGS}
collect_args=run addroot addtty addhw addmisc
start_static=bootnr \${kernel_addr_ld} \${initrd_addr_ld} \${fdt_addr_ld}
start_multi=bootm \${multi_addr_fw}\${multi_conf}
EOF
}

#
# Emit SPI-flash image boot commands
#
# $1 ... .env filename
# $2 ... name of the item
# $3 ... root device path
uboot_env_emit_rom_cmd() {

	# Make sure the root device is valid
	devp=$(echo ${3} | cut -c1-5)
        devi=$(echo ${3} | cut -c1-6)
	if [ "${devp}" != "/dev/" -a "${devi}" != "initfs" ]; then
		bberror "Invalid kernel root device ${3} in boot item ${2}"
		return 1
	fi

	cat << EOF >> ${1}
setroot_${2}=setenv root_dev ${3}
boot_${2}=run setroot_${2} collect_args start_multi
EOF
}

#
# Emit SATA/USB devices boot commands
#
# $1 ... .env filename
# $2 ... name of the item
# $3 ... sata/usb interface
# $4 ... root device path
# $5 ... interface configuration arguments
uboot_env_emit_dev_cmd() {
	local fs
	local part
	local dir
	local fsop
	local init_exec
	local fini_exec

	# Make sure the root device is valid
	devp=$(echo ${4} | cut -c1-5)
	devi=$(echo ${4} | cut -c1-6)
	if [ "${devp}" != "/dev/" -a "${devi}" != "initfs" ]; then
		bberror "Invalid kernel root device ${4} in boot item ${2}"
		return 1
	fi

	# Parse the supplied interface arguments
	for ifarg in $(echo ${5} | tr ';' '\n'); do
		argname=$(echo ${ifarg} | cut -d"=" -f1)
		argval=$(echo ${ifarg} | cut -d"=" -f2 -s)

		case ${argname} in
		fs)
			fs=${argval}
			;;
		part)
			part=${argval}
			;;
		dir)
			dir=${argval}
			;;
		*)
			bberror "Invalid argument ${argname} in boot item ${2}"
			return 1
			;;
		esac
	done

	# Select corresponding FS operation
	if [ "${fs}" = "ext" ]; then
		fsop=ext4load
	elif [ "${fs}" = "fat" ]; then
		fsop=fatload
	elif [ "${fs}" = "any" -o -z "${fs}" ]; then
		fsop=load
	else
		bberror "Invalid filesystem ${fs} in boot item ${2}"
		return 1
	fi

	# Parse device:partition pair
	case ${part} in
	[0-9]:[0-9])
		;;
	*)
		bberror "Invalid partition pair ${part} in boot item ${2}"
		return 1
	esac

	# Parse the root directory value
	if [ "$(echo ${dir} | cut -c1-1)" != "/" ]; then
		bberror "${dir} isn't full path dir in boot item ${2}"
		return 1
	fi

	# Select proper init and fini methods
	if [ "${3}" = "sata" ]; then
		init_exec="sata init"
		fini_exec="sata reset"
	else
		init_exec="usb start"
		fini_exec="usb stop"
	fi

	cat << EOF >> ${1}
init_${2}=${init_exec}; sleep 1;
load_kernel_${2}=echo Loading kernel: ${dir}/\${kernel_file_name}; ${fsop} ${3} ${part} \${kernel_addr_ld} ${dir}/\${kernel_file_name}
load_initrd_${2}=echo Loading ramdisk: ${dir}/\${initrd_file_name}; ${fsop} ${3} ${part} \${initrd_addr_ld} ${dir}/\${initrd_file_name}; setenv initrd_len \${filesize}
load_fdt_${2}=echo Loading FDT: ${dir}/\${fdt_file_name}; ${fsop} ${3} ${part} \${fdt_addr_ld} ${dir}/\${fdt_file_name}; setenv fdt_len \${filesize}; fdt addr \${fdt_addr_ld}
fini_${2}=${fini_exec}; sleep 1;
setroot_${2}=setenv root_dev ${4}
boot_${2}=run init_${2} select_vmlinuz load_kernel_${2} load_initrd_${2} load_fdt_${2} fini_${2} setroot_${2} collect_args start_static
EOF
}

#
# Primary IP addresses validation
#
# $1 ... ip address
uboot_env_validate_ips() {
	local cntr=0
	local ip=${1}

	for byte in $(echo ${ip} | tr '.' '\n'); do
		case ${byte} in
		[0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])
			;;
		*)
			return 1
		esac

		cntr=$(expr $cntr + 1)
	done

	if [ "$cntr" -ne "4" ]; then
		return 1
	fi

	return 0
}

#
# Emit network TFTP/NFS boot commands
#
# $1 ... .env filename
# $2 ... name of the item
# $3 ... tftp/nfs interface
# $4 ... root device path
# $5 ... interface configuration arguments
uboot_env_emit_net_cmd() {
	local ipaddr
	local serverip
	local gatewayip
	local netmask
	local dnsip
	local dnsip2
	local dir

	# Make sure the root device is valid
	devp=$(echo ${4} | cut -c1-5)
	devi=$(echo ${4} | cut -c1-6)
	if [ "${devp}" != "/dev/" -a "${devi}" != "initfs" ]; then
		bberror "Invalid kernel root device ${4} in boot item ${2}"
		return 1
	fi

	# Parse the supplied interface arguments
	for ifarg in $(echo ${5} | tr ';' '\n'); do
		argname=$(echo ${ifarg} | cut -d"=" -f1)
		argval=$(echo ${ifarg} | cut -d"=" -f2 -s)

		case ${argname} in
		ip)
			ipaddr=${argval}
			;;
		srvip)
			serverip=${argval}
			;;
		gwip)
			gatewayip=${argval}
			;;
		mask)
			netmask=${argval}
			;;
		dnsip)
			dnsip=${argval}
			;;
		dnsip2)
			dnsip2=${argval}
			;;
		dir)
			dir=${argval}
			;;
		*)
			bberror "Invalid argument ${argname} in boot item ${2}"
			return 1
			;;
		esac
	done

	# Make sure the mandatory arguments are defined
	if [ -z "${ipaddr}" ]; then
		bberror "ip is mandatory for network iface in boot item ${2}"
		return 1
	fi
	if [ -z "${serverip}" ]; then
		bberror "srvip is mandatory for network iface in boot item ${2}"
		return 1
	fi

	# Parse IP arguments if if they are static
	if [ "${ipaddr}" != "dhcp" ]; then
		# Set ip default values if ones omitted
		if [ -z "${gatewayip}" ]; then
			gatewayip=${serverip}
		fi

		if [ -z "${netmask}" ]; then
			netmask="255.255.255.0"
		fi

		if [ -z "${dnsip}" ]; then
			dnsip=${serverip}
		fi

		if [ -z "${dnsip2}" ]; then
			dnsip2=${serverip}
		fi

		# Validate passed IP addresses
		for ip in ${ipaddr} ${serverip} ${gatewayip} ${netmask} ${dnsip} ${dnsip2}; do
			if ! uboot_env_validate_ips $ip; then
				bberror "Invalid IP address $ip in boot item ${2}"
				return 1
			fi
		done
	elif ! uboot_env_validate_ips ${serverip}; then
		bberror "Invalid IP address $serverip in boot item ${2}"
		return 1
	fi

	# Parse the root directory value if nfs interface is requested
	if [ "${3}" = "nfs" ]; then
		if [ "$(echo ${dir} | cut -c1-1)" != "/" ]; then
			bberror "${dir} isn't full path dir in boot item ${2}"
			return 1
		fi
		dir=${dir}/
	fi

	# Set dhcp or static IPs at the interface init procedure
	if [ "$ipaddr" = "dhcp" -a -z "${serverip}" ]; then
		cat << EOF >> ${1}
init_${2}=dhcp
EOF
	elif [ "$ipaddr" = "dhcp" -a -n "${serverip}" ]; then
		cat << EOF >> ${1}
init_${2}=dhcp; setenv serverip ${serverip};
EOF
	else
		cat << EOF >> ${1}
init_${2}=setenv ipaddr ${ipaddr}; setenv serverip ${serverip}; setenv gatewayip ${gatewayip}; setenv netmask ${netmask}; setenv dnsip ${dnsip}; setenv dnsip2 ${dnsip2}
EOF
	fi

	cat << EOF >> ${1}
load_kernel_${2}=echo Loading kernel: ${dir}\${kernel_file_name}; ${3} \${kernel_addr_ld} ${dir}\${kernel_file_name}
load_initrd_${2}=echo Loading ramdisk: ${dir}\${initrd_file_name}; ${3} \${initrd_addr_ld} ${dir}\${initrd_file_name}; setenv initrd_len \${filesize}
load_fdt_${2}=echo Loading FDT: ${dir}\${fdt_file_name}; ${3} \${fdt_addr_ld} ${dir}\${fdt_file_name}; setenv fdt_len \${filesize}; fdt addr \${fdt_addr_ld}
setroot_${2}=setenv root_dev ${4}
boot_${2}=run init_${2} select_vmlinuz load_kernel_${2} load_initrd_${2} load_fdt_${2} setroot_${2} collect_args start_static
EOF
}

#
# Emit reset U-boot commands
#
# $1 ... .env filename
# $2 ... name of the item
uboot_env_emit_reset_cmd() {
	echo "boot_${2}=reset" >> ${1}
}

#
# Emit select-fdt command
#
# $1 ... .env filename
# $2 ... name of the item
# $3 ... fdt-name
uboot_env_emit_fdt_cmd() {
	cat << EOF >> ${1}
setfdt_file_${2}=setenv fdt_file_name ${3##*/}
setmulti_conf_${2}=setenv multi_conf \\\#conf-$(printf '%s' "${3}" | sed 's/\//_/')
boot_${2}=run setfdt_file_${2} setmulti_conf_${2}; bootmenu -1
EOF
}

#
# Emit menu page selector command
#
# $1 ... .env filename
# $2 ... name of the item
# $3 ... page name
uboot_env_emit_menu_cmd() {
	if [ "${3}" != "main" -a "${3}" != "recovery" ]; then
		bberror "Invalid command ${2} menu page name ${3}"
		return 1
	fi

	echo "boot_${2}=run menu_page_${3}" >> ${1}

	return 0
}

#
# Emit the kernel boot argument collectors
#
# $1 ... .env filename
uboot_env_emit_boot_cmd() {
	cat << EOF >> ${1}
bootcmd=${UBOOT_ENV_BOOTCMD}
bootdelay=${UBOOT_ENV_BOOTDELAY}
EOF

	# UBOOT_ENV_BOOTUNITS consists of boot items separated by the double semicolon pattern.
	# We'll walk over all the items calling the corresponding boot command handler.
	bootunits=$(echo "${UBOOT_ENV_BOOTUNITS}" | sed 's/^\s*//g;s/\s*$//g')
	bootmenu="${UBOOT_ENV_MENU_MAIN}"
	idx=0
	while [ -n "$bootunits" ]; do
		# Extract leading unit and discard it from the bootunits handling the
		# last case when ;; delimiter is absent
		item=$(echo "${bootunits%%;;*}" | sed 's/^\s*//g;s/\s*$//g')
		bootunits=$(echo "${bootunits#*;;}" | sed 's/^\s*//g;s/\s*$//g')
		[ "${item}" = "${bootunits}" ] && bootunits=""

		# Detach mandatory menu text string simultaniously checking whether the
		# menuitem consists of at least two section
		itemname=$(echo "$item" | cut -d";" -f1 -s | cut -d"=" -f1 -s)
		itemtext=$(echo "$item" | cut -d";" -f1 -s | cut -d"=" -f2 -s)
		if [ -z "${itemname}" -o -z "${itemtext}" ]; then
			echo "Boot item name/text is empty ($item)"
			continue
		fi
		iface=$(echo "$item" | cut -d";" -f2 | cut -d"=" -f1)
		rootdev=$(echo "$item" | cut -d";" -f2 | cut -d"=" -f2 -s)
		ifargs=$(echo "$item" | cut -d";" -f3-)

		# Create corresponing initialization commands sequence
		case ${iface} in
		rom)
			uboot_env_emit_rom_cmd "${1}" "${itemname}" "${rootdev}"
			;;
		sata|usb)
			uboot_env_emit_dev_cmd "${1}" "${itemname}" "${iface}" "${rootdev}" "${ifargs}"
			;;
		tftp|nfs)
			uboot_env_emit_net_cmd "${1}" "${itemname}" "${iface}" "${rootdev}" "${ifargs}"
			;;
		fdt)
			uboot_env_emit_fdt_cmd "${1}" "${itemname}" "${rootdev}"
			;;
		menu)
			uboot_env_emit_menu_cmd "${1}" "${itemname}" "${rootdev}"
			;;
		reset)
			uboot_env_emit_reset_cmd "${1}" "${itemname}"
			;;
		*)
			bberror "Unknown interface ${iface} in boot item (${item})"
			continue
			;;
		esac

		# Emit menu_item if command was emitted successfully
		if [ $? -eq 0 ]; then
			echo "menu_item_${itemname}=${itemtext}=run boot_${itemname}" >> ${1}
			# Create default bootmenu if it is a part of the main menu list
			if [ -z "${bootmenu##*${itemname}*}" ]; then
				echo "bootmenu_${idx}=${itemtext}=run boot_${itemname}" >> ${1}
				idx=$(expr $idx + 1)
			fi
		fi
	done
}

#
# Emit u-boot menu page collector
#
# $1 ... .env filename
# $2 ... page name
# $3 ... list of menu items
uboot_env_emit_menu_page() {
	# Create main menu selector
	menuitems=$(echo "${3}" | sed 's/^\s*//g;s/\s*$//g')
	menupage=""
	idx=0
	while [ -n "$menuitems" ]; do
		# Extract leading unit and discard it from the bootunits handling the
		# last case when ;; delimiter is absent
		item=$(echo "${menuitems%%;;*}" | sed 's/^\s*//g;s/\s*$//g')
		menuitems=$(echo "${menuitems#*;;}" | sed 's/^\s*//g;s/\s*$//g')
		[ "${item}" = "${menuitems}" ] && menuitems=""

		if [ -n "${menupage}" ]; then
			menupage="${menupage}; setenv bootmenu_${idx} \$menu_item_${item}"
		else
			menupage="setenv bootmenu_${idx} \$menu_item_${item}"
		fi

		idx=$(expr $idx + 1)
	done

	if [ ! -z "${menupage}" ]; then
		echo "menu_page_${2}=${menupage}; setenv bootmenu_${idx}; bootmenu -1" >> ${1}
	fi
}

#
# Emit u-boot menu page collector
#
# $1 ... .env filename
uboot_env_emit_menu() {
	cat << EOF >> ${1}
menucmd=bootmenu -1
EOF

	# Create main menu page selector
	uboot_env_emit_menu_page "${1}" "main" "${UBOOT_ENV_MENU_MAIN}"

	# Create recovery menu page selector
	uboot_env_emit_menu_page "${1}" "recovery" "${UBOOT_ENV_MENU_RCVR}"
}

#
# Emit u-boot extra variables defined by configs
#
# $1 ... .env filename
uboot_env_emit_extra() {
	printf "${UBOOT_ENV_EXTRA_VARS}\n" >> ${1}
}

#
# Assemble U-boot environment image
#
# $1 ... environment source filename
# $2 ... environment binary name
uboot_env_assemble() {

	bbdebug 2 "Creating U-boot environment image"

	# Create an empty file for environment variables declarations
	touch ${1} && > "${1}"

	# Create the image build information first in the file
	uboot_env_emit_build_info "${1}"

	# Set SoC performance basic configuration
	uboot_env_emit_soc_params "${1}"

	# Set serial console parameters
	uboot_env_emit_serial_params "${1}"

	# Set basic peripheral interfaces parameters
	uboot_env_emit_dev_params "${1}"

	# Set mtd-utility parameters
	uboot_env_emit_mtd_vars "${1}"

	# Write boot files and addresses settings
	uboot_env_emit_boot_files "${1}"

	# Create variables with common kernel boot arguments
	uboot_env_emit_boot_args "${1}"

	# Emit kernel boot commands and menu items
	uboot_env_emit_boot_cmd "${1}"

	# Emit boot menu
	uboot_env_emit_menu "${1}"

	# Emit extra variables if ones defined in configs
	uboot_env_emit_extra "${1}"

	# Filter out variables that are set to empty values
	grep -Ev '=$' ${1} > ${1}.tmp
	mv ${1}.tmp ${1}

	# Create U-boot environment binary
	uboot-mkenvimage -s ${UBOOT_ENV_SIZE} -o ${2} ${1}
}

do_assemble_uboot_env() {
	uboot_env_assemble "${WORKDIR}/u-boot-env.txt" "${WORKDIR}/u-boot-env.bin"
}
do_assemble_uboot_env[vardepsexclude] = "DATETIME DATE do_compile"
addtask assemble_uboot_env after do_compile before do_install do_deploy

do_install:append () {
	install -m 644 "${WORKDIR}/u-boot-env.txt" "${D}/boot/${UBOOT_ENV_BASE_NAME}.txt"
	install -m 644 "${WORKDIR}/u-boot-env.bin" "${D}/boot/${UBOOT_ENV_BASE_NAME}.bin"
	ln -sf "${UBOOT_ENV_BASE_NAME}.txt" "${D}/boot/${UBOOT_ENV_SYMLINK_NAME}.txt"
	ln -sf "${UBOOT_ENV_BASE_NAME}.bin" "${D}/boot/${UBOOT_ENV_SYMLINK_NAME}.bin"
}
do_install[vardepsexclude] += "DATETIME"

do_deploy:append() {
	install -d ${DEPLOYDIR}
	install -m 644 "${WORKDIR}/u-boot-env.txt" "${DEPLOYDIR}/${UBOOT_ENV_BASE_NAME}.txt"
	install -m 644 "${WORKDIR}/u-boot-env.bin" "${DEPLOYDIR}/${UBOOT_ENV_BASE_NAME}.bin"
	ln -sf "${UBOOT_ENV_BASE_NAME}.txt" "${DEPLOYDIR}/${UBOOT_ENV_SYMLINK_NAME}.txt"
	ln -sf "${UBOOT_ENV_BASE_NAME}.bin" "${DEPLOYDIR}/${UBOOT_ENV_SYMLINK_NAME}.bin"
}
do_deploy[vardepsexclude] += "DATETIME"
