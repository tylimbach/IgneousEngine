#pragma once

#include "entity_component_registry.h"
#include "entity_component_view.h"

#include <tuple>
#include <any>
#include <atomic>
#include <optional>
#include <ranges>
#include <span>
#include <string>
#include <unordered_map>
#include <iterator>

namespace bve
{
	using Entity = uint32_t;

	class EntityManager
	{
	public:
		EntityManager() : counter_(0) {}
		~EntityManager() = default;

		EntityManager(const EntityManager&) = delete;
		void operator=(const EntityManager&) = delete;

		Entity createEntity(const std::string& name = std::string())
		{
			const Entity newEntity = counter_.fetch_add(1);
			if (!name.empty()) {
				entityNames_.emplace(newEntity, name);
			} else {
				entityNames_.emplace(newEntity, "Entity " + std::to_string(newEntity));
			}

			return newEntity;
		}

		void setEntityName(Entity entity, const std::string& name)
		{
			if (entityNames_.contains(entity)) {
				entityNames_[entity] = name;
			}
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

		// returns the single Entity with the specified tag if exactly one exists
		template <typename Component>
		std::optional<Entity> getOnlyEntity()
		{
			auto view = this->view<Component>();
			auto it = view.begin();
			auto end = view.end();

			if (it == end) {
				// The view is empty
				return std::nullopt;
			}

			Entity entity = std::get<0>(*it); // Assuming the entity is the first element of the tuple
			++it;

			if (it != end) {
				// More than one element exists
				return std::nullopt;
			}

			// Exactly one element exists
			return entity;
		}

		template <typename Component>
		bool removeComponent(Entity entity)
		{
			return EntityComponentRegistry<Component>::erase(entity);
		}

		std::ranges::view auto getEntities() const
		{
			return std::views::all(entityNames_);
		}

	private:
		std::atomic<uint32_t> counter_ = 0;
		std::unordered_map<Entity, std::string> entityNames_;
	};
}
