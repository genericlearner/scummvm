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


#include "common/achievements.h"
#include "common/debug.h"
#include "common/system.h"
#include "common/translation.h"

namespace Common {

DECLARE_SINGLETON(AchievementsManager);


AchievementsManager::AchievementsManager() {
	_iniFile = nullptr;
	unsetActiveDomain();
}


AchievementsManager::~AchievementsManager() {
}

bool AchievementsManager::setActiveDomain(const AchievementsInfo &info) {
	if (info.appId.empty()) {
		unsetActiveDomain();
		return false;
	}

	_descriptions = info.descriptions;

	return setActiveDomain(info.platform, info.appId);
}

bool AchievementsManager::setActiveDomain(AchievementsPlatform platform, const String &appId) {
	const char* prefix = platform == STEAM_ACHIEVEMENTS ? "steam" :
					platform == GALAXY_ACHIEVEMENTS ? "galaxy" :
					"achman";

	String iniFileName = String::format("%s-%s.dat", prefix, appId.c_str());

	if (_iniFileName == iniFileName) {
		return true;
	}

	if (isReady()) {
		unsetActiveDomain();
	}

	_iniFileName = iniFileName;

	_iniFile = new Common::INIFile();
	_iniFile->loadFromSaveFile(_iniFileName); // missing file is OK

	return true;
}


bool AchievementsManager::unsetActiveDomain() {
	_iniFileName = "";

	delete _iniFile;
	_iniFile = nullptr;

	_descriptions.clear();

	return true;
}


bool AchievementsManager::setAchievement(const String &id) {
	if (!isReady()) {
		return false;
	}
	if (isAchieved(id)) {
		return true;
	}

	String displayedMessage = id;
	for (uint32 i = 0; i < _descriptions.size(); i++) {
		if (strcmp(_descriptions[i].id, id.c_str()) == 0) {
			displayedMessage = _descriptions[i].title;
			break;
		}
	}

	return setAchievement(id, displayedMessage);
}

bool AchievementsManager::setAchievement(const String &id, const String &displayedMessage) {
	if (!isReady()) {
		return false;
	}
	if (isAchieved(id)) {
		return true;
	}

	debug("AchievementsManager::setAchievement('%s'): %s", id.c_str(), displayedMessage.c_str());

	_iniFile->setKey(id, "achievements", "true");
	_iniFile->saveToSaveFile(_iniFileName);

	if (!displayedMessage.empty() && g_system) {
		U32String msg;
		msg = Common::U32String::format("%S\n%S",
			_("Achievement unlocked!").c_str(),
			Common::U32String(displayedMessage).c_str()
		);
		g_system->displayMessageOnOSD(msg);
	}

	return true;
}


bool AchievementsManager::isAchieved(const String &id) const {
	if (!isReady()) {
		return false;
	}

	return _iniFile->hasKey(id, "achievements");
}


bool AchievementsManager::clearAchievement(const String &id) {
	if (!isReady()) {
		return false;
	}

	_iniFile->removeKey(id, "achievements");
	_iniFile->saveToSaveFile(_iniFileName);
	return true;
}


bool AchievementsManager::setStatFloat(const String &id, float value) {
	if (!isReady()) {
		return false;
	}

	String tmp = Common::String::format("%8.8f", value);
	_iniFile->setKey(id, "statistics", tmp);
	_iniFile->saveToSaveFile(_iniFileName);
	return 0;
}


float AchievementsManager::getStatFloat(const String &id) {
	if (!isReady()) {
		return 0.0;
	}

	String tmp;
	_iniFile->getKey(id, "statistics", tmp);
	return (float)atof(tmp.c_str());
}

bool AchievementsManager::setStatInt(String const &id, int value) {
	if (!isReady()) {
		return false;
	}

	String tmp = Common::String::format("%d", value);
	_iniFile->setKey(id, "statistics", tmp);
	_iniFile->saveToSaveFile(_iniFileName);
	return 0;
}


int AchievementsManager::getStatInt(String const &id) {
	if (!isReady()) {
		return 0;
	}

	String tmp;
	_iniFile->getKey(id, "statistics", tmp);
	return (int)atol(tmp.c_str());
}


bool AchievementsManager::resetAllAchievements() {
	if (!isReady()) {
		return false;
	}

	_iniFile->removeSection("achievements");
	_iniFile->saveToSaveFile(_iniFileName);
	return 0;
}


bool AchievementsManager::resetAllStats() {
	if (!isReady()) {
		return false;
	}

	_iniFile->removeSection("statistics");
	_iniFile->saveToSaveFile(_iniFileName);
	return 0;
}


} // End of namespace Common
