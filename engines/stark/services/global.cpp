/* ResidualVM - A 3D game interpreter
 *
 * ResidualVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the AUTHORS
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

#include "engines/stark/services/global.h"

#include "engines/stark/resources/item.h"
#include "engines/stark/resources/level.h"
#include "engines/stark/resources/knowledge.h"
#include "engines/stark/resources/knowledgeset.h"

namespace Stark {

Global::Global() :
		_millisecondsPerGameloop(0),
		_april(nullptr),
		_root(nullptr),
		_level(nullptr),
		_current(nullptr),
		_debug(false),
		_fastForward(false),
		_inventory(nullptr) {
}

int32 Global::getCurrentChapter() {
	Resources::KnowledgeSet *globalState = _level->findChildWithSubtype<Resources::KnowledgeSet>(Resources::KnowledgeSet::kState);
	Resources::Knowledge *chapter = globalState->findChildWithIndex<Resources::Knowledge>(0);
	return chapter->getIntegerValue();
}

void Global::setCurrentChapter(int32 value) {
	Resources::KnowledgeSet *globalState = _level->findChildWithSubtype<Resources::KnowledgeSet>(Resources::KnowledgeSet::kState);
	Resources::Knowledge *chapter = globalState->findChildWithIndex<Resources::Knowledge>(0);
	chapter->setIntegerValue(value);
}

void Global::printInventory(bool printAll) {
	Common::Array<Resources::Item*> inventoryItems = _inventory->listChildren<Resources::Item>(Resources::Item::kItemInventory);
	Common::Array<Resources::Item*>::iterator it = inventoryItems.begin();
	for (int i = 0; it != inventoryItems.end(); ++it, i++) {
		if (printAll || (*it)->isEnabled()) {
			warning("Item %d: %s", i, (*it)->getName().c_str());
		}
	}
}

void Global::enableInventoryItem(int32 num) {
	Common::Array<Resources::Item*> inventoryItems = _inventory->listChildren<Resources::Item>(Resources::Item::kItemInventory);
	inventoryItems[num]->setEnabled(true);
}

} // End of namespace Stark
