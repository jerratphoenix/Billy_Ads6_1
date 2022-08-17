FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI += " \
        file://0001-Common-Add-header-file-to-enable-ASD-function.patch \
        file://0016-Add-ASPEED-SGPIO-driver.patch \
        "

SRC_URI += " \
        file://0001-media-aspeed-refine-to-avoid-full-jpeg-update.patch        \
        file://0002-media-aspeed-add-debugfs.patch                             \
        file://0003-media-aspeed-fix-mode-detect-always-timeout-at-2nd-r.patch \
        file://0004-media-aspeed-Add-version-to-runtime-configuration.patch    \
        file://0005-media-aspeed-move-err-handling-together-to-the-botto.patch \
        file://0006-media-aspeed-use-v4l2_info-v4l2_warn-v4l2_dbg-for-lo.patch \
        file://0007-media-aspeed-add-more-debug-log-messages.patch             \
        file://0008-media-aspeed-add-comments-and-macro.patch                  \
        file://0009-media-aspeed-Update-signal-status-immediately-to-ens.patch \
        file://0010-media-aspeed-Add-macro-for-the-fields-of-the-mode-de.patch \
        file://0011-media-aspeed-Fix-ast2500-dram-hang-at-res-change.patch     \
        file://0012-media-aspeed-refactor-to-gather-format-compress-sett.patch \
        file://0013-media-aspeed-Support-aspeed-mode-to-reduce-compresse.patch \
        file://0014-media-aspeed-Extend-debug-message.patch                    \
        file://0015-media-aspeed-Fix-no-complete-irq-for-non-64-aligned-.patch \
        file://0016-media-aspeed-Correct-the-reg-for-h-total-pixels.patch      \
        file://0017-media-aspeed-Use-FIELD_GET-to-improve-readability.patch    \
        file://0018-media-aspeed-Correct-values-for-detected-timing.patch      \
        file://0019-media-aspeed-Fix-timing-polarity-incorrect.patch           \
        file://0020-media-aspeed-sync-timing-upstream-patch.patch              \
        file://0021-media-aspeed-Fix-driver-stop-working-if-user-qbuf-la.patch \
        file://0022-medai-aspeed-Fix-using-sync-mode-if-refresh-rate-cha.patch \
        file://0023-video-aspeed-Fix-unstable-timing-detection.patch           \
        file://0024-media-aspeed-Fix-kanic-nobody-cared-irq.patch              \
        file://0025-media-aspeed-Fix-video-stopped-if-changing-refresh-r.patch \
        file://0026-media-aspeed-Refine-timing-update.patch                    \
        file://0027-Add-video-reset-patches-to-fix-probe-fail-in-video-d.patch \
        "
