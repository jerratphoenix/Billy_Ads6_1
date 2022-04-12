FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI += "file://0001-Change-login-and-header-logo-to-Phoenix.patch \
            file://0002-Add-virtual-media-page-show-file-name-after-press-st.patch \
            file://0003-Improve-thermal-URI-to-dynamic-get-chassis-id.patch \ 
            file://0004-Improve-power-URI-to-dynamic-get-chassis-id.patch \
            file://0005-Fix-always-erroneous-pop-up-message-when-saving-stat.patch \
            file://0006-Add-ipv6-in-Network-info.patch \
            file://0007-Remove-repetitive-quick-label-on-the-Inventory-and-L.patch \
            file://0008-Common-webui-vue-Fix-dynamically-assigned-DNS-IP-add.patch \
            file://0009-Common-webui-vue-Add-DNS-information-in-Network-page.patch \
           "


