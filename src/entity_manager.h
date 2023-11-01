#pragma once

#include "stdint.h"

#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <any>
#include <typeinfo>
#include <typeindex>
#include <string>
#include <atomic>

#include "util/sparse_set.h"
#include "type_registry.h"

namespace bve {
	class EntityManager {
	public:
		using Entity = uint32_t;

		EntityManager();
		~EntityManager();

		Entity createEntity();

		template <typename T>
		void addComponent(Entity entity, T component);

		template<typename T>
		T* getComponent(Entity entity);
		
		template <typename... T>
		std::tuple<T*...> getComponents(Entity entity);

		template <typename T>
		SparseSet<T>& getAllComponents();

		template <typename T>
		bool removeComponent(Entity entity);

	private:
		std::atomic<uint32_t> counter;
		vector<Entity> entities;
		std::unordered_set<std::type_index> registered_components;
	
		template <typename T>
		void registerComponent();
	};
}
