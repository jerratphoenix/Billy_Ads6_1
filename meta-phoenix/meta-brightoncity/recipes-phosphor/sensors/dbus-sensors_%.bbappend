FILESEXTRAPATHS:append := "${THISDIR}/${PN}:"

EXTRA_OECMAKE += "-DDISABLE_CPUERROR=ON"
SYSTEMD_SERVICE:${PN}:remove += " xyz.openbmc_project.cpuerrsensor.service"
