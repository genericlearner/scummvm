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
#include "chewy/ani_dat.h"
#include "chewy/room.h"
#include "chewy/rooms/room45.h"

namespace Chewy {
namespace Rooms {

void Room45::entry(int16 eib_nr) {
	int16 ch_x, ch_y;
	int16 ho_x, ho_y;

	_G(spieler).ScrollxStep = 2;
	SetUpScreenFunc = setup_func;
	_G(r45_delay) = 0;
	_G(zoom_horizont) = 150;
	flags.ZoomMov = true;
	_G(zoom_mov_fak) = 4;
	_G(spieler).ZoomXy[P_HOWARD][0] = 80;
	_G(spieler).ZoomXy[P_HOWARD][1] = 70;

	if (_G(spieler).PersonRoomNr[P_HOWARD] == 46) {
		spieler_mi[P_HOWARD].Mode = true;
		_G(spieler).PersonRoomNr[P_HOWARD] = 45;
	}

	spieler_mi[P_HOWARD].Mode = true;
	if (!flags.LoadGame) {
		if (_G(spieler).R48TaxiEntry) {
			ch_x = 67;
			ch_y = 146;
			ho_x = 43;
			ho_y = 129;

			_G(spieler).R48TaxiEntry = false;
			_G(maus_links_click) = false;
		} else {
			if (eib_nr == 72) {
				ch_x = 259;
				ch_y = 146;
				ho_x = 304;
				ho_y = 130;
				_G(spieler).scrollx = 130;
			} else {
				ch_x = 68;
				ch_y = 132;
				ho_x = 45;
				ho_y = 124;
			}
		}

		set_person_pos(ch_x, ch_y, P_CHEWY, P_LEFT);
		set_person_pos(ho_x, ho_y, P_HOWARD, P_LEFT);
	}
}

void Room45::xit(int16 eib_nr) {
	_G(spieler).ScrollxStep = 1;

	if (_G(spieler).PersonRoomNr[P_HOWARD] == 45) {
		spieler_mi[P_HOWARD].Mode = false;

		if (eib_nr == 87) {
			_G(spieler).PersonRoomNr[P_HOWARD] = 40;
		}
	}
	if (eib_nr == 87) {
		flags.NoPalAfterFlc = true;
		flic_cut(FCUT_073, FLC_MODE);
	}
}

void Room45::setup_func() {
	int16 i;
	int16 x;
	int16 y;
	int16 ch_x;

	if (menu_display == 0) {
		if (!_G(r45_delay)) {
			_G(r45_delay) = _G(spieler).DelaySpeed / 2;

			for (i = 0; i < MAX_PERSON; i++) {
				if (_G(r45_pinfo)[i][0] == 1) {
					y = Adi[3 + i].y;
					x = Adi[3 + i].x;

					if (i < 6) {
						x -= _G(r45_pinfo)[i][1];
						if (x < -30) {
							_G(r45_pinfo)[i][0] = 0;
							det->stop_detail(3 + i);
						}
					} else {
						x += _G(r45_pinfo)[i][1];
						if (x > 540) {
							_G(r45_pinfo)[i][0] = 0;
							det->stop_detail(3 + i);
						}
					}
					det->set_detail_pos(3 + i, x, y);
				} else {
					++_G(r45_pinfo)[i][2];

					if (_G(r45_pinfo)[i][2] >= _G(r45_pinfo)[i][3]) {
						_G(r45_pinfo)[i][2] = 0;
						_G(r45_pinfo)[i][0] = 1;
						y = Adi[3 + i].y;
						if (i < 6) {
							x = 500;
						} else {
							x = 0;
						}
						det->set_detail_pos(3 + i, x, y);
						det->start_detail(3 + i, 255, ANI_VOR);
					}
				}
			}
		} else {
			--_G(r45_delay);
		}

		if (_G(spieler).PersonRoomNr[P_HOWARD] == 45 && HowardMov != 2) {
			calc_person_look();
			x = spieler_vector[P_HOWARD].Xypos[0];
			y = 64;
			ch_x = spieler_vector[P_CHEWY].Xypos[0];

			if (ch_x < 95) {
				x = 18;
				y = 130;
			} else if (ch_x > 240) {
				x = 304;
				y = 130;
			} else {
				x = 176;
				y = 130;
			}

			if (HowardMov && flags.ExitMov) {
				x = 56;
				y = 122;
			}

			go_auto_xy(x, y, P_HOWARD, ANI_GO);
		}
	}
}

int16 Room45::use_taxi() {
	int16 action_ret = false;
	hide_cur();
	auto_move(1, P_CHEWY);

	if (!_G(spieler).inv_cur) {
		if (_G(spieler).ChewyAni == CHEWY_PUMPKIN) {
			action_ret = true;
			talk_taxi(254);
		} else if (!_G(spieler).R45TaxiOk) {
			action_ret = true;
			talk_taxi(260);
		} else if (_G(spieler).R45TaxiOk) {
			action_ret = true;
			taxi_mov();
		}
	} else if (is_cur_inventar(RING_INV)) {
		action_ret = true;
		del_inventar(RING_INV);
		talk_taxi(256);
		_G(spieler).R45TaxiOk = true;
		taxi_mov();
	} else if (is_cur_inventar(UHR_INV)) {
		action_ret = true;
		talk_taxi(400);
	}

	show_cur();
	return action_ret;
}

void Room45::talk_taxi(int16 aad_nr) {
	room->set_timer_status(12, TIMER_STOP);
	det->del_static_ani(12);
	start_detail_wait(13, 1, ANI_VOR);
	det->set_static_ani(14, -1);
	start_aad_wait(aad_nr, -1);
	det->del_static_ani(14);
	start_detail_wait(13, 1, ANI_RUECK);
	det->set_static_ani(12, -1);
	room->set_timer_status(12, TIMER_START);
}

void Room45::taxi_mov() {
	HowardMov = 2;
	room->set_timer_status(12, TIMER_STOP);
	det->del_static_ani(12);
	det->enable_sound(15, 1);
	det->show_static_spr(11);
	auto_move(3, P_CHEWY);
	_G(spieler).PersonHide[P_CHEWY] = true;
	_G(spieler).R48TaxiPerson[P_CHEWY] = true;

	if (_G(spieler).PersonRoomNr[P_HOWARD] == 45) {
		go_auto_xy(93, 127, P_HOWARD, ANI_WAIT);
		_G(spieler).PersonHide[P_HOWARD] = true;
		_G(spieler).R48TaxiPerson[P_HOWARD] = true;
		_G(spieler).PersonRoomNr[P_HOWARD] = 48;
	}

	det->hide_static_spr(11);
	det->enable_sound(15, 2);
	det->play_sound(15, 2);
	det->enable_sound(15, 0);
	start_detail_wait(15, 1, ANI_VOR);
	det->disable_sound(15, 0);
	switch_room(48);
}

int16 Room45::use_boy() {
	int16 action_ret = false;
	hide_cur();
	auto_move(2, P_CHEWY);

	if (!_G(spieler).R45MagOk) {
		if (is_cur_inventar(DOLLAR175_INV)) {
			action_ret = true;
			new_invent_2_cur(CUTMAG_INV);
			atds->set_ats_str(DOLLAR175_INV, 1, INV_ATS_DATEI);
			start_aad_wait(258, -1);
			room->set_timer_status(0, TIMER_STOP);
			det->del_static_ani(0);
			start_detail_wait(1, 1, ANI_VOR);
			room->set_timer_status(0, TIMER_START);
			det->set_static_ani(0, -1);
			_G(spieler).R45MagOk = true;
		}
	} else {
		start_aad_wait(259, -1);
	}

	show_cur();
	return action_ret;
}

void Room45::talk_boy() {
	int16 aad_nr;
	hide_cur();

	if (!_G(spieler).R45MagOk) {
		auto_move(2, P_CHEWY);
		aad_nr = 257;
	} else {
		aad_nr = 259;
	}

	start_aad_wait(aad_nr, -1);
	show_cur();
}

} // namespace Rooms
} // namespace Chewy
