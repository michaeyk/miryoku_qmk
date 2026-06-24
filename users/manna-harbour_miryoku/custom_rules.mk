# Copyright 2019 Manna Harbour
# https://github.com/manna-harbour/miryoku

MIRYOKU_NAV=VI
MIRYOKU_ALPHAS=QWERTY

# RP2040: lengthen the crystal oscillator startup delay so the clock is stable
# before USB init on cold boot. Must be a global -D compiler flag, not a #define
# in config.h: the pico-sdk file that reads it (lib/pico-sdk/src/rp2_common/
# hardware_xosc/xosc.c) never includes QMK's config.h, so a header #define is
# invisible to it. OPT_DEFS applies to every translation unit, including xosc.c.
# Addresses QMK issue #25362: when USB power arrives before the PC finishes
# booting, an unstable clock causes a hardfault in protocol_post_init() and the
# board isn't recognized until reset. The default 64 was too low here; a reporter
# took failures from ~30% to ~2% at 174. 174 is also the hardware maximum: the
# DELAY register field caps STARTUP_DELAY (= 47 * multiplier at 12 MHz) below
# 8192, so anything above 174 fails to compile. Lower it later if cold boot proves
# reliable.
OPT_DEFS += -DPICO_XOSC_STARTUP_DELAY_MULTIPLIER=174
