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

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef PRINCE_MOB_H
#define PRINCE_MOB_H

#include "common/scummsys.h"
#include "common/rect.h"
#include "common/str.h"

namespace Common {
	class SeekableReadStream;
}

namespace Prince {

class Mob {
public:
	Mob() {}

	bool loadFromStream(Common::SeekableReadStream &stream);

	bool _visible; 
	uint16 _type;
	Common::Rect _rect;
	Common::String _name;
	Common::String _examText;
};

}

#endif

/* vim: set tabstop=4 noexpandtab: */
