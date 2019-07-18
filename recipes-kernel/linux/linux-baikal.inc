SRCREV_machine ?= "AUTOINC"
SRCREV_meta ?= "AUTOINC"

EXTERNALSRC = "${EXTERNAL_KERNEL_SRC}"
TPSDK_REPO ?= "gitlab.tpl"

SRC_URI = "git://${TPSDK_REPO}/core/kernel.git;protocol=ssh;user=git;name=machine;branch=${KBRANCH}; \
           git://git.yoctoproject.org/yocto-kernel-cache;type=kmeta;name=meta;branch=yocto-${PV};destsuffix=${KMETA}"

DEPENDS += "openssl-native util-linux-native"

LINUX_VERSION ?= "${PV}"
KERNEL_VERSION_SANITY_SKIP = "1"

LINUX_VERSION_EXTENSION ?= "-${MACHINE}"

KCONFIG_MODE = "--alldefconfig"
KMETA = "kernel-meta"
KCONF_BSP_AUDIT_LEVEL = "2"

COMPATIBLE_MACHINE = "baikal|qemumips|qemumips64"

# Functionality flags
KERNEL_EXTRA_FEATURES ?= ""
KERNEL_FEATURES_append = " ${KERNEL_EXTRA_FEATURES}"

# Customized kernel names
FIT_IMAGE_BASE_NAME ??= "${PKGE}-${PKGV}-${PKGR}-${MACHINE}-${DATETIME}"
FIT_IMAGE_SYMLINK_NAME ??= "${MACHINE}"
FIT_IMAGE_BASE_NAME[vardepsexclude] += "DATETIME"
DTB_IMAGE_BASE_NAME ??= "${PKGE}-${PKGV}-${PKGR}-${MACHINE}-${DATETIME}"
DTB_IMAGE_SYMLINK_NAME ??= "${MACHINE}"
DTB_IMAGE_BASE_NAME[vardepsexclude] += "DATETIME"
MODULE_TARBALL_BASE_NAME[vardepsexclude] += "DATETIME"

FIT_IMAGE_GENKEY ?= "0"

python () {
    if d.getVar("UBOOT_SIGN_ENABLE", True) != "1" or d.getVar("UBOOT_SIGN_KEYDIR", True):
        return

    d.setVar("FIT_IMAGE_GENKEY", "1")

    d.setVar("UBOOT_SIGN_KEYDIR", os.path.join(d.getVar("WORKDIR", True), "keys"))
    if not d.getVar("UBOOT_SIGN_KEYNAME", True):
        d.setVar("UBOOT_SIGN_KEYNAME", "dev")
}

# Create rsa-key if u-boot signature is enabled
python do_create_keys() {
    if d.getVar("FIT_IMAGE_GENKEY", True) != "1":
        return

    keydir = d.getVar("UBOOT_SIGN_KEYDIR", True)
    keysuf = os.path.join(keydir, d.getVar("UBOOT_SIGN_KEYNAME", True))
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
addtask do_create_keys after do_compile before do_assemble_fitimage do_assemble_fitimage_initramfs

# Fix the deployed fitImage and DTB file names. We don't really need so
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
        if [ "${FIT_IMAGE_GENKEY}" == "1" ]; then
            bbdebug 2 "Copying U-boot signature keys..."

            keysuf="${WORKDIR}/keys/${UBOOT_SIGN_KEYNAME}"

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
        ln -sf ${DTB_BASE_NAME}-${DTB_IMAGE_BASE_NAME}.${DTB_EXT} ${DEPLOYDIR}/${DTB_BASE_NAME}.${DTB_EXT}
    done
}
