#pragma once

#include "stdint.h"
#include <tuple>
#include <span>
#include <cassert>

namespace bve {
	template<typename T>
	class EntityComponentView {
	private:
		std::span<uint32_t> entitySpan;
		std::span<T> componentSpan;

	public:
		EntityComponentView(std::span<uint32_t> eSpan, std::span<T> cSpan)
			: entitySpan(eSpan), componentSpan(cSpan) {
			assert(entitySpan.size() == componentSpan.size() && "Entities & component spans have different lengths");
		}

		class Iterator {
		private:
			typename std::span<uint32_t>::iterator entityIter;
			typename std::span<T>::iterator componentIter;

		public:
			Iterator(typename std::span<uint32_t>::iterator eIt, typename std::span<T>::iterator cIt)
				: entityIter(eIt), componentIter(cIt) {}

			std::tuple<uint32_t&, T&> operator*() {
				return { *entityIter, *componentIter };
			}

			Iterator& operator++() {
				++entityIter;
				++componentIter;
				return *this;
			}

			Iterator operator++(int) {  
				Iterator tmp = *this;
				++(*this); 
				return tmp;
			}

			bool operator==(const Iterator& other) const {
				return entityIter == other.entityIter;
			}

			bool operator!=(const Iterator& other) const {
				return !(*this == other);
			}
		};

		Iterator begin() {
			return Iterator(entitySpan.begin(), componentSpan.begin());
		}

		Iterator end() {
			return Iterator(entitySpan.end(), componentSpan.end());
		}

		const Iterator begin() const {
			return Iterator(entitySpan.begin(), componentSpan.begin());
		}

		const Iterator end() const {
			return Iterator(entitySpan.end(), componentSpan.end());
		}
	};
}
