FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

SRC_URI:append = " file://0001-Fix-the-path-of-Redfish.patch \
		   file://0002-Correct-Redfish-URL-for-BC-and-remove-Power-cap-setting.patch \
                 "
