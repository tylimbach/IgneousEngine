#pragma once

#include "stdint.h"

#include <vector>
#include <span>

namespace bve
{
	template <typename Component>
	class EntityComponentRegistry
	{
	public:
		EntityComponentRegistry(const EntityComponentRegistry&) = delete;
		void operator=(const EntityComponentRegistry&) = delete;

		static EntityComponentRegistry& instance()
		{
			static EntityComponentRegistry instance;
			return instance;
		}

		static void insert(uint32_t id, Component value)
		{
			if (id >= lookup.size()) {
				lookup.resize(id + 1, UINT32_MAX); // Using UINT32_MAX as a sentinel for non-existent entries
			}

			lookup[id] = static_cast<uint32_t>(components.size());
			components.push_back(std::move(value));
			entities.push_back(id);
		}

		static bool erase(uint32_t id)
		{
			if (id < lookup.size() && lookup[id] != UINT32_MAX) {
				uint32_t last = static_cast<uint32_t>(components.size()) - 1;
				uint32_t idx_to_remove = lookup[id];

				if (idx_to_remove != last) {
					std::swap(components[idx_to_remove], components[last]);
					std::swap(entities[idx_to_remove], entities[last]);
					lookup[entities[idx_to_remove]] = idx_to_remove;
				}

				components.pop_back();
				entities.pop_back();
				lookup[id] = UINT32_MAX;
				return true;
			}

			return false;
		}

		static bool contains(uint32_t id)
		{
			return id < lookup.size() && lookup[id] != UINT32_MAX;
		}

		static Component& getComponent(uint32_t id)
		{
			return components[lookup[id]];
		}

		static uint32_t getEntity(uint32_t index)
		{
			return entities[index];
		}

		static std::span<Component> viewComponents()
		{
			return std::span<Component>(components.data(), components.size());
		}

		static std::span<uint32_t> viewEntities()
		{
			return std::span(entities.data(), entities.size());
		}

		static bool empty() noexcept
		{
			return components.empty();
		}

		static size_t size() noexcept
		{
			return components.size();
		}
			private:
		static inline std::vector<uint32_t> lookup;
		static inline std::vector<Component> components;
		static inline std::vector<uint32_t> entities;

	};
}
