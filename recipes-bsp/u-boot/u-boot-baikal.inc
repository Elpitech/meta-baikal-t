
KBRANCH = "u-boot-${PV}-tp"
SRC_URI = "git://192.168.1.253/IT/T-Platforms/sdk/u-boot.git;protocol=ssh;user=git;branch=${KBRANCH}"
SRCREV = "${AUTOREV}"
S = "${WORKDIR}/git"
B = "${WORKDIR}/build"
