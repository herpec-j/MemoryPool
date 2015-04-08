#pragma once

#include <utility>
#include <cassert>

#include "MemoryPool/BufferPool.hpp"

namespace BlindingTechnologies
{
	namespace BlindingEngine
	{
		inline namespace Version_1
		{
			template <typename T, std::size_t Alignment = 16, std::size_t NumberOfObjectPerChunk = 1024>
			class ObjectPool final : public BufferPool
			{
			public:
				// Constructors
				ObjectPool(void)
					: BufferPool(sizeof(T), Alignment, NumberOfObjectPerChunk)
				{
					return;
				}

				ObjectPool(ObjectPool const &) = delete;

				ObjectPool(ObjectPool &&) = default;

				// Assignment Operators
				ObjectPool &operator=(ObjectPool const &) = delete;

				ObjectPool &operator=(ObjectPool &&) = default;

				// Destructor
				virtual ~ObjectPool(void) = default;

				// Methods
				template <typename... Args>
				T *create(Args &&...args)
				{
					return new (allocateMemory()) T(std::forward<Args>(args)...);
				}

				void destroy(void *address)
				{
					destroy(static_cast<T *>(address));
				}

				void destroy(T *object)
				{
					assert(object != nullptr && "Invalid object");
					object->~T();
					deallocateMemory(object);
				}
			};
		}
	}
}