// Copyright 2019 Manna Harbour
// https://github.com/manna-harbour/miryoku

// This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>.

#pragma once

#define MASTER_LEFT          // left half is always master

// Press both Shift home-row mods together to turn on Caps Word. NOTE: QMK's
// built-in BOTH_SHIFTS_TURNS_ON_CAPS_WORD does NOT work with Miryoku, because
// Miryoku uses same-handed mods on both halves: the right index is LSFT_T(KC_J),
// not RSFT_T. Holding both shifts only ever sets the Left Shift bit, so
// get_mods() never equals MOD_MASK_SHIFT (LSFT|RSFT) and the feature can't fire.
// It's implemented manually in process_record_user() in manna-harbour_miryoku.c
// by counting held shift home-row mods. Tap a word-breaking key to turn it off.

// Resolve Shift home-row mods as hold when the other key is released first,
// instead of waiting for the full tapping term. Restricted to Shift via the
// per-key function in manna-harbour_miryoku.c so same-hand rolls (e.g. "fa")
// still tap; intentional capitals (release alpha before Shift) still hold.
#define PERMISSIVE_HOLD_PER_KEY
