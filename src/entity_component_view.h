#pragma once

#include "stdint.h"
#include <tuple>
#include <span>
#include <cassert>

namespace bve
{
	template <typename T>
	class EntityComponentView
	{
	public:
		EntityComponentView(std::span<uint32_t> eSpan, std::span<T> cSpan)
			: entitySpan_(eSpan), componentSpan_(cSpan)
		{
			assert(entitySpan_.size() == componentSpan_.size() && "Entities & component spans have different lengths");
		}

		class Iterator
		{
		public:
			Iterator(std::span<uint32_t>::iterator eIt, typename std::span<T>::iterator cIt)
				: entityIter_(eIt), componentIter_(cIt) {}

			std::tuple<uint32_t&, T&> operator*()
			{
				return {*entityIter_, *componentIter_};
			}

			Iterator& operator++()
			{
				++entityIter_;
				++componentIter_;
				return *this;
			}

			Iterator operator++(int)
			{
				Iterator tmp = *this;
				++(*this);
				return tmp;
			}

			bool operator==(const Iterator& other) const
			{
				return entityIter_ == other.entityIter_;
			}

			bool operator!=(const Iterator& other) const
			{
				return !(*this == other);
			}

		private:
			std::span<uint32_t>::iterator entityIter_;
			typename std::span<T>::iterator componentIter_;
		};

		Iterator begin()
		{
			return Iterator(entitySpan_.begin(), componentSpan_.begin());
		}

		Iterator end()
		{
			return Iterator(entitySpan_.end(), componentSpan_.end());
		}

		Iterator begin() const
		{
			return Iterator(entitySpan_.begin(), componentSpan_.begin());
		}

		Iterator end() const
		{
			return Iterator(entitySpan_.end(), componentSpan_.end());
		}

		std::span<uint32_t> entitySpan_;
		std::span<T> componentSpan_;
	};
}
