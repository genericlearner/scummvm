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

/*
 * This code is based on the CRAB engine
 *
 * Copyright (c) Arvind Raja Yadav
 *
 * Licensed under MIT
 *
 */

#ifndef CRAB_GAMECLOCK_H
#define CRAB_GAMECLOCK_H

#include "crab/common_header.h"
#include "crab/timer.h"

namespace Crab {

class GameClock {
	Timer timer;
	uint32 start;
	std::string seperator;

public:
	GameClock() : seperator(" : ") { start = 0; }

	void Start(uint32 initial_time = 0) {
		start = initial_time;
		timer.Start();
	}

	void Start(const std::string &str) {
		uint32 ms = 0, hr = 0, min = 0, sec = 0;
		std::string str_hrs, str_min, str_sec;

		std::size_t found_1 = str.find_first_of(seperator);
		if (found_1 > 0 && found_1 != std::string::npos) {
			str_hrs = str.substr(0, found_1);
			hr = StringToNumber<uint32>(str_hrs);

			std::size_t found_2 = str.find_last_of(seperator);
			if (found_2 > 0 && found_2 != std::string::npos) {
				str_sec = str.substr(found_2 + 1, std::string::npos);
				sec = StringToNumber<uint32>(str_sec);

				str_min = str.substr(found_1 + seperator.length(), found_2 - (2 * seperator.length()));
				min = StringToNumber<uint32>(str_min);
			}
		}

		ms = 3600000 * hr + 60000 * min + 1000 * sec;
		Start(ms);
	}

	std::string GetTime() {
		uint32 ms = start + timer.Ticks();

		uint32 x = ms / 1000;
		uint32 seconds = x % 60;
		x /= 60;
		uint32 minutes = x % 60;
		uint32 hours = x / 60;

		std::string time_str = NumberToString(hours).c_str();
		time_str += seperator;
		time_str += NumberToString(minutes).c_str();
		time_str += seperator;
		time_str += NumberToString(seconds).c_str();

		return time_str;
	}
};

} // End of namespace Crab

#endif // CRAB_GAMECLOCK_H
