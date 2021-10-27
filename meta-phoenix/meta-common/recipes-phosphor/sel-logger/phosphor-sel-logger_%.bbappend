PACKAGECONFIG:append:intel = " log-threshold"

FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"
PROJECT_SRC_DIR := "${THISDIR}/${PN}"

SRC_URI += "file://0000-Add-discrete-event-monitoring.patch \
            "

# Enable sensor event monitoring
EXTRA_OECMAKE += "-DSEL_LOGGER_MONITOR_THRESHOLD_EVENTS=ON"
EXTRA_OECMAKE += "-DSEL_LOGGER_MONITOR_DISCRETE_EVENTS=ON"
