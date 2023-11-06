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

		template <typename Component>
		bool has(Entity entity)
		{
			return EntityComponentRegistry<Component>::contains(entity);
		}

		template <typename... Components>
		void add(Entity entity, Components&&... components) {
			(EntityComponentRegistry<std::decay_t<Components>>::insert(entity, std::forward<Components>(components)), ...);
		}


		template <typename Component, typename... ComponentsLeft>
		void add(Entity entity)
		{
			EntityComponentRegistry<Component>::insert(entity, Component{});
			if constexpr (sizeof...(ComponentsLeft) > 0) {
				add<ComponentsLeft...>(entity);
			}
		}

		template <typename Component>
		Component& get(Entity entity)
		{
			return EntityComponentRegistry<Component>::getComponent(entity);
		}

		//template <typename... Component>
		//std::tuple<Component...> get(Entity entity)
		//{
		//	return { this->get<Component>(entity)... };
		//}

		template<typename Component>
		EntityComponentView<Component> view() {
			std::span<uint32_t> entities = EntityComponentRegistry<Component>::viewEntities();
			std::span<Component> components = EntityComponentRegistry<Component>::viewComponents();
			return EntityComponentView<Component>(entities, components);
		}

		template <typename Component>
		bool remove(Entity entity)
		{
			return EntityComponentRegistry<Component>::erase(entity);
		}

	private:
		std::atomic<uint32_t> counter = 0;
		std::unordered_set<Entity> entities;
	};
}
