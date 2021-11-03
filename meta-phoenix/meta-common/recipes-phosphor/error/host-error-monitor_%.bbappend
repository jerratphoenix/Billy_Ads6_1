FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"
PROJECT_SRC_DIR := "${THISDIR}/${PN}"

SRC_URI = "git://git@github.com/pteceng/ServerBMC-Gold-host-error-monitor;protocol=ssh;branch=phoenixip"
SRCREV = "3abe775e661cbfede68f8eac0a1eca4f77f7cafb"

SRC_URI += "file://0000-Fix-ASIO-Build.patch \
	   "

