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
#include "chewy/rooms/room51.h"

namespace Chewy {
namespace Rooms {

void Room51::entry() {
	int16 i;
	_G(zoom_horizont) = 140;
	flags.ZoomMov = true;
	_G(zoom_mov_fak) = 4;
	for (i = 0; i < 2; i++)
		det->start_detail(3 + i, 1, ANI_VOR);
	if (_G(spieler).PersonRoomNr[P_HOWARD] == 51) {
		_G(spieler).ZoomXy[P_HOWARD][0] = 40;
		_G(spieler).ZoomXy[P_HOWARD][1] = 30;
		if (!flags.LoadGame) {
			set_person_pos(88, 93, P_HOWARD, P_RIGHT);
		}
		if (!_G(spieler).R51FirstEntry) {
			hide_cur();
			_G(spieler).R51FirstEntry = true;
			set_person_spr(P_LEFT, P_CHEWY);
			start_aad_wait(283, -1);
			show_cur();
		}
		SetUpScreenFunc = setup_func;
		spieler_mi[P_HOWARD].Mode = true;
	}
}

void Room51::xit(int16 eib_nr) {
	if (_G(spieler).PersonRoomNr[P_HOWARD] == 51) {
		if (eib_nr == 85) {
			_G(spieler).PersonRoomNr[P_HOWARD] = 50;
		} else
			_G(spieler).PersonRoomNr[P_HOWARD] = 52;
		spieler_mi[P_HOWARD].Mode = false;
	}
}

bool Room51::timer(int16 t_nr, int16 ani_nr) {
	if (_G(spieler).flags32_10)
		timer_action(t_nr, room->room_timer.ObjNr[ani_nr]);
	else
		return true;

	return false;
}

void Room51::setup_func() {
	int16 x, y;
	int16 ch_y;
	if (_G(spieler).PersonRoomNr[P_HOWARD] == 51) {
		calc_person_look();
		x = spieler_vector[P_HOWARD].Xypos[0];
		y = 64;
		ch_y = spieler_vector[P_CHEWY].Xypos[1];
		if (ch_y < 129) {
			x = 56;
			y = 106;
		} else {
			x = 31;
			y = 118;
		}
		if (HowardMov && flags.ExitMov) {
			SetUpScreenFunc = 0;
			HowardMov = 0;
			auto_move(9, P_HOWARD);
		} else
			go_auto_xy(x, y, P_HOWARD, ANI_GO);
	}
}

int16 Room51::use_door(int16 txt_nr) {
	int16 action_ret = false;
	if (is_cur_inventar(KEY_INV)) {
		hide_cur();
		action_ret = true;
		switch (txt_nr) {
		case 329:
			auto_move(8, P_CHEWY);
			SetUpScreenFunc = 0;
			det->show_static_spr(0);
			if (!_G(spieler).R51HotelRoom) {
				auto_move(11, P_HOWARD);
				set_person_spr(P_LEFT, P_HOWARD);
				_G(spieler).R51HotelRoom = true;
				_G(spieler).room_e_obj[86].Attribut = AUSGANG_LINKS;
				start_aad_wait(285, -1);
				atds->set_ats_str(329, 1, ATS_DATEI);
				SetUpScreenFunc = setup_func;
			} else {
				show_cur();
				switch_room(52);
			}
			break;

		case 330:
			auto_move(9, P_CHEWY);
			if (!_G(spieler).R51KillerWeg) {
				det->show_static_spr(1);
				start_detail_wait(2, 1, ANI_VOR);
				det->start_detail(5, 255, ANI_VOR);
				if (!_G(spieler).R52HotDogOk) {
					start_aad_wait(287, -1);
					auto_move(12, P_CHEWY);
					det->stop_detail(5);
					start_ani_block(5, ablock37);
					det->hide_static_spr(1);
					start_aad_wait(284, -1);
				} else {
					_G(spieler).R51KillerWeg = true;
					start_aad_wait(290, -1);
					out->ausblenden(1);
					out->setze_zeiger(0);
					out->cls();
					out->einblenden(pal, 0);
					flags.NoPalAfterFlc = true;
					flc->set_flic_user_function(cut_serv);
					det->show_static_spr(16);
					flic_cut(FCUT_068, FLC_MODE);
					flc->remove_flic_user_function();
					det->hide_static_spr(16);
					flags.NoPalAfterFlc = false;
					det->stop_detail(5);
					obj->show_sib(SIB_AUSRUEST_R52);
					obj->calc_rsi_flip_flop(SIB_AUSRUEST_R52);
					det->hide_static_spr(1);

					fx_blend = BLEND3;
					set_up_screen(DO_SETUP);
					start_aad_wait(291, -1);
				}
			} else
				start_aad_wait(401, -1);
			break;

		case 331:
			auto_move(10, P_CHEWY);
			break;

		case 332:
			auto_move(6, P_CHEWY);
			start_aad_wait(286, -1);
			break;

		case 333:
			auto_move(4, P_CHEWY);
			switch (_G(spieler).R51DoorCount) {
			case 0:
				det->show_static_spr(3);
				start_aad_wait(278, -1);
				start_detail_frame(0, 1, ANI_VOR, 3);
				start_spz(HO_BRILL_JMP, 1, ANI_VOR, P_HOWARD);
				wait_detail(0);

				det->show_static_spr(14);
				start_aad_wait(279, -1);
				++_G(spieler).R51DoorCount;
				obj->show_sib(SIB_FLASCHE_R51);
				obj->calc_rsi_flip_flop(SIB_FLASCHE_R51);
				det->hide_static_spr(3);
				break;

			case 1:
				det->show_static_spr(3);
				start_aad_wait(280, -1);
				start_detail_wait(1, 1, ANI_VOR);
				++_G(spieler).R51DoorCount;
				obj->show_sib(SIB_KAPPE_R51);
				obj->calc_rsi_flip_flop(SIB_KAPPE_R51);
				det->hide_static_spr(3);
				det->show_static_spr(15);
				break;

			default:
				start_aad_wait(281, -1);
				if (_G(spieler).PersonRoomNr[P_HOWARD] == 51)
					start_aad_wait(282, -1);
				break;

			}
			break;

		case 334:
			auto_move(7, P_CHEWY);
			break;

		}
		show_cur();
	}
	return action_ret;
}

int16 Room51::cut_serv(int16 frame) {
	det->plot_static_details(0, 0, 16, 16);
	return 0;
}

void Room51::timer_action(int16 t_nr, int16 obj_nr) {
	::error("TODO");
}

} // namespace Rooms
} // namespace Chewy
