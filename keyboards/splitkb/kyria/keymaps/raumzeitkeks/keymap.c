/* Copyright 2019 Thomas Baart <thomas@splitkb.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include QMK_KEYBOARD_H

#include "keymap_german.h"

enum layers {
    _BASE = 0,
    _SYM1,
    _SYM2,
    _NUM,
    _FN,
    _SCR,
    _DEL,
    _VOL,
};

enum custom_keycodes {
    TAPPING_TERM_INC = SAFE_RANGE,
    TAPPING_TERM_DEC,
    TAPPING_TERM_OUT,
    COMBO_TERM_INC,
    COMBO_TERM_DEC,
    COMBO_TERM_OUT,
};

uint16_t g_tapping_term = TAPPING_TERM;

uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    return g_tapping_term;
}

uint16_t g_combo_term = COMBO_TERM;

uint16_t get_combo_term(uint16_t index, combo_t *combo) {
    return g_combo_term;
}

void send_term(uint16_t term, uint8_t id1, uint8_t id2) {
    tap_code(KC_SPACE);
    tap_code(id1);
    tap_code(id2);
    const char* str = get_u16_str(term, ' ');
    for (; *str != '\0'; ++str) {
        if ('0' <= *str && *str <= '9') {
            const uint8_t lut[] = { DE_0, DE_1, DE_2, DE_3, DE_4, DE_5, DE_6, DE_7, DE_8, DE_9 };
            tap_code(lut[*str - '0']);
        }
    }
}

bool process_term(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        switch (keycode) {
            case TAPPING_TERM_INC: {
                g_tapping_term += 10;
                return false;
            }
            case TAPPING_TERM_DEC: {
                g_tapping_term -= 10;
                return false;
            }
            case TAPPING_TERM_OUT: {
                send_term(g_tapping_term, DE_T, DE_T);
                return false;
            }
            case COMBO_TERM_INC: {
                g_combo_term += 10;
                return false;
            }
            case COMBO_TERM_DEC: {
                g_combo_term -= 10;
                return false;
            }
            case COMBO_TERM_OUT: {
                send_term(g_combo_term, DE_C, DE_T);
                return false;
            }
        }
    }
    return true;
}

bool process_shortcut(uint16_t keycode, keyrecord_t *record) {
    const uint8_t mods = get_mods();
    const uint8_t weak_mods = get_mods();
    const uint8_t oneshot_mods = get_mods();
    const bool is_shift_pressed = ((mods | weak_mods | oneshot_mods) & MOD_MASK_SHIFT) != 0;

    uint16_t shortcut_keycode = KC_NO;
    switch (QK_MODS_GET_BASIC_KEYCODE(keycode)) {
        case KC_CUT: {
            shortcut_keycode = C(DE_X);
            break;
        }
        case KC_COPY: {
            shortcut_keycode = C(DE_C);
            break;
        }
        case KC_PSTE: {
            shortcut_keycode = C(DE_V);
            break;
        }
        case KC_UNDO: {
            shortcut_keycode = is_shift_pressed ? C(DE_Y) : C(DE_Z);
            break;
        }
    }

    // Continue default handling if this is not a shortcut key.
    if (shortcut_keycode == KC_NO) {
        return true;
    }

    if (record->event.pressed) {
        clear_oneshot_mods();
        clear_weak_mods();
        clear_mods();

        tap_code16(shortcut_keycode);

        set_mods(mods);
        set_weak_mods(weak_mods);
        set_oneshot_mods(oneshot_mods);
    }

    return false;
}

bool is_dead_key(uint16_t keycode) {
    switch (keycode) {
        case DE_CIRC: return true;
        case DE_ACUT: return true;
        case DE_GRV:  return true;
    }
    return false;
}

bool process_record_user(uint16_t keycode, keyrecord_t* record) {
    // Continue default handling if this is a tap-hold being held.
    if ((IS_QK_MOD_TAP(keycode) || IS_QK_LAYER_TAP(keycode)) && record->tap.count == 0) {
        return true;
    }

    static uint16_t registered_dead_keycode = KC_NO;

    // Dead keys must always be released and finalized before the next event is handled.
    if (registered_dead_keycode != KC_NO) {
        unregister_code16(registered_dead_keycode);
        registered_dead_keycode = KC_NO;
        tap_code16(KC_SPACE);
    }

    if (record->event.pressed && is_dead_key(keycode)) {
        register_code16(keycode);
        registered_dead_keycode = keycode;
        return false;
    }

    return (
        process_term(keycode, record) &&
        process_shortcut(keycode, record)
    );
}

bool caps_word_press_user(uint16_t keycode) {
    switch (keycode) {
        case DE_A ... DE_Z:
        case DE_ADIA:
        case DE_ODIA:
        case DE_UDIA:
        case DE_SS: {
            add_weak_mods(MOD_BIT_LSHIFT);
            return true;
        }

        case KC_1 ... KC_0:
        case DE_MINS:
        case DE_UNDS:
        case KC_DEL:
        case KC_BSPC:
        case KC_LEFT:
        case KC_RIGHT:
        case LCTL(KC_LEFT):
        case LCTL(KC_RIGHT):
        case RCTL(KC_LEFT):
        case RCTL(KC_RIGHT):
        case LSFT(KC_LEFT):
        case LSFT(KC_RIGHT):
        case RSFT(KC_LEFT):
        case RSFT(KC_RIGHT):
        case LSFT(LCTL(KC_LEFT)):
        case LSFT(LCTL(KC_RIGHT)):
        case RSFT(RCTL(KC_LEFT)):
        case RSFT(RCTL(KC_RIGHT)): {
            return true;
        }
    }
    return false;
}

#define MO_SYM1 MO(_SYM1)
#define MO_SYM2 MO(_SYM2)
#define MO_NUM  MO(_NUM)
#define MO_FN   MO(_FN)
#define MO_SCR  MO(_SCR)
#define MO_DEL  MO(_DEL)
#define MO_VOL  MO(_VOL)

#define SYM1(kc) LT(_SYM1, kc)
#define SYM2(kc) LT(_SYM2, kc)
#define NUM(kc)  LT(_NUM, kc)
#define FN(kc)   LT(_FN, kc)
#define SCR(kc)  LT(_SCR, kc)
#define DEL(kc)  LT(_DEL, kc)
#define VOL(kc)  LT(_VOL, kc)

#define LG(kc) LGUI_T(kc)
#define LC(kc) LCTL_T(kc)
#define LS(kc) LSFT_T(kc)
#define LA(kc) LALT_T(kc)
#define RG(kc) RGUI_T(kc)
#define RC(kc) RCTL_T(kc)
#define RS(kc) RSFT_T(kc)

#define __XXX__ KC_NO

#define TTI TAPPING_TERM_INC
#define TTD TAPPING_TERM_DEC
#define TTO TAPPING_TERM_OUT
#define CTI TAPPING_TERM_INC
#define CTD TAPPING_TERM_DEC
#define CTO TAPPING_TERM_OUT

#define DENSE_LAYOUT(L30, L31, L32, L33, L34, L35, R35, R34, R33, R32, R31, R30, L20, L21, L22, L23, L24, L25, R25, R24, R23, R22, R21, R20, L11, L12, L13, L14, L15, R15, R14, R13, R12, R11, L01, L02, L03, L04, R04, R03, R02, R01) \
    LAYOUT( L30, L31, L32, L33, L34, L35,                     R35, R34, R33, R32, R31, R30, \
            L20, L21, L22, L23, L24, L25,                     R25, R24, R23, R22, R21, R20, \
            TTO, L11, L12, L13, L14, L15, TTD, TTI, CTI, CTD, R15, R14, R13, R12, R11, CTO, \
                           TTO, L01, L02, L03, L04, R04, R03, R02, R01, CTO )

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = DENSE_LAYOUT(
                 KC_ESC, DE_V,     DE_W,     DEL(DE_M),  VOL(DE_G),  DE_K,              DE_J,    VOL(DE_DOT), DEL(DE_COMM), DE_QUES,  DE_B,     DE_QUOT,
                 DE_Z,   LS(DE_S), LC(DE_C), SYM2(DE_N), SYM1(DE_T), KC_TAB,            KC_BSPC, NUM(DE_A),   FN(DE_E),     RC(DE_I), RS(DE_H), DE_Q,
                         DE_X,     LS(DE_P), LC(DE_L),   SCR(DE_D),  KC_INS,            CW_TOGG, SCR(DE_U),   RC(DE_O),     LS(DE_Y), DE_F,
                                   MO_FN,    MO_NUM,     LA(DE_R),   KC_ENT,            KC_LGUI, LA(KC_SPC),  MO_SYM1,      MO_SYM2
    ),
    [_SYM1] = DENSE_LAYOUT(
                       KC_ESC,  __XXX__, __XXX__, __XXX__, __XXX__, __XXX__,            DE_TILD, DE_COLN, DE_DLR,  DE_EXLM, DE_CIRC, DE_DQUO,
                       __XXX__, KC_LCTL, KC_LALT, KC_DOWN, MO_SYM1, KC_TAB,             KC_BSPC, DE_LPRN, DE_RPRN, DE_LCBR, DE_RCBR, DE_AT,
                                __XXX__, __XXX__, KC_UP,   __XXX__, __XXX__,            __XXX__, DE_SLSH, DE_ASTR, DE_PLUS, DE_PERC,
                                         __XXX__, __XXX__, KC_LALT, KC_ENT,             KC_LGUI, DE_MINS, MO_SYM1, __XXX__
    ),
    [_SYM2] = DENSE_LAYOUT(
                       KC_ESC,  __XXX__, __XXX__, __XXX__, KC_HOME,  KC_END,            __XXX__, DE_SCLN, DE_HASH, DE_EQL,  DE_DEG,  DE_GRV,
                       __XXX__, KC_LCTL, KC_LALT, MO_SYM2, KC_RIGHT, KC_TAB,            KC_BSPC, DE_LBRC, DE_RBRC, DE_LABK, DE_RABK, DE_EURO,
                                __XXX__, __XXX__, __XXX__, KC_LEFT,  __XXX__,           __XXX__, DE_BSLS, DE_PIPE, DE_AMPR, __XXX__,
                                         __XXX__, __XXX__, KC_LALT,  KC_ENT,            KC_LGUI, DE_UNDS, __XXX__, MO_SYM2
    ),
    [_NUM] = DENSE_LAYOUT(
                       KC_ESC,  __XXX__,  DE_7,    DE_8,    DE_9,   __XXX__,            __XXX__, __XXX__, __XXX__, __XXX__, __XXX__, __XXX__,
                       __XXX__, LS(DE_0), DE_1,    DE_2,    DE_3,   KC_TAB,             KC_BSPC, MO_NUM,  KC_DOWN, KC_RCTL, KC_RSFT, __XXX__,
                                __XXX__,  DE_4,    DE_5,    DE_6,   KC_INS,             __XXX__, __XXX__, KC_UP,   __XXX__, __XXX__,
                                          __XXX__, MO_NUM,  DE_DOT, KC_ENT,             KC_LGUI, KC_LALT, __XXX__, __XXX__
    ),
    [_FN] = DENSE_LAYOUT(
                    KC_ESC,  KC_F12,     KC_F7,   KC_F8,   KC_F9,   __XXX__,            KC_HOME, KC_END,   __XXX__, __XXX__, __XXX__, __XXX__,
                    __XXX__, LS(KC_F10), KC_F1,   KC_F2,   KC_F3,   __XXX__,            KC_BSPC, KC_RIGHT, MO_FN,   KC_RCTL, KC_RSFT, __XXX__,
                             KC_F11,     KC_F4,   KC_F5,   KC_F6,   __XXX__,            __XXX__, KC_LEFT,  __XXX__, __XXX__, __XXX__,
                                         MO_FN,   __XXX__, __XXX__, KC_ENT,             KC_LGUI, KC_LALT,  __XXX__, __XXX__
    ),
    [_SCR] = DENSE_LAYOUT(
                       KC_ESC,  __XXX__, __XXX__, __XXX__, __XXX__, __XXX__,            __XXX__, __XXX__, __XXX__, __XXX__, __XXX__, __XXX__,
                       __XXX__, KC_LCTL, KC_LALT, KC_PGDN, __XXX__, KC_TAB,             KC_BSPC, __XXX__, KC_PGDN, KC_RCTL, KC_RSFT, __XXX__,
                                __XXX__, __XXX__, KC_PGUP, MO_SCR,  __XXX__,            __XXX__, MO_SCR,  KC_PGUP, __XXX__, __XXX__,
                                         __XXX__, __XXX__, KC_LALT, KC_ENT,             KC_LGUI, KC_LALT, __XXX__, __XXX__
    ),
    [_DEL] = DENSE_LAYOUT(
                       KC_ESC,  __XXX__, __XXX__, MO_DEL,  KC_BSPC, KC_DEL,             KC_DEL,  KC_BSPC, MO_DEL,  __XXX__, __XXX__, __XXX__,
                       __XXX__, KC_LCTL, KC_LALT, __XXX__, __XXX__, KC_TAB,             KC_BSPC, __XXX__, __XXX__, KC_RCTL, KC_RSFT, __XXX__,
                                __XXX__, __XXX__, __XXX__, __XXX__, __XXX__,            __XXX__, __XXX__, __XXX__, __XXX__, __XXX__,
                                         __XXX__, __XXX__, KC_LALT, KC_ENT,             KC_LGUI, KC_LALT, __XXX__, __XXX__
    ),
    [_VOL] = DENSE_LAYOUT(
                       KC_ESC,  __XXX__, __XXX__, KC_MUTE, MO_VOL,  __XXX__,            __XXX__, MO_VOL,  KC_MUTE, __XXX__, __XXX__, __XXX__,
                       __XXX__, KC_LCTL, KC_LALT, KC_VOLD, __XXX__, KC_TAB,             KC_BSPC, __XXX__, KC_VOLD, KC_RCTL, KC_RSFT, __XXX__,
                                __XXX__, __XXX__, KC_VOLU, __XXX__, __XXX__,            __XXX__, __XXX__, KC_VOLU, __XXX__, __XXX__,
                                         __XXX__, __XXX__, KC_LALT, KC_ENT,             KC_LGUI, KC_LALT, __XXX__, __XXX__
    ),
};

const uint16_t PROGMEM combo_umlaut_ae[] = {NUM(DE_A), FN(DE_E),  COMBO_END};
const uint16_t PROGMEM combo_umlaut_oe[] = {SCR(DE_U), FN(DE_E),  COMBO_END};
const uint16_t PROGMEM combo_umlaut_ue[] = {RC(DE_O),  NUM(DE_A), COMBO_END};
const uint16_t PROGMEM combo_umlaut_ss[] = {LS(DE_S),  DE_Z,      COMBO_END};

combo_t key_combos[] = {
    COMBO(combo_umlaut_ae, DE_ADIA),
    COMBO(combo_umlaut_oe, DE_ODIA),
    COMBO(combo_umlaut_ue, DE_UDIA),
    COMBO(combo_umlaut_ss, DE_SS),
};

bool get_combo_must_tap(uint16_t index, combo_t *combo) {
    return true;
}
