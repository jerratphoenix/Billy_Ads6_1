FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://0001-Change-login-and-header-logo-to-Phoenix.patch \
            file://0002-Add-virtual-media-page-show-file-name-after-press-st.patch \
            file://0003-Improve-thermal-URI-to-dynamic-get-chassis-id.patch \ 
            file://0004-Improve-power-URI-to-dynamic-get-chassis-id.patch \
            file://0005-Fix-always-erroneous-pop-up-message-when-saving-stat.patch \
           "


