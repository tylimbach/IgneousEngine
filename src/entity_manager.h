#pragma once

#include "stdint.h"

#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <any>
#include <atomic>

#include "component_registry.h"
#include "util/sparse_set.h"

namespace bve
{
	using Entity = uint32_t;

	class EntityManager {
	public:
		EntityManager() : counter(0) {}
		~EntityManager() = default;

		Entity createEntity()
		{
			Entity newEntity = counter.fetch_add(1);
			entities.insert(newEntity);

			return newEntity;
		}

		template <typename T>
		void addComponent(Entity& entity, T component)
		{
			SparseSet<T>& set = ComponentRegistry<T>::get();
			set.insert(entity, component);
		}

		template <typename T>
		T* getComponent(Entity entity)
		{
			SparseSet<T>& set = ComponentRegistry<T>::get();
			return set.get(entity);
		}

		template <typename... T>
		std::tuple<T*...> getComponents(Entity entity)
		{
			return { EntityManager::getComponent<T>(entity)... };
		}

		template <typename T>
		SparseSet<T>& getAllComponents()
		{
			return ComponentRegistry<T>::get();
		}

		template <typename T>
		bool removeComponent(Entity entity)
		{
			SparseSet<T>& set = ComponentRegistry<T>::get();
			return set.erase(entity);
		}

	private:
		std::atomic<uint32_t> counter = 0;
		std::unordered_set<Entity> entities;
	};
}
