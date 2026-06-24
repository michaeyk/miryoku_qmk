// Copyright 2022 Manna Harbour
// https://github.com/manna-harbour/miryoku

// This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>.

#include QMK_KEYBOARD_H

#include "manna-harbour_miryoku.h"


// Additional Features double tap guard

enum {
    U_TD_BOOT,
#define MIRYOKU_X(LAYER, STRING) U_TD_U_##LAYER,
MIRYOKU_LAYER_LIST
#undef MIRYOKU_X
};

void u_td_fn_boot(tap_dance_state_t *state, void *user_data) {
  if (state->count == 2) {
    reset_keyboard();
  }
}

#define MIRYOKU_X(LAYER, STRING) \
void u_td_fn_U_##LAYER(tap_dance_state_t *state, void *user_data) { \
  if (state->count == 2) { \
    default_layer_set((layer_state_t)1 << U_##LAYER); \
  } \
}
MIRYOKU_LAYER_LIST
#undef MIRYOKU_X

tap_dance_action_t tap_dance_actions[] = {
    [U_TD_BOOT] = ACTION_TAP_DANCE_FN(u_td_fn_boot),
#define MIRYOKU_X(LAYER, STRING) [U_TD_U_##LAYER] = ACTION_TAP_DANCE_FN(u_td_fn_U_##LAYER),
MIRYOKU_LAYER_LIST
#undef MIRYOKU_X
};


// keymap

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
#define MIRYOKU_X(LAYER, STRING) [U_##LAYER] = U_MACRO_VA_ARGS(MIRYOKU_LAYERMAPPING_##LAYER, MIRYOKU_LAYER_##LAYER),
MIRYOKU_LAYER_LIST
#undef MIRYOKU_X
};


// shift functions

bool get_permissive_hold(uint16_t keycode, keyrecord_t *record) {
  if (IS_QK_MOD_TAP(keycode)) {
    uint8_t mods = QK_MOD_TAP_GET_MODS(keycode);
    if (mods == MOD_LSFT || mods == MOD_RSFT) {
      return true;
    }
  }
  return false;
}

const key_override_t capsword_key_override = ko_make_basic(MOD_MASK_SHIFT, CW_TOGG, KC_CAPS);

const key_override_t **key_overrides = (const key_override_t *[]){
    &capsword_key_override,
    NULL
};


// Caps Word: keep the word alive for SCREAMING_SNAKE_CASE. Letters are shifted,
// minus becomes underscore (shift applied), digits/backspace/delete/underscore
// pass through without breaking the word. Anything else turns Caps Word off.

bool caps_word_press_user(uint16_t keycode) {
  switch (keycode) {
    // Continue Caps Word, with shift applied.
    case KC_A ... KC_Z:
    case KC_MINS:                    // '-' produces '_' while in Caps Word
      add_weak_mods(MOD_BIT(KC_LSFT));
      return true;

    // Continue Caps Word, without shifting.
    case KC_1 ... KC_0:
    case KC_BSPC:
    case KC_DEL:
    case KC_UNDS:
      return true;

    // A held Shift must NOT end Caps Word. When a Shift home-row mod is held
    // while Caps Word is active, process_caps_word() passes KC_LSFT/KC_RSFT
    // here (mod-taps are rewritten to their plain mod keycode). Without these
    // cases the default below returns false and immediately calls
    // caps_word_off() -- which is exactly what killed the both-shifts gesture:
    // caps_word_on() fired, then the still-held Shift turned it straight back
    // off on the same key event.
    case KC_LSFT:
    case KC_RSFT:
      return true;

    default:
      return false;                  // Deactivate Caps Word.
  }
}


// Both-shifts gesture: hold both Shift home-row mods to turn on Caps Word.
// QMK's built-in BOTH_SHIFTS_TURNS_ON_CAPS_WORD relies on get_mods() reaching
// MOD_MASK_SHIFT (LSFT | RSFT), but Miryoku's same-handed mods make the right
// index LSFT_T too, so both shifts only ever set the Left Shift bit. We instead
// count Shift home-row mods that resolve to a HOLD and switch Caps Word on when
// the second one is held.
//
// Only count mod-taps that resolved to a hold (record->tap.count == 0). Tapped
// mod-taps (tap.count >= 1) are ignored, otherwise quickly tapping both shifts
// would momentarily reach a count of 2 (both press events arrive before the
// release events) and turn Caps Word on by accident. Because of the tapping
// term, the gesture is: hold both index keys together until they resolve to
// hold (~TAPPING_TERM), and Caps Word switches on.

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  static uint8_t shifts_held = 0;
  if (IS_QK_MOD_TAP(keycode) && QK_MOD_TAP_GET_MODS(keycode) == MOD_LSFT
      && record->tap.count == 0) {
    if (record->event.pressed) {
      if (++shifts_held >= 2 && !is_caps_word_on()) {
        caps_word_on();
      }
    } else if (shifts_held) {
      shifts_held--;
    }
  }
  return true;
}


// thumb combos

#if defined (MIRYOKU_KLUDGE_THUMBCOMBOS)
const uint16_t PROGMEM thumbcombos_base_right[] = {LT(U_SYM, KC_ENT), LT(U_NUM, KC_BSPC), COMBO_END};
const uint16_t PROGMEM thumbcombos_base_left[] = {LT(U_NAV, KC_SPC), LT(U_MOUSE, KC_TAB), COMBO_END};
const uint16_t PROGMEM thumbcombos_nav[] = {KC_ENT, KC_BSPC, COMBO_END};
const uint16_t PROGMEM thumbcombos_mouse[] = {KC_BTN2, KC_BTN1, COMBO_END};
const uint16_t PROGMEM thumbcombos_media[] = {KC_MSTP, KC_MPLY, COMBO_END};
const uint16_t PROGMEM thumbcombos_num[] = {KC_0, KC_MINS, COMBO_END};
  #if defined (MIRYOKU_LAYERS_FLIP)
const uint16_t PROGMEM thumbcombos_sym[] = {KC_UNDS, KC_LPRN, COMBO_END};
  #else
const uint16_t PROGMEM thumbcombos_sym[] = {KC_RPRN, KC_UNDS, COMBO_END};
  #endif
const uint16_t PROGMEM thumbcombos_fun[] = {KC_SPC, KC_TAB, COMBO_END};
combo_t key_combos[COMBO_COUNT] = {
  COMBO(thumbcombos_base_right, LT(U_FUN, KC_DEL)),
  COMBO(thumbcombos_base_left, LT(U_MEDIA, KC_ESC)),
  COMBO(thumbcombos_nav, KC_DEL),
  COMBO(thumbcombos_mouse, KC_BTN3),
  COMBO(thumbcombos_media, KC_MUTE),
  COMBO(thumbcombos_num, KC_DOT),
  #if defined (MIRYOKU_LAYERS_FLIP)
  COMBO(thumbcombos_sym, KC_RPRN),
  #else
  COMBO(thumbcombos_sym, KC_LPRN),
  #endif
  COMBO(thumbcombos_fun, KC_APP)
};
#endif
