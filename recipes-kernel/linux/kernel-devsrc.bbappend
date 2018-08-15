
# We need to set source directory pointing to the kernel external source
# directory in case if externalsrc is enabled
S = "${@'${EXTERNAL_KERNEL_SRC}' if '${EXTERNAL_KERNEL_SRC}' else '${STAGING_KERNEL_DIR}'}"
