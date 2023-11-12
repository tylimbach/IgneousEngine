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
		EntityManager(const EntityManager&&) = delete;
		void operator=(const EntityManager&&) = delete;

		Entity createEntity(const std::string& name = std::string());
		void setEntityName(Entity entity, const std::string& name);
		std::string getEntityName(Entity entity);

		std::ranges::view auto getEntities() const { return std::views::all(entityNames_); }

		template <typename Component>
		bool hasComponent(Entity entity);
		template <typename... Components>
		void addComponent(Entity entity, Components&&... components);
		template <typename Component, typename... ComponentsLeft>
		void addComponent(Entity entity);
		template <typename Component>
		bool removeComponent(Entity entity);
		template <typename Component>
		Component& getComponent(Entity entity);
		template <typename Component>
		std::optional<Entity> getOnlyEntity();
		template <typename Component>
		EntityComponentView<Component> view();

	private:
		std::atomic<uint32_t> counter_ = 0;
		std::unordered_map<Entity, std::string> entityNames_;
	};


	template <typename Component>
	bool EntityManager::hasComponent(Entity entity)
	{
		return EntityComponentRegistry<Component>::contains(entity);
	}

	template <typename... Components>
	void EntityManager::addComponent(Entity entity, Components&&... components)
	{
		(EntityComponentRegistry<std::decay_t<Components>>::insert(entity, std::forward<Components>(components)), ...);
	}

	template <typename Component, typename... ComponentsLeft>
	void EntityManager::addComponent(Entity entity)
	{
		EntityComponentRegistry<Component>::insert(entity, Component{});
		if constexpr (sizeof...(ComponentsLeft) > 0) {
			addComponent<ComponentsLeft...>(entity);
		}
	}

	template <typename Component>
	bool EntityManager::removeComponent(Entity entity)
	{
		return EntityComponentRegistry<Component>::erase(entity);
	}

	template <typename Component>
	Component& EntityManager::getComponent(Entity entity)
	{
		return EntityComponentRegistry<Component>::getComponent(entity);
	}

	// returns the single Entity with the specified tag if exactly one exists
	template <typename Component>
	std::optional<Entity> EntityManager::getOnlyEntity()
	{
		auto view = this->view<Component>();
		auto it = view.begin();
		auto end = view.end();

		if (it == end) {
			return std::nullopt;
		}

		Entity entity = std::get<0>(*it);
		++it;

		if (it != end) {
			return std::nullopt;
		}

		return entity;
	}

	// returns a view that can be iterated over containing each entity id a
	template <typename Component>
	EntityComponentView<Component> EntityManager::view()
	{
		std::span<Entity> entities = EntityComponentRegistry<Component>::viewEntities();
		std::span<Component> components = EntityComponentRegistry<Component>::viewComponents();
		return EntityComponentView<Component>(entities, components);
	}
}
