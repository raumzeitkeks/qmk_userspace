/* Copyright 2022 Thomas Baart <thomas@splitkb.com>
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

#pragma once

#define CAPS_WORD_INVERT_ON_SHIFT
#define CAPS_WORD_IDLE_TIMEOUT 0  // disable timeout

#define TAPPING_TERM_PER_KEY  // enable get_tapping_term() function
#define QUICK_TAP_TERM 0  // disable double-tap auto-repeat

#define COMBO_TERM_PER_COMBO  // enable get_combo_term() function
#define COMBO_MUST_TAP_PER_COMBO  // enable get_combo_must_tap() function
#define EXTRA_SHORT_COMBOS  // limit combos to six keys to save memory
