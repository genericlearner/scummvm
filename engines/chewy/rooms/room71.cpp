/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "chewy/defines.h"
#include "chewy/events.h"
#include "chewy/global.h"
#include "chewy/room.h"
#include "chewy/rooms/room71.h"

namespace Chewy {
namespace Rooms {

void Room71::entry(int16 eib_nr) {
	det->enable_sound(0, 0);
	det->enable_sound(0, 1);
	det->play_sound(0, 0);
	det->play_sound(0, 1);
	_G(spieler).ScrollxStep = 2;
	spieler_mi[P_HOWARD].Mode = true;
	spieler_mi[P_NICHELLE].Mode = true;
	_G(spieler).ZoomXy[P_HOWARD][0] = 8;
	_G(spieler).ZoomXy[P_HOWARD][1] = 16;
	_G(spieler).ZoomXy[P_NICHELLE][0] = 8;
	_G(spieler).ZoomXy[P_NICHELLE][1] = 16;
	_G(zoom_horizont) = 0;
	_G(spieler).r71_word18DB16 = 0;
	_G(spieler).r71_word18DB18 = 0;
	SetUpScreenFunc = setup_func;
	_G(spieler).DiaAMov = 4;
	if (!_G(spieler).flags28_2 && !_G(spieler).flags28_4) {
		_G(timer_nr)[0] = room->set_timer(0, 10);
		det->set_static_ani(0, -1);
	} else if (_G(spieler).flags28_2) {
		det->start_detail(2, 255, false);
		det->show_static_spr(8);
		det->hide_static_spr(2);
		det->hide_static_spr(3);
	} else if (_G(spieler).flags28_4) {
		det->show_static_spr(5);
		det->hide_static_spr(3);
	}

	if (flags.LoadGame)
		return;

	switch (eib_nr) {
	case 110:
		set_person_pos(500, 67, P_CHEWY, P_LEFT);
		set_person_pos(568, 31, P_NICHELLE, P_LEFT);
		set_person_pos(538, 35, P_HOWARD, P_LEFT);
		break;
	case 114:
		set_person_pos(70, 48, P_CHEWY, P_RIGHT);
		set_person_pos(165, 16, P_NICHELLE, P_RIGHT);
		set_person_pos(120, 18, P_HOWARD, P_RIGHT);
		break;
	default:
		break;
	}
}

void Room71::xit(int16 eib_nr) {
	_G(spieler).ScrollxStep = 1;
	if (_G(spieler).PersonRoomNr[P_HOWARD] != 71)
		return;

	switch (eib_nr) {
	case 105:
		_G(spieler).PersonRoomNr[P_HOWARD] = 74;
		_G(spieler).PersonRoomNr[P_NICHELLE] = 74;
		break;
	case 106:
		_G(spieler).PersonRoomNr[P_HOWARD] = 73;
		_G(spieler).PersonRoomNr[P_NICHELLE] = 73;
	default:
		break;
	}
}

void Room71::setup_func() {
	calc_person_look();

	if (_G(spieler).r71_word18DB16) {
		if (_G(spieler).r71_word18DB18)
			--_G(spieler).r71_word18DB18;
		else {
			_G(spieler).r71_word18DB18 = (_G(spieler).DelaySpeed + 1) / 2;
			if (_G(spieler).r71_word18DB16 == 1) {
				det->set_static_pos(3, 244 + g_engine->_rnd.getRandomNumber(4), -1 * g_engine->_rnd.getRandomNumber(4), false, false);				
			} else {
				det->set_static_pos(5, 198 + g_engine->_rnd.getRandomNumber(3), -1 * g_engine->_rnd.getRandomNumber(3), false, false);
			}
		}
	} else {
		const int posX = spieler_vector[P_CHEWY].Xypos[0];
		int howDestX, howDestY, nicDestX, nicDestY;
		if (posX < 40) {
			howDestX = 82;
			howDestY = 18;
			nicDestX = 127;
			nicDestY = 19;
		} else if (posX < 190) {
			howDestX = 229;
			howDestY = 22;
			nicDestX = 268;
			nicDestY = 22;
		} else if (posX < 290) {
			howDestX = 329;
			howDestY = 26;
			nicDestX = 368;
			nicDestY = 29;
		} else if (posX < 420) {
			howDestX = 434;
			howDestY = 36;
			nicDestX = 477;
			nicDestY = 32;
		} else {
			howDestX = 538;
			howDestY = 35;
			nicDestX = 568;
			nicDestY = 31;
		}

		go_auto_xy(howDestX, howDestY, P_HOWARD, ANI_GO);
		go_auto_xy(nicDestX, nicDestY, P_NICHELLE, ANI_GO);
	}
}

int Room71::proc1() {
	if (_G(spieler).inv_cur) {
		flags.NoScroll = false;
		return 0;
	}

	hide_cur();
	auto_move(2, P_CHEWY);
	flags.NoScroll = true;
	auto_scroll(256, 0);
	_G(spieler).r71_word18DB18 = 0;
	if (menu_item == CUR_HOWARD) {
		proc4();
		if (_G(spieler).flags28_4) {
			if (_G(spieler).R71Val2)
				proc2();
			else
				_G(spieler).R71Val1 = 1;
		}
	} else if (menu_item == CUR_NICHELLE) {
		proc3();
		if (_G(spieler).flags28_4) {
			if (_G(spieler).R71Val1)
				proc2();
			else
				_G(spieler).R71Val2 = 1;
		}
	} else {
		_G(spieler).PersonHide[P_CHEWY] = true;
		proc5(3);
		_G(spieler).PersonHide[P_CHEWY] = false;
	}

	if (!_G(spieler).flags28_2)
		start_aad_wait(432, -1);

	show_cur();
	flags.NoScroll = false;
	return 1;
}

void Room71::proc2() {
	start_aad_wait(432, -1);
	start_aad_wait(433, -1);
	SetUpScreenFunc = nullptr;
	go_auto_xy(518, 35, P_HOWARD, ANI_GO);
	go_auto_xy(568, 36, P_NICHELLE, ANI_VOR);
	SetUpScreenFunc = setup_func;
	flags.NoScroll = true;
	auto_scroll(284, 0);
	_G(spieler).PersonHide[P_CHEWY] = true;
	_G(spieler).PersonHide[P_NICHELLE] = true;
	_G(spieler).PersonHide[P_HOWARD] = true;
	proc5(5);
	det->hide_static_spr(2);
	det->start_detail(5, 255, false);
	auto_scroll(160, 0);
	det->hide_static_spr(5);
	start_detail_wait(1, 1, ANI_VOR);
	det->start_detail(2, 255, false);
	det->stop_detail(5);
	det->show_static_spr(8);
	atds->set_steuer_bit(442, ATS_AKTIV_BIT, ATS_DATEI);
	atds->set_steuer_bit(446, ATS_AKTIV_BIT, ATS_DATEI);
	_G(spieler).PersonHide[P_HOWARD] = false;
	_G(spieler).PersonHide[P_NICHELLE] = false;
	_G(spieler).PersonHide[P_CHEWY] = false;
	_G(spieler).flags28_2 = true;
	flags.NoScroll = false;
	menu_item = CUR_WALK;
	cursor_wahl(CUR_WALK);
	start_aad_wait(434, -1);
}

void Room71::proc3() {
	start_aad_wait(431, -1);
	SetUpScreenFunc = nullptr;
	go_auto_xy(510, 34, P_NICHELLE, ANI_VOR);
	SetUpScreenFunc = setup_func;
	_G(spieler).PersonHide[P_CHEWY] = true;
	_G(spieler).PersonHide[P_NICHELLE] = true;
	proc5(6);
	_G(spieler).PersonHide[P_NICHELLE] = false;
	_G(spieler).PersonHide[P_CHEWY] = false;
}

void Room71::proc4() {
	start_aad_wait(430, -1);
	SetUpScreenFunc = nullptr;
	go_auto_xy(518, 35, P_HOWARD, ANI_VOR);
	SetUpScreenFunc = setup_func;
	_G(spieler).PersonHide[P_CHEWY] = true;
	_G(spieler).PersonHide[P_HOWARD] = true;
	proc5(4);
	_G(spieler).PersonHide[P_HOWARD] = false;
	_G(spieler).PersonHide[P_CHEWY] = false;
}

void Room71::proc5(int16 val) {
	_G(spieler).r71_word18DB16 = 1 + (_G(spieler).flags28_4 ? 1 : 0);
	det->hide_static_spr(2);
	start_detail_wait(val, 5, ANI_VOR);
	det->show_static_spr(2);
	_G(spieler).r71_word18DB16 = 0;
}

int Room71::proc6() {
	if (_G(spieler).inv_cur)
		return 0;

	hide_cur();
	auto_move(3, P_CHEWY);
	start_spz_wait(13, 1, false, P_CHEWY);
	det->hide_static_spr(3);
	det->show_static_spr(4);
	spieler_mi[P_CHEWY].Mode = true;
	go_auto_xy(244, 44, P_CHEWY, ANI_VOR);
	start_spz_wait(14, 1, false, P_CHEWY);
	det->hide_static_spr(4);
	det->show_static_spr(5);
	_G(spieler).flags28_4 = true;
	room->set_timer_status(0, TIMER_STOP);
	det->del_static_ani(0);
	atds->set_steuer_bit(443, ATS_AKTIV_BIT, ATS_DATEI);
	auto_move(3, P_CHEWY);
	spieler_mi[P_CHEWY].Mode = false;
	
	show_cur();
	return 1;
}

void Room71::proc7() {
	hide_cur();
	auto_move(4, P_CHEWY);
	start_aad_wait(429, -1);
	SetUpScreenFunc = nullptr;
	set_person_spr(0, P_CHEWY);
	go_auto_xy(323, 28, P_NICHELLE, ANI_VOR);
	flags.NoScroll = true;
	auto_scroll(200, 0);
	_G(spieler).PersonHide[P_NICHELLE] = true;
	det->start_detail(7, 255, false);
	start_aad_wait(624, -1);
	det->stop_detail(7);
	_G(spieler).PersonHide[P_NICHELLE] = false;
	SetUpScreenFunc = setup_func;
	flags.NoScroll = false;
	start_aad_wait(432, -1);

	show_cur();
}

} // namespace Rooms
} // namespace Chewy
