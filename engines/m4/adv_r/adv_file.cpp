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

#include "common/system.h"
#include "common/savefile.h"
#include "m4/adv_r/adv_file.h"
#include "m4/m4.h"

namespace M4 {

static Common::SeekableReadStream *openForLoading(int slot) {
	Common::String slotName = g_engine->getSaveStateName(slot);
	return g_system->getSavefileManager()->openForLoading(slotName);
}

bool kernel_save_game_exists(int32 slot) {
	Common::SeekableReadStream *save = openForLoading(slot);
	bool result = save != nullptr;
	delete save;

	return result;
}

bool kernel_load_game(int slot) {
	return g_engine->loadGameState(slot).getCode() == Common::kNoError;
}


} // End of namespace M4
