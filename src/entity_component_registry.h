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
			if (id >= lookup_.size()) {
				lookup_.resize(id + 1, UINT32_MAX); // Using UINT32_MAX as a sentinel for non-existent entries
			}

			lookup_[id] = static_cast<uint32_t>(components_.size());
			components_.push_back(std::move(value));
			entities_.push_back(id);
		}

		static bool erase(uint32_t id)
		{
			if (id < lookup_.size() && lookup_[id] != UINT32_MAX) {
				uint32_t last = static_cast<uint32_t>(components_.size()) - 1;
				uint32_t idx_to_remove = lookup_[id];

				if (idx_to_remove != last) {
					std::swap(components_[idx_to_remove], components_[last]);
					std::swap(entities_[idx_to_remove], entities_[last]);
					lookup_[entities_[idx_to_remove]] = idx_to_remove;
				}

				components_.pop_back();
				entities_.pop_back();
				lookup_[id] = UINT32_MAX;
				return true;
			}

			return false;
		}

		static bool contains(uint32_t id)
		{
			return id < lookup_.size() && lookup_[id] != UINT32_MAX;
		}

		static Component& getComponent(uint32_t id)
		{
			return components_[lookup_[id]];
		}

		static uint32_t getEntity(uint32_t index)
		{
			return entities_[index];
		}

		static std::span<Component> viewComponents()
		{
			return std::span<Component>(components_.data(), components_.size());
		}

		static std::span<uint32_t> viewEntities()
		{
			return std::span(entities_.data(), entities_.size());
		}

		static bool empty() noexcept
		{
			return components_.empty();
		}

		static size_t size() noexcept
		{
			return components_.size();
		}

	private:
		static inline std::vector<uint32_t> lookup_;
		static inline std::vector<Component> components_;
		static inline std::vector<uint32_t> entities_;
	};
}
