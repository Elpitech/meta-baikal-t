SUMMARY = "Linux kernel"
DESCRIPTION = "Custom Linux kernel source code specifically patched for Baikal-T based devices"
SECTION = "kernel"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=d7810fab7487fb0aad327b76f1be7cd7"

# We don't use kernel-yocto class since it's overwhelmed with kmeta functions,
# which we don't indend to use. Additionally we set fitImage class to be supported
# by the recipe, so to use it in recovery-image creation procedure.
KERNEL_IMAGETYPE = "fitImage"
KERNEL_CLASSES = " kernel-fitimage "

inherit kernel

DEPENDS += "xz-native openssl-native"

EXTERNALSRC = "${EXTERNAL_KERNEL_SRC}"
SRC_URI = "git://192.168.1.253/IT/T-Platforms/sdk/kernel.git;protocol=ssh;user=git;branch=${KBRANCH}"
SRCREV = "${AUTOREV}"
S = "${WORKDIR}/git"
B = "${WORKDIR}/build"

# It must be ${PV} branch by default. The specific revision shall be extracted from
# the sources by kernel.bbclass, so shut the version sanity check up
KERNEL_REALTIME ?= "0"
KBRANCH ?= "linux-${@'rt-' if '${KERNEL_REALTIME}' == '1' else ''}${PV}.y-tp"
LINUX_VERSION ?= "${PV}"
KERNEL_VERSION_SANITY_SKIP = "1"

# We use the kernel default config selected by the <machine>.conf file
KERNEL_CONFIG_COMMAND = "oe_runmake -C ${S} O=${B} ${KBUILD_DEFCONFIG} silentoldconfig"

# Yeah, you may want to clean up some sections, but in additional it will delete ALL the
# symbols, so you won't be able to build kernel modules again!
# DON'T USE THIS VARIABLE! IT BREAKS KERNEL BUILD PROCESS!
#############KERNEL_IMAGE_STRIP_EXTRA_SECTIONS  = ".comment"###############

# The kernel source code shall be at least compatible with baikal SoC and qemu MIPS-targets
COMPATIBLE_MACHINE = "(baikal|qemumips)"

# Skip processing of this recipe if it is not explicitly specified as the
# PREFERRED_PROVIDER for virtual/kernel. This avoids network access required
# by the use of AUTOREV SRCREVs, which are the default for this recipe.
python () {
    if d.getVar("PREFERRED_PROVIDER_virtual/kernel") != d.getVar("PN"):
        d.delVar("BB_DONT_CACHE")
        raise bb.parse.SkipPackage("Set PREFERRED_PROVIDER_virtual/kernel to %s to enable it" % (d.getVar("PN")))
}

FIT_IMAGE_BASE_NAME ??= "${PKGE}-${PKGV}-${PKGR}-${MACHINE}-${DATETIME}"
FIT_IMAGE_SYMLINK_NAME ??= "${MACHINE}"
FIT_IMAGE_BASE_NAME[vardepsexclude] += "DATETIME"
DTB_IMAGE_BASE_NAME ??= "${PKGE}-${PKGV}-${PKGR}-${MACHINE}-${DATETIME}"
DTB_IMAGE_SYMLINK_NAME ??= "${MACHINE}"
DTB_IMAGE_BASE_NAME[vardepsexclude] += "DATETIME"
MODULE_TARBALL_BASE_NAME[vardepsexclude] += "DATETIME"

# Modify kernel config if real-time option is selected
do_configure_append() {
    if [ "${KERNEL_REALTIME}" == "1" ]; then
        sed -ie "s/^# CONFIG_PREEMPT_RT_FULL is not set/CONFIG_PREEMPT_RT_FULL=y/g" ${B}/.config
        sed -ie "s/^# CONFIG_HIGH_RES_TIMERS is not set/CONFIG_HIGH_RES_TIMERS=y/g" ${B}/.config
    fi
}

# Create rsa-key if u-boot signature is enabled
python do_create_keys() {
    if d.getVar("UBOOT_SIGN_ENABLE", True) != "1" or d.getVar("UBOOT_SIGN_KEYDIR", True):
        return

    keydir = os.path.join(d.getVar("WORKDIR", True), "keys")
    keysuf = os.path.join(keydir, "dev")
    priv = keysuf + ".key"
    cert = keysuf + ".crt"
    pub = keysuf + ".pub"

    try:
        bb.utils.mkdirhier(keydir)
        bb.process.run(["openssl", "genpkey", "-algorithm", "RSA", "-out", priv, "-pkeyopt", "rsa_keygen_bits:2048", "-pkeyopt", "rsa_keygen_pubexp:65537"])
        bb.process.run(["openssl", "req", "-batch", "-new", "-x509", "-key", priv, "-out", cert])
        pubkey, _ = bb.process.run(["openssl", "rsa", "-in", priv, "-pubout"])
        with open(pub, 'w') as f:
            f.write(pubkey.split("\n", 1)[1])
    except bb.process.ExecutionError:
        bb.error("Failed to create rsa-keys")
}

addtask do_create_keys after do_compile before do_assemble_fitimage

# Override the fitImage an DTB installation appended methods. We don't really need so
# many files and links deployed. We'll create only unique and necessary minimum ones.
do_deploy_append() {
    # Remove kernel image symlinks and replace non-fitImages with proper one
    for type in ${KERNEL_IMAGETYPES} ; do

        bbdebug 2 "Discard messy ${type} symbolic links..."
        rm -f ${DEPLOYDIR}/${type}
        rm -f ${DEPLOYDIR}/${type}-${KERNEL_IMAGE_SYMLINK_NAME}.bin

        if [ ${type} == "fitImage" ]; then
            continue
        fi

        # Extract extention and type of the image
        ext=${type##*.}
        typeextless=${type%.*}
        if [ ${ext} != ${type} ]; then
            ext=".${ext}"
        else
            ext=""
        fi

        bbdebug 2 "Copying ${typeextless}-${KERNEL_IMAGE_BASE_NAME}${ext} source files..."
        mv ${DEPLOYDIR}/${type}-${KERNEL_IMAGE_BASE_NAME}.bin ${DEPLOYDIR}/${typeextless}-${KERNEL_IMAGE_BASE_NAME}${ext}
        ln -sf ${typeextless}-${KERNEL_IMAGE_BASE_NAME}${ext} ${DEPLOYDIR}/${typeextless}-${KERNEL_IMAGE_SYMLINK_NAME}${ext}
    done

    # Fix deployed fitImage files and symbolic links
    if echo ${KERNEL_IMAGETYPES} | grep -wq "fitImage"; then
        cd ${B}
        bbdebug 2 "Discard messy fitImage*.its source files..."
        rm -f ${DEPLOYDIR}/${its_base_name}.its
        rm -f ${DEPLOYDIR}/${its_symlink_name}.its
        rm -f ${DEPLOYDIR}/${linux_bin_base_name}.bin
        rm -f ${DEPLOYDIR}/${linux_bin_symlink_name}.bin

        bbdebug 2 "Copying fitImage.{its,} source files..."
        install -m 0644 fit-image.its ${DEPLOYDIR}/fitImage-${FIT_IMAGE_BASE_NAME}.its
        ln -sf fitImage-${FIT_IMAGE_BASE_NAME}.its ${DEPLOYDIR}/fitImage-${FIT_IMAGE_SYMLINK_NAME}.its
        # No need to copy this since kernel.bbclass will do.
        # install -m 0644 arch/${ARCH}/boot/fitImage ${DEPLOYDIR}/fitImage-${FIT_IMAGE_BASE_NAME}.bin
        # ln -sf fitImage-${FIT_IMAGE_BASE_NAME}.bin ${DEPLOYDIR}/fitImage-${FIT_IMAGE_SYMLINK_NAME}.bin

        if [ -n "${INITRAMFS_IMAGE}" ]; then
            bbdebug 2 "Discard messy fitImage-initramfs*.{its,bin} source files..."
            rm -f ${DEPLOYDIR}/${its_initramfs_base_name}.its
            rm -f ${DEPLOYDIR}/${its_initramfs_symlink_name}.its
            rm -f ${DEPLOYDIR}/${fit_initramfs_base_name}.bin
            rm -f ${DEPLOYDIR}/${fit_initramfs_symlink_name}.bin

            bbdebug 2 "Copying fitImage-initramfs.{its,bin} source files..."
            install -m 0644 fit-image-${INITRAMFS_IMAGE}.its ${DEPLOYDIR}/fitImage-initramfs-${FIT_IMAGE_BASE_NAME}.its
            ln -sf fitImage-initramfs-${FIT_IMAGE_BASE_NAME}.its ${DEPLOYDIR}/fitImage-initramfs-${FIT_IMAGE_SYMLINK_NAME}.its
            install -m 0644 arch/${ARCH}/boot/fitImage-${INITRAMFS_IMAGE} ${DEPLOYDIR}/fitImage-initramfs-${FIT_IMAGE_BASE_NAME}.bin
            ln -sf fitImage-initramfs-${FIT_IMAGE_BASE_NAME}.bin ${DEPLOYDIR}/fitImage-initramfs-${FIT_IMAGE_SYMLINK_NAME}.bin
        fi

        # Deploy signature keys
        if [ "${UBOOT_SIGN_ENABLE}" == "1" -a -z "${UBOOT_SIGN_KEYDIR}" ]; then
            bbdebug 2 "Copying U-boot signature keys..."

            keysuf="${WORKDIR}/keys/dev"

            install -m 0644 ${keysuf}.key ${DEPLOYDIR}/sign-${FIT_IMAGE_BASE_NAME}.key
            ln -sf sign-${FIT_IMAGE_BASE_NAME}.key ${DEPLOYDIR}/sign-${FIT_IMAGE_SYMLINK_NAME}.key
            install -m 0644 ${keysuf}.crt ${DEPLOYDIR}/sign-${FIT_IMAGE_BASE_NAME}.crt
            ln -sf sign-${FIT_IMAGE_BASE_NAME}.crt ${DEPLOYDIR}/sign-${FIT_IMAGE_SYMLINK_NAME}.crt
            install -m 0644 ${keysuf}.pub ${DEPLOYDIR}/sign-${FIT_IMAGE_BASE_NAME}.pub
            ln -sf sign-${FIT_IMAGE_BASE_NAME}.pub ${DEPLOYDIR}/sign-${FIT_IMAGE_SYMLINK_NAME}.pub
        fi
    fi

    # Fix deployed dtb files and symlinks
    for DTB in ${KERNEL_DEVICETREE}; do
        DTB=`normalize_dtb "${DTB}"`
        DTB_EXT=${DTB##*.}
        DTB_BASE_NAME=`basename ${DTB} ."${DTB_EXT}"`
        DTB_PATH=`get_real_dtb_path_in_kernel "${DTB}"`
        for type in ${KERNEL_IMAGETYPE_FOR_MAKE}; do
            base_name=${type}"-"${KERNEL_IMAGE_BASE_NAME}
            symlink_name=${type}"-"${KERNEL_IMAGE_SYMLINK_NAME}
            DTB_NAME=`echo ${base_name} | sed "s/${MACHINE}/${DTB_BASE_NAME}/g"`
            DTB_SYMLINK_NAME=`echo ${symlink_name} | sed "s/${MACHINE}/${DTB_BASE_NAME}/g"`

            bbdebug 2 "Discard messy ${DTB_BASE_NAME}.${DTB_EXT} files"
            rm -f ${DEPLOYDIR}/${DTB_NAME}.${DTB_EXT}
            rm -f ${DEPLOYDIR}/${DTB_SYMLINK_NAME}.${DTB_EXT}
            rm -f ${DEPLOYDIR}/${DTB_BASE_NAME}.${DTB_EXT}
        done

        bbdebug 2 "Copying ${DTB_BASE_NAME}.${DTB_EXT} files"
        install -m 0644 ${DTB_PATH} ${DEPLOYDIR}/${DTB_BASE_NAME}-${DTB_IMAGE_BASE_NAME}.${DTB_EXT}
        if ! echo ${DTB_BASE_NAME} | grep -q ${DTB_IMAGE_SYMLINK_NAME}; then
            ln -sf ${DTB_BASE_NAME}-${DTB_IMAGE_BASE_NAME}.${DTB_EXT} ${DEPLOYDIR}/${DTB_BASE_NAME}-${DTB_IMAGE_SYMLINK_NAME}.${DTB_EXT}
        else
            ln -sf ${DTB_BASE_NAME}-${DTB_IMAGE_BASE_NAME}.${DTB_EXT} ${DEPLOYDIR}/${DTB_BASE_NAME}.${DTB_EXT}
        fi
    done
}

addtask kernel_version_sanity_check after do_kernel_checkout before do_compile
