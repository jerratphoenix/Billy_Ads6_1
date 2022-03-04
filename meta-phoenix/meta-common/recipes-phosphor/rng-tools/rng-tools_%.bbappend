FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"
PROJECT_SRC_DIR := "${THISDIR}/${PN}"

SRC_URI += "file://0001-hook_add_entropy_buffer_h.patch \
            file://0001-hook_add_entropy_buffer_c.patch \
            file://0001-Adds-Hook-to-rng-tools.patch \
           "
