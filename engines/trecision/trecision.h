/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef TRECISION_TRECISION_H
#define TRECISION_TRECISION_H

#include "engines/engine.h"
#include "nl/sysdef.h"
#include "trecision/nl/struct.h"

namespace Trecision {

class TrecisionEngine : public Engine {
public:
	TrecisionEngine(OSystem *syst);
	~TrecisionEngine() override;

	Common::Error run() override;

	char _gamePath[250];
	uint16 _curRoom;
	uint16 _oldRoom;
	uint16 _curInventory;
	int32 _curSortTableNum;
	uint16 _curObj;
	uint16 CurScriptFrame[10];

	SRoom _room[MAXROOMS];
};

extern TrecisionEngine *g_vm;

} // End of namespace Trecision

#endif
