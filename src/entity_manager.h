#pragma once

#include "entity_component_registry.h"
#include "entity_component_view.h"

#include <tuple>
#include <unordered_set>
#include <any>
#include <atomic>
#include <span>


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
		bool hasComponent(Entity entity)
		{
			return EntityComponentRegistry<T>::contains(entity);
		}

		template <typename T>
		void addComponent(Entity entity, T component)
		{
			EntityComponentRegistry<T>::insert(entity, std::move(component));
		}

		template <typename T>
		T& getComponent(Entity entity)
		{
			return EntityComponentRegistry<T>::getComponent(entity);
		}

		template <typename... T>
		std::tuple<T...> getComponents(Entity entity)
		{
			return { this->getComponent<T>(entity)... };
		}

		template<typename T>
		EntityComponentView<T> view() {
			std::span<uint32_t> entities = EntityComponentRegistry<T>::viewEntities();
			std::span<T> components = EntityComponentRegistry<T>::viewComponents();
			return EntityComponentView<T>(entities, components);
		}

		template <typename T>
		bool removeComponent(Entity entity)
		{
			return EntityComponentRegistry<T>::erase(entity);
		}

	private:
		std::atomic<uint32_t> counter = 0;
		std::unordered_set<Entity> entities;
	};
}
