FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"
PROJECT_SRC_DIR := "${THISDIR}/${PN}"

SRC_URI += " \
            file://hook_add_entropy_buffer.c \
            file://hook_add_entropy_buffer.h \
            file://0001-Adds-Hook-to-rng-tools.patch \
            file://0003-rngd_linux_c.patch \
            file://0004-rngd_linux_c.patch \
            file://0005-rngd_linux_c.patch \
           "

S = "${WORKDIR}/git"

do_patch:append() {
    bb.build.exec_func('do_override_hooks', d)
}

do_override_hooks () {
    if [ -r "${DEVTOOL_TEMPDIR}" ]; then
        cp ${DEVTOOL_TEMPDIR}/oe-local-files/hook_add_entropy_buffer.c \
        ${S}/hook_add_entropy_buffer.c
        cp ${DEVTOOL_TEMPDIR}/oe-local-files/hook_add_entropy_buffer.h \
        ${S}/hook_add_entropy_buffer.h
    fi

    if [ -r "${WORKDIR}/hook_add_entropy_buffer.c" ]; then
        cp ${WORKDIR}/hook_add_entropy_buffer.c \
            ${S}/hook_add_entropy_buffer.c
    fi

    if [ -r "${WORKDIR}/hook_add_entropy_buffer.h" ]; then
        cp ${WORKDIR}/hook_add_entropy_buffer.h \
            ${S}/hook_add_entropy_buffer.h
    fi
}
