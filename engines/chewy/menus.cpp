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
#include "chewy/menus.h"
#include "chewy/dialogs/inventory.h"

namespace Chewy {

void plot_main_menu() {
	static const int IMAGES[] = { 7, 8, 9, 10, 12, 11 };
	int16 zoomx, zoomy;

	if (_G(menu_item) != _G(tmp_menu)) {
		_G(m_flip) = 0;
		_G(tmp_menu) = _G(menu_item);
	}

	maus_mov_menu();
	int16 *korrektur = (int16 *)_G(menutaf)->korrektur;

	for (int16 i = MENU_START_SPRITE; i < MAX_MENU_SPRITE; i++) {
		int deltaX = 0;

		if (i <= 8) {
			zoomx = 0;
			zoomy = 0;
		} else {
			zoomx = -3;
			zoomy = -3;

			if (i == 11)
				deltaX = 40;
			else if (i == 12)
				deltaX = -40;
		}

		_G(out)->scale_set(_G(menutaf)->image[i],
			MENU_X + deltaX + korrektur[i * 2],
		    _G(spieler).MainMenuY + korrektur[i * 2 + 1],
			zoomx, zoomy, 0);
	}

	zoomx = 16;
	zoomy = 16;
	++_G(m_flip);
	if (_G(m_flip) < 12 * (_G(spieler).DelaySpeed + 1)) {
		int deltaX = 0;
		if (_G(menu_item) == CUR_SAVE)
			deltaX = -40;
		else if (_G(menu_item) == CUR_INVENT)
			deltaX = 40;

		int img = IMAGES[_G(menu_item)];
		_G(out)->scale_set(_G(menutaf)->image[img],
		    MENU_X + deltaX + korrektur[img * 2] - 5,
		    _G(spieler).MainMenuY + korrektur[img * 2 + 1] - 10,
			zoomx, zoomy, 0);
	} else {
		if (_G(m_flip) > 15 * (_G(spieler).DelaySpeed + 1))
			_G(m_flip) = 0;
	}
}

void maus_mov_menu() {
	_G(maus_menu_x) = g_events->_mousePos.x;
	if (_G(maus_menu_x) > 200) {
		g_events->warpMouse(Common::Point(200, g_events->_mousePos.y));
		_G(maus_menu_x) = 200;
	}

	_G(menu_item) = (_G(maus_menu_x) / (MAUS_MENU_MAX_X / 5));
}

void calc_txt_xy(int16 *x, int16 *y, char *txt_adr, int16 txt_anz) {
	int16 vorx, vory, fntbr, fnth;
	_G(out)->get_fontinfo(&vorx, &vory, &fntbr, &fnth);
	int16 len = 0;
	for (int16 i = 0; i < txt_anz; i++) {
		int16 tmp_len = strlen(_G(txt)->str_pos((char *)txt_adr, i));
		if (tmp_len > len)
			len = tmp_len;
	}
	len = len * vorx;
	int16 pix_len = len / 2;
	*x = *x - pix_len + 12;
	if (*x > (SCREEN_WIDTH - len))
		*x = SCREEN_WIDTH - len;
	else if (*x < 0)
		*x = 0;
	*y = *y - (10 * txt_anz);
	if (*y < 0)
		*y = 0;
}

void get_display_xy(int16 *x, int16 *y, int16 nr) {
	int16 *xy = (int16 *)_G(inv_spr)[nr];
	int16 x1 = 48 - xy[0];
	x1 /= 2;
	*x = x1;
	int16 y1 = 48 - xy[1];
	y1 /= 2;
	*y = y1;
}

void build_menu(int16 x, int16 y, int16 xanz, int16 yanz, int16 col, int16 mode) {
#define BAU_MENU_ECKE_LO 15
#define BAU_MENU_ECKE_RO 16
#define BAU_MENU_ECKE_LU 17
#define BAU_MENU_ECKE_RU 18
#define BAU_MENU_UNTEN 19
#define BAU_MENU_SEITE_L 20
#define BAU_MENU_SEITE_R 21
#define BAU_MENU_OBEN 22
#define BAU_MENU_OBEN_L 23
#define BAU_MENU_OBEN_R 24
#define BAU_MENU_OBEN_M 25
	int16 i, j;
	int16 xy[4][2] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}};
	uint8 sprite_eckenr[4] = { BAU_MENU_ECKE_LO, BAU_MENU_ECKE_RO, BAU_MENU_ECKE_LU, BAU_MENU_ECKE_RU };

	xy[0][0] = x;
	xy[0][1] = y;
	xy[1][0] = x + 16 * (xanz - 1);
	xy[1][1] = y;
	xy[2][0] = x;
	xy[2][1] = y + 16 * (yanz - 1);
	xy[3][0] = x + 16 * (xanz - 1);
	xy[3][1] = y + 16 * (yanz - 1);

	for (i = 0; i < 4; i++)
		_G(out)->sprite_set(_G(menutaf)->image[(int16)sprite_eckenr[i]],
		                 xy[i][0], xy[i][1], _G(scr_width));

	int16 s_nr = BAU_MENU_SEITE_L;
	for (j = 0; j < 2; j++) {
		y = xy[j][1] + 16;
		for (i = 0; i < yanz - 2; i++) {
			_G(out)->sprite_set(_G(menutaf)->image[s_nr], xy[j][0], y + i * 16, _G(scr_width));
		}
		++s_nr;
	}

	s_nr = BAU_MENU_OBEN;
	for (j = 0; j < 2; j++) {
		x = xy[j * 2][0] + 16;
		if ((!mode) || (mode == 1 && j == 1)) {
			for (i = 0; i < xanz - 2; i++) {
				_G(out)->sprite_set(_G(menutaf)->image[s_nr], x + i * 16, xy[j * 2][1], _G(scr_width));
			}
		}
		s_nr -= 3;
	}

	if (mode) {
		int16 leer = 0;
		int16 mitte;
		i = xanz - 7;
		if (i == 0)
			mitte = 1;
		else if (i < 6)
			mitte = i + 1;
		else if (i < 8) {
			leer = 1;
			mitte = i - 1;
		} else {
			leer = 2;
			mitte = i - 3;
		}

		x = xy[0][0] + 16;
		for (j = 0; j < 2; j++) {
			for (i = 0; i < leer; i++)
				_G(out)->sprite_set(_G(menutaf)->image[BAU_MENU_OBEN], x + i * 16, xy[0][1], _G(scr_width));
			x = xy[1][0] - leer * 16;
		}
		_G(out)->sprite_set(_G(menutaf)->image[BAU_MENU_OBEN_L], xy[0][0] + 16 + leer * 16, xy[0][1], _G(scr_width));

		x = xy[0][0] + 16 + leer * 16 + 32;
		for (i = 0; i < mitte; i++)
			_G(out)->sprite_set(_G(menutaf)->image[BAU_MENU_OBEN_M], x + i * 16, xy[0][1], _G(scr_width));
		_G(out)->sprite_set(_G(menutaf)->image[BAU_MENU_OBEN_R], x + i * 16, xy[0][1], _G(scr_width));
	}

	_G(out)->box_fill(xy[0][0] + 16, xy[0][1] + 16, xy[0][0] + 16 + (xanz - 2) * 16, xy[0][1] + 16 + (yanz - 2) * 16, col);
}

void auto_menu(int16 *x, int16 *y, int16 zeilen_anz, int16 hoehe, char *text, int16 mode) {
	int16 tmp;

	int16 x_pix = 0;
	for (int16 i = 0; i < zeilen_anz; i++) {
		tmp = strlen(_G(txt)->str_pos(text, i));
		if (x_pix < tmp)
			x_pix = tmp;
	}
	x_pix *= _G(fvorx6x8);
	x_pix += 12;
	tmp = x_pix;
	if (x_pix % 16)
		tmp += 16;
	int16 tmp1 = zeilen_anz * hoehe + 6 + 11;
	if (tmp1 % 16)
		tmp1 += 16;
	*x -= ((tmp / 16) * 16) / 2;
	*y -= ((tmp1 / 16) * 16) / 2;
	build_menu(*x, *y, tmp / 16, tmp1 / 16, 60, mode);
	*x += 6 + (((tmp / 16) * 16) - x_pix) / 2;
	*y += 5 + ((((tmp1 / 16) * 16) - 4) - (zeilen_anz * hoehe)) / 2;
}

#define ADS_WIN 0,153,20,3,60,1

void ads_menu() {
	int16 cur_y_start;
	int16 col;

	if (_G(flags).AdsDialog) {
		_G(flags).ShowAtsInvTxt = false;
		_G(flags).MainInput = false;
		if (_G(ads_item_anz) > 4)
			cur_y_start = 190;
		else
			cur_y_start = 190 - (4 - _G(ads_item_anz)) * 10;
		int16 cur_y = _G(minfo).y;
		if (cur_y < 160 || cur_y > cur_y_start + 10)
			cur_y = 255;
		else
			cur_y = (cur_y_start + 5 - cur_y) / 10;

		if (_G(atds)->aad_get_status() == -1 && _G(ads_push) == false &&
		        _G(flags).NoDiaBox == false) {
			_G(cur_display) = true;

			build_menu(ADS_WIN);
			_G(out)->set_fontadr(_G(font6x8));
			_G(out)->set_vorschub(_G(fvorx6x8), _G(fvory6x8));
			if (_G(ads_item_anz) > 4)
				cur_y_start = 190;
			else
				cur_y_start = 190 - (4 - _G(ads_item_anz)) * 10;
			for (int16 i = 0; i < _G(ads_item_anz) && i < 4; i++) {
				if (cur_y == i)
					col = 255;
				else
					col = 14;
				_G(out)->printxy(4, cur_y_start - i * 10, col, 300, 0, _G(ads_item_ptr)[i]);
			}
		}

		switch (_G(in)->get_switch_code()) {
		case 255:
		case Common::KEYCODE_RETURN:
			if (cur_y < _G(ads_item_anz) && cur_y >= 0 && _G(ads_push) == false) {
				_G(cur_display) = false;
				_G(ads_push) = true;
				_G(minfo).y = 159;
				AdsNextBlk *an_blk = _G(atds)->ads_item_choice(_G(ads_blk_nr), cur_y);
				if (an_blk->BlkNr == -1) {
					ads_action(_G(ads_dia_nr), _G(ads_blk_nr), an_blk->EndNr);
					ads_ende(_G(ads_dia_nr), _G(ads_blk_nr), an_blk->EndNr);
					stop_ads_dialog();
				} else {
					an_blk = _G(atds)->calc_next_block(_G(ads_blk_nr), cur_y);
					ads_action(_G(ads_dia_nr), _G(ads_blk_nr), an_blk->EndNr);
					_G(ads_blk_nr) = an_blk->BlkNr;
					_G(ads_item_ptr) = _G(atds)->ads_item_ptr(_G(ads_blk_nr),
					                                  &_G(ads_item_anz));
				}
				_G(det)->stop_detail(_G(talk_start_ani));
				_G(det)->show_static_spr(_G(talk_hide_static));
				_G(talk_start_ani) = -1;
				_G(talk_hide_static) = -1;
				if (_G(flags).AdsDialog == false) {
					_G(atds)->save_ads_header(_G(ads_dia_nr));
				}
			}
			break;

		default:
			_G(ads_push) = false;
			break;
		}
	}
}

void stop_ads_dialog() {
	aad_wait(-1);
	_G(spieler).DispFlag = _G(ads_tmp_dsp);
	_G(cur_display) = true;
	_G(flags).ShowAtsInvTxt = true;
	_G(flags).MainInput = true;
	_G(flags).AdsDialog = false;
	_G(maus_links_click) = false;
	_G(atds)->stop_ads();
	if (_G(minfo).button)
		_G(flags).main_maus_flag = 1;
}

void cur_2_inventory() {
	if (_G(spieler).AkInvent != -1) {
		invent_2_slot(_G(spieler).AkInvent);
		_G(spieler).AkInvent = -1;
		_G(menu_item) = CUR_WALK;
		cursorChoice(_G(menu_item));
	}
	_G(spieler).inv_cur = false;
}

void inventory_2_cur(int16 nr) {
	if (_G(spieler).AkInvent == -1 && _G(obj)->checkInventory(nr)) {
		del_invent_slot(nr);
		_G(menu_item) = CUR_USE;
		_G(spieler).AkInvent = nr;
		cursorChoice(CUR_AK_INVENT);
		get_display_xy(&_G(spieler).DispZx, &_G(spieler).DispZy, _G(spieler).AkInvent);
	}
}

void new_invent_2_cur(int16 inv_nr) {
	cur_2_inventory();
	_G(obj)->addInventory(inv_nr, &_G(room_blk));
	inventory_2_cur(inv_nr);
}

void invent_2_slot(int16 nr) {
	int16 ok = 0;
	for (int16 i = 0; i < MAX_MOV_OBJ && !ok; i++) {
		if (_G(spieler).InventSlot[i] == -1) {
			_G(spieler).InventSlot[i] = nr;
			ok = true;
		}
	}
	_G(obj)->addInventory(nr, &_G(room_blk));
}

int16 del_invent_slot(int16 nr) {
	int16 ok = -1;
	for (int16 i = 0; i < MAX_MOV_OBJ; i++) {
		if (_G(spieler).InventSlot[i] == nr) {
			_G(spieler).InventSlot[i] = -1;
			if (ok == -1)
				ok = i;
		}
	}

	return ok;
}

void remove_inventory(int16 nr) {
	if (nr == _G(spieler).AkInvent) {
		del_inventar(nr);
	} else {
		_G(obj)->delInventory(nr, &_G(room_blk));
		del_invent_slot(nr);
	}
}

} // namespace Chewy
