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
#include "chewy/globals.h"
#include "chewy/room.h"
#include "chewy/rooms/room84.h"
#include "chewy/sound.h"

namespace Chewy {
namespace Rooms {

bool Room84::_flag;

void Room84::entry() {
	_G(spieler).ScrollxStep = 2;
	_G(zoom_horizont) = 110;
	_G(flags).ZoomMov = true;
	_G(zoom_mov_fak) = 3;
	_G(spieler_mi)[P_HOWARD].Mode = true;
	_G(spieler_mi)[P_NICHELLE].Mode = true;
	_G(spieler).R84GoonsPresent = false;
	_flag = false;
	g_engine->_sound->playSound(0, 0);

	if (_G(spieler).R88UsedMonkey) {
		_G(det)->del_static_ani(3);
		_G(det)->stop_detail(0);
		_G(spieler).room_e_obj[124].Attribut = AUSGANG_OBEN;
		_G(atds)->set_steuer_bit(478, ATS_AKTIV_BIT, ATS_DATEI);
		_G(atds)->set_steuer_bit(479, ATS_AKTIV_BIT, ATS_DATEI);
		_G(atds)->set_ats_str(485, 1, ATS_DATEI);	
	}

	if (_G(spieler).flags32_10) {
		_G(det)->start_detail(7, 255, false);
		_G(atds)->del_steuer_bit(504, ATS_AKTIV_BIT, ATS_DATEI);
		_G(spieler).room_e_obj[124].Attribut = 255;
		_G(atds)->set_ats_str(485, 2, ATS_DATEI);
	}

	if (_G(spieler).r88DestRoom == 84) {
		_G(det)->show_static_spr(7);
		_G(atds)->del_steuer_bit(481, ATS_AKTIV_BIT, ATS_DATEI);
		_G(atds)->del_steuer_bit(482, ATS_AKTIV_BIT, ATS_DATEI);
	}

	if (_G(spieler).flags32_40) {
		_G(atds)->set_steuer_bit(481, ATS_AKTIV_BIT, ATS_DATEI);
		_G(atds)->set_steuer_bit(482, ATS_AKTIV_BIT, ATS_DATEI);
		_G(atds)->set_ats_str(485, 1, ATS_DATEI);		
		_G(spieler).room_e_obj[124].Attribut = AUSGANG_OBEN;
	}

	if (_G(flags).LoadGame) {
		; // Nothing. It avoids a deeper level of if, and we need the _G(SetUpScreenFunc) at the end
	} else if (_G(spieler).flags30_1 || _G(spieler).flags31_8) {
		set_person_pos(569, 135, P_CHEWY, P_LEFT);
		set_person_pos(489, 113, P_HOWARD, P_RIGHT);
		set_person_pos(523, 110, P_NICHELLE, P_RIGHT);
		_G(spieler).flags30_1 = false;
		_G(spieler).flags31_8 = false;
		_G(maus_links_click) = false;
		_G(spieler).scrollx = 319;

		if (_G(spieler).flags32_10) {
			set_person_pos(347, 130, P_CHEWY, P_RIGHT);
			set_person_pos(408, 113, P_HOWARD, P_LEFT);
			_G(spieler).scrollx = 250;
			if (_G(spieler).flags32_40) {
				start_aad_wait(481, -1);
				_G(spieler).room_e_obj[124].Attribut = AUSGANG_OBEN;
			}
		}
	} else if (_G(spieler).flags32_20) {
		hideCur();
		set_person_pos(347, 130, P_CHEWY, P_RIGHT);
		set_person_pos(408, 113, P_HOWARD, P_LEFT);
		_G(spieler).scrollx = 250;
		_G(spieler).flags32_20 = false;
		_G(spieler).room_e_obj[124].Attribut = 255;
		_G(spieler).room_e_obj[125].Attribut = 255;
		_G(atds)->set_ats_str(485, 2, ATS_DATEI);
		start_aad_wait(477, -1);
		showCur();
	} else {
		set_person_pos(263, 136, P_CHEWY, P_LEFT);
		set_person_pos(238, 113, P_HOWARD, P_RIGHT);
		set_person_pos(294, 110, P_NICHELLE, P_RIGHT);
		_G(spieler).flags30_1 = false;
		_G(spieler).scrollx = 156;
	}
	
	_G(SetUpScreenFunc) = setup_func;
}

void Room84::xit(int16 eib_nr) {
	_G(spieler).ScrollxStep = 1;
	if (eib_nr == 125) {
		_G(spieler).R79Val[P_CHEWY] = 1;
		_G(spieler).R79Val[P_HOWARD] = 1;
		_G(spieler).R79Val[P_NICHELLE] = 1;
		_G(spieler).PersonRoomNr[P_HOWARD] = 80;
		_G(spieler).PersonRoomNr[P_NICHELLE] = 80;
	} else {
		if (_G(spieler).PersonRoomNr[P_HOWARD] == 84)
			_G(spieler).PersonRoomNr[P_HOWARD] = 85;
		if (_G(spieler).PersonRoomNr[P_NICHELLE] == 84)
			_G(spieler).PersonRoomNr[P_NICHELLE] = 85;
	}
}

void Room84::setup_func() {
	calc_person_look();

	if (_G(spieler).R84GoonsPresent && !_flag) {
		_flag = true;
		_G(spieler).flags30_80 = true;
		hideCur();
		auto_move(4, P_CHEWY);
		_G(flags).NoScroll = true;
		set_person_spr(P_LEFT, P_CHEWY);
		auto_scroll(150, 0);
		_G(det)->del_static_ani(3);
		_G(det)->set_static_ani(4, -1);
		start_aad_wait(455, -1);
		_G(det)->del_static_ani(4);
		start_spz(62, 1, false, P_HOWARD);
		start_detail_wait(5, 1, ANI_VOR);
		_G(det)->set_static_ani(3, -1);
		start_aad_wait(456, -1);
		_G(flags).NoScroll = false;
		showCur();
	}

	const int posX = _G(spieler_vector)[P_CHEWY].Xypos[0];
	int howDestX, nicDestX;
	if (posX > 610) {
		howDestX = nicDestX = 610;
	} else if (posX < 280) {
		howDestX = 238;
		nicDestX = 294;
	} else if (posX < 380) {
		howDestX = 408;
		nicDestX = 454;
	} else {
		howDestX = 489;
		nicDestX = 523;
	}

	go_auto_xy(howDestX, 113, P_HOWARD, ANI_GO);
	go_auto_xy(nicDestX, 110, P_NICHELLE, ANI_GO);
}

void Room84::talk1() {
	auto_move(4, P_CHEWY);
	_G(flags).NoScroll = true;
	set_person_spr(P_LEFT, P_CHEWY);
	auto_scroll(150, 0);
	start_ads_wait(22);
	_G(flags).NoScroll = false;
}

void Room84::talk2() {
	start_aad_wait(478, -1);
}

int Room84::proc4() {
	if (_G(spieler).inv_cur)
		return 0;

	hideCur();
	auto_move(5, P_CHEWY);

	if (_G(spieler).flags32_10) {
		_G(det)->stop_detail(7);
		start_detail_wait(8, 1, ANI_VOR);
	} else {
		_G(spieler).PersonRoomNr[P_NICHELLE] = 88;
	}

	_G(spieler).PersonRoomNr[P_HOWARD] = 88;
	switch_room(88);
	_G(menu_item) = CUR_WALK;
	cursorChoice(CUR_WALK);
	showCur();

	return 1;
}

} // namespace Rooms
} // namespace Chewy
