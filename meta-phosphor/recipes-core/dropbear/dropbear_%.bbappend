# 0001-Only-load-dropbear-default-host-keys-if-a-key-is-not.patch
# has been upstreamed.  This patch can be removed once we upgrade
# to yocto 2.5 or later which will pull in the latest dropbear code.
FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"
SRC_URI += "file://dropbearkey.service \
            file://localoptions.h \
            file://dropbear.default \
			file://backup.sh \
           "

# pull in OpenSSH's /usr/libexec/sftp-server so we don't have to rely
# on the crufty old scp protocol for file transfer
RDEPENDS:${PN} += "openssh-sftp-server"
RDEPENDS_${PN} += "bash"

do_install:append() {
install -d ${D}${sysconfdir}/keystore
install -m 0777 ${WORKDIR}/backup.sh ${D}/${sysconfdir}/keystore/backup.sh
}
