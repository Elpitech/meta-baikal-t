KBRANCH ?= "linux-${@'rt-' if '${LINUX_KERNEL_TYPE}' == 'preempt-rt' else ''}${PV}.y-tp"

require recipes-kernel/linux/linux-yocto.inc
require recipes-kernel/linux/linux-baikal.inc
