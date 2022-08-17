PACKAGECONFIG:append = " log-threshold clears-sel"

FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"
PROJECT_SRC_DIR := "${THISDIR}/${PN}"

SRC_URI += "file://0000-Add-discrete-event-monitoring.patch \
            file://0001-Improve-sel-log-condition.patch \
            file://0002-Correct-WDT-sensor-object-path-and-remove-WDT-redfis.patch \
            file://0003-Improve-trigger-signal-for-add-sel-and-oem-sel.patch \
            "

# Enable sensor event monitoring
EXTRA_OECMAKE += "-DSEL_LOGGER_MONITOR_THRESHOLD_EVENTS=ON"
EXTRA_OECMAKE += "-DSEL_LOGGER_MONITOR_DISCRETE_EVENTS=ON"
