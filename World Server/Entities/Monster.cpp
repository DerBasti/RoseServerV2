#include "Monster.h"
#include "..\FileTypes\IFO.h"

MonsterSpawn::~MonsterSpawn() {
	std::for_each(this->spawnedMonsters.begin(), this->spawnedMonsters.end(), [](Monster* mon) {
		delete mon;
		mon = nullptr;
	});
	this->spawnedMonsters.clear();
}

void MonsterSpawn::removeMonster(Monster* monRef) {
	if (monRef == nullptr) {
		return;
	}
	auto it = std::find_if(this->spawnedMonsters.begin(), this->spawnedMonsters.end(), [&](Monster* mon) {
		return (mon == monRef);
	});
	if (it != this->spawnedMonsters.end()) {
		this->spawnedMonsters.erase(it);
	}
}

void MonsterSpawn::addMonster(const word_t typeId, const word_t amount) {
	for (word_t i = 0; i < amount; i++) {
		this->spawnedMonsters.push_back(new Monster(typeId, this));
	}
}

Monster::Monster(const word_t typeId, MonsterSpawn* spawnRef) : NPC(typeId, 0.0f) {
	this->spawnReference = spawnRef;
}

void Monster::onDeath() {

	this->getSpawn()->removeMonster(this);

}