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

	class EntityManager
	{
	public:
		EntityManager() : counter_(0) {}
		~EntityManager() = default;

		Entity createEntity()
		{
			Entity newEntity = counter_.fetch_add(1);
			entities_.insert(newEntity);

			return newEntity;
		}

		template <typename Component>
		bool hasComponent(Entity entity)
		{
			return EntityComponentRegistry<Component>::contains(entity);
		}

		template <typename... Components>
		void addComponent(Entity entity, Components&&... components)
		{
			(EntityComponentRegistry<std::decay_t<Components>>::insert(entity, std::forward<Components>(components)), ...);
		}


		template <typename Component, typename... ComponentsLeft>
		void addComponent(Entity entity)
		{
			EntityComponentRegistry<Component>::insert(entity, Component{});
			if constexpr (sizeof...(ComponentsLeft) > 0) {
				addComponent<ComponentsLeft...>(entity);
			}
		}

		template <typename Component>
		Component& getComponent(Entity entity)
		{
			return EntityComponentRegistry<Component>::getComponent(entity);
		}

		//template <typename... Component>
		//std::tuple<Component&...> get(Entity entity)
		//{
		//	return { this->get<Component>(entity)... };
		//}

		template <typename Component>
		EntityComponentView<Component> view()
		{
			std::span<Entity> entities = EntityComponentRegistry<Component>::viewEntities();
			std::span<Component> components = EntityComponentRegistry<Component>::viewComponents();
			return EntityComponentView<Component>(entities, components);
		}

		template <typename Component>
		bool removeComponent(Entity entity)
		{
			return EntityComponentRegistry<Component>::erase(entity);
		}

		std::vector<Entity> getEntities() const
		{
			return std::vector (entities_.begin(), entities_.end());
		}

	private:
		std::atomic<uint32_t> counter_ = 0;
		std::unordered_set<Entity> entities_;
	};
}
