FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://0001-Extend-OEM-command-to-support-full-and-quiet-mode.patch \
            file://0002-Add-fan-control-mode-value-set-to-config-file.patch \
            file://0003-Enhance-Debug-Logging.patch \
			file://0004-Clean-Exit-For-Reload.patch \
            "

