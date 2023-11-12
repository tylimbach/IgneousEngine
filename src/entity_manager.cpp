#include "entity_manager.h"

namespace bve
{
	Entity EntityManager::createEntity(const std::string& name)
	{
		const Entity newEntity = counter_.fetch_add(1);
		if (!name.empty()) {
			entityNames_.emplace(newEntity, name);
		} else {
			entityNames_.emplace(newEntity, "Entity " + std::to_string(newEntity));
		}

		return newEntity;
	}

	void EntityManager::setEntityName(Entity entity, const std::string& name)
	{
		if (entityNames_.contains(entity)) {
			entityNames_[entity] = name;
		}
	}

	std::string EntityManager::getEntityName(Entity entity)
	{
		if (entityNames_.contains(entity)) {
			return entityNames_[entity];
		}

		return {};
	}
}
