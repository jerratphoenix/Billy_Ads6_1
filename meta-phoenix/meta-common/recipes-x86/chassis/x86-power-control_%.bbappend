FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://0001-Fix-state-machine-off-state.patch \
            file://0002-Fix-power-restore-policy-and-correct-service-name-for-POH.patch \
            file://0003-Add-system-restart-cause-for-PEF.patch \
            file://0004-Reset-POH-counter-after-AC-cycle.patch \
            "

# Use PLT_RST to detect warm resets
EXTRA_OECMAKE:intel += "-DUSE_PLT_RST=ON"
