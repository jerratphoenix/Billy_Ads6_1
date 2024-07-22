inherit obmc-phosphor-dbus-service
inherit obmc-phosphor-systemd

SUMMARY = "CPU Crashdump"
DESCRIPTION = "CPU utilities for dumping CPU Crashdump and registers over PECI"

DEPENDS = "boost cjson sdbusplus safec gtest libpeci"
inherit cmake

EXTRA_OECMAKE = "-DYOCTO_DEPENDENCIES=ON -DCRASHDUMP_BUILD_UT=OFF -DBHS_EGS_BUILD=OFF"

LICENSE = "CLOSED"

SRC_URI = "file://Intel-crashdump.tar.gz"

S = "${WORKDIR}/"

SYSTEMD_SERVICE:${PN} += "com.intel.crashdump.service"
DBUS_SERVICE:${PN} += "com.intel.crashdump.service"

# linux-libc-headers guides this way to include custom uapi headers
CFLAGS:append = " -I ${STAGING_KERNEL_DIR}/include/uapi"
CFLAGS:append = " -I ${STAGING_KERNEL_DIR}/include"
CXXFLAGS:append = " -I ${STAGING_KERNEL_DIR}/include/uapi"
CXXFLAGS:append = " -I ${STAGING_KERNEL_DIR}/include"
do_configure[depends] += "virtual/kernel:do_shared_workdir"
