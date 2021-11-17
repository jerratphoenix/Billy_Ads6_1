RDEPENDS:${PN}-extras:append:wolfpass = " \
	webui-vue \
	phosphor-hostlogger \
	phosphor-host-postd \
	phosphor-post-code-manager \
	"

RDEPENDS:${PN}-extras:append:brightoncity = " \
        phosphor-misc-usb-ctrl \
        usb-network \
        "
RDEPENDS:${PN}-inventory:append:brightoncity = " \
        virtual/obmc-gpio-monitor \
        "

RDEPENDS:${PN}-extras:append:archercity = ""
