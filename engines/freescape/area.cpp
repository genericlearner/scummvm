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

// Based on Phantasma code by Thomas Harte (2013)

#include "freescape/area.h"
#include "common/algorithm.h"
#include "freescape/freescape.h"
#include "freescape/objects/entrance.h"
#include "freescape/objects/geometricobject.h"
#include "freescape/objects/global.h"

namespace Freescape {

Object *Area::objectWithIDFromMap(ObjectMap *map, uint16 objectID) {
	if (!map)
		return nullptr;
	if (!map->contains(objectID))
		return nullptr;
	return (*map)[objectID];
}

Object *Area::objectWithID(uint16 objectID) {
	return objectWithIDFromMap(objectsByID, objectID);
}

Object *Area::entranceWithID(uint16 objectID) {
	return objectWithIDFromMap(entrancesByID, objectID);
}

uint16 Area::getAreaID() {
	return areaID;
}

uint16 Area::getAreaFlags() {
	return areaFlags;
}

uint8 Area::getScale() {
	return scale;
}

Area::Area(uint16 _areaID, uint16 _areaFlags, ObjectMap *_objectsByID, ObjectMap *_entrancesByID) {
	areaID = _areaID;
	areaFlags = _areaFlags;
	objectsByID = _objectsByID;
	entrancesByID = _entrancesByID;

	scale = 0;
	palette = 0;
	skyColor = 255;
	groundColor = 255;
	gasPocketRadius = 0;

	// create a list of drawable objects only
	for (auto &it : *objectsByID) {
		if (it._value->isDrawable()) {
			drawableObjects.push_back(it._value);
		}
	}

	// sort so that those that are planar are drawn last
	struct {
		bool operator()(Object *object1, Object *object2) {
			if (!object1->isPlanar() && object2->isPlanar())
				return true;
			if (object1->isPlanar() && !object2->isPlanar())
				return false;
			return object1->getObjectID() > object2->getObjectID();
		};
	} compareObjects;

	Common::sort(drawableObjects.begin(), drawableObjects.end(), compareObjects);
}

Area::~Area() {
	if (entrancesByID) {
		for (auto &it : *entrancesByID)
			delete it._value;
	}

	if (objectsByID) {
		for (auto &it : *objectsByID)
			delete it._value;
	}

	delete entrancesByID;
	delete objectsByID;

	for (auto &it : conditionSources)
		delete it;
}

void Area::show() {
	debugC(1, kFreescapeDebugMove, "Area name: %s", name.c_str());
	for (auto &it : *objectsByID)
		debugC(1, kFreescapeDebugMove, "objID: %d, type: %d", it._value->getObjectID(), it._value->getType());

	for (auto &it : *entrancesByID)
		debugC(1, kFreescapeDebugMove, "objID: %d, type: %d (entrance)", it._value->getObjectID(), it._value->getType());
}

void Area::loadObjects(Common::SeekableReadStream *stream, Area *global) {
	int objectsByIDSize = stream->readUint32LE();

	for (int i = 0; i < objectsByIDSize; i++) {
		uint16 key = stream->readUint32LE();
		uint32 flags = stream->readUint32LE();
		float x = stream->readFloatLE();
		float y = stream->readFloatLE();
		float z = stream->readFloatLE();
		Object *obj = nullptr;
		if (objectsByID->contains(key)) {
			obj = (*objectsByID)[key];
		} else {
			obj = global->objectWithID(key);
			assert(obj);
			obj = (Object *)((GeometricObject *)obj)->duplicate();
			addObject(obj);
		}
		obj->setObjectFlags(flags);
		obj->setOrigin(Math::Vector3d(x, y, z));
	}
}

void Area::saveObjects(Common::WriteStream *stream) {
	stream->writeUint32LE(objectsByID->size());

	for (auto &it : *objectsByID) {
		Object *obj = it._value;
		stream->writeUint32LE(it._key);
		stream->writeUint32LE(obj->getObjectFlags());
		stream->writeFloatLE(obj->getOrigin().x());
		stream->writeFloatLE(obj->getOrigin().y());
		stream->writeFloatLE(obj->getOrigin().z());
	}
}

void Area::draw(Freescape::Renderer *gfx) {
	gfx->clear();
	assert(drawableObjects.size() > 0);
	for (auto &obj : drawableObjects) {
		if (!obj->isDestroyed() && !obj->isInvisible()) {
			obj->draw(gfx);
		}
	}
}

Object *Area::shootRay(const Math::Ray &ray) {
	float size = 16.0 * 8192.0; // TODO: check if this is max size
	Object *collided = nullptr;
	for (auto &obj : drawableObjects) {
		float objSize = obj->getSize().length();
		if (!obj->isDestroyed() && !obj->isInvisible() && obj->boundingBox.isValid() && ray.intersectAABB(obj->boundingBox) && size >= objSize) {
			debugC(1, kFreescapeDebugMove, "shot obj id: %d", obj->getObjectID());
			collided = obj;
			size = objSize;
		}
	}
	return collided;
}

Object *Area::checkCollisions(const Math::AABB &boundingBox) {
	float size = 3.0 * 8192.0 * 8192.0; // TODO: check if this is max size
	Object *collided = nullptr;
	for (auto &obj : drawableObjects) {
		if (!obj->isDestroyed() && !obj->isInvisible()) {
			GeometricObject *gobj = (GeometricObject *)obj;
			float objSize = gobj->getSize().length();
			if (gobj->collides(boundingBox) && size > objSize) {
				collided = gobj;
				size = objSize;
			}
		}
	}
	return collided;
}

void Area::addObject(Object *obj) {
	assert(obj);
	int id = obj->getObjectID();
	debugC(1, kFreescapeDebugParser, "Adding object %d to room %d", id, areaID);
	assert(!objectsByID->contains(id));
	(*objectsByID)[id] = obj;
	if (obj->isDrawable())
		drawableObjects.insert_at(0, obj);
}

void Area::removeObject(int16 id) {
	assert(objectsByID->contains(id));
	for (uint i = 0; i < drawableObjects.size(); i++) {
		if (drawableObjects[i]->getObjectID() == id) {
			drawableObjects.remove_at(i);
			break;
		}
	}
	objectsByID->erase(id);
}

void Area::addObjectFromArea(int16 id, Area *global) {
	debugC(1, kFreescapeDebugParser, "Adding object %d to room structure", id);
	Object *obj = global->objectWithID(id);
	if (!obj) {
		assert(global->entranceWithID(id));
		(*entrancesByID)[id] = global->entranceWithID(id);
	} else {
		(*objectsByID)[id] = global->objectWithID(id);
		if (obj->isDrawable())
			drawableObjects.insert_at(0, obj);
	}
}

void Area::addStructure(Area *global) {
	Object *obj = nullptr;
	if (!global || !entrancesByID->contains(255)) {
		int id = 254;
		Common::Array<uint8> *gColors = new Common::Array<uint8>;
		for (int i = 0; i < 6; i++)
			gColors->push_back(groundColor);

		obj = (Object *)new GeometricObject(
			Object::Type::Cube,
			id,
			0,                             // flags
			Math::Vector3d(0, -1, 0),      // Position
			Math::Vector3d(4128, 1, 4128), // size
			gColors,
			nullptr,
			FCLInstructionVector());
		(*objectsByID)[id] = obj;
		drawableObjects.insert_at(0, obj);
		return;
	}
	GlobalStructure *rs = (GlobalStructure *)(*entrancesByID)[255];

	for (int i = 0; i < int(rs->structure.size()); i++) {
		int16 id = rs->structure[i];
		if (id == 0)
			continue;

		addObjectFromArea(id, global);
	}
}

} // End of namespace Freescape