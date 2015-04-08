#pragma once

#include <utility>
#include <cassert>
#include <mutex>

#include "MemoryPool/BufferPool.hpp"
#include "MemoryPool/Private/SpinLock.hpp"

namespace AO
{
	namespace MemoryPool
	{
		inline namespace Version_1
		{
			template <typename T, std::size_t Alignment = 16, std::size_t NumberOfObjectPerChunk = 1024>
			class ThreadSafeObjectPool final : public BufferPool
			{
			private:
				// Attributes
				Private::SpinLock spinLock;

			public:
				// Constructors
				ThreadSafeObjectPool(void)
					: BufferPool(sizeof(T), Alignment, NumberOfObjectPerChunk)
				{
					return;
				}

				ThreadSafeObjectPool(ThreadSafeObjectPool const &) = delete;

				ThreadSafeObjectPool(ThreadSafeObjectPool &&) = default;

				// Assignment Operators
				ThreadSafeObjectPool &operator=(ThreadSafeObjectPool const &) = delete;

				ThreadSafeObjectPool &operator=(ThreadSafeObjectPool &&) = default;

				// Destructor
				virtual ~ThreadSafeObjectPool(void) = default;

				// Methods
				template <typename... Args>
				T *create(Args &&...args)
				{
					void *address = nullptr;
					{
						std::lock_guard<decltype(spinLock)> lock(spinLock);
						address = allocateMemory();
					}
					return new (address) T(std::forward<Args>(args)...);
				}

				void destroy(void *address)
				{
					destroy(reinterpret_cast<T *>(address));
				}

				void destroy(T *object)
				{
					assert(object != nullptr && "Invalid object");
					object->~T();
					std::lock_guard<decltype(spinLock)> lock(spinLock);
					deallocateMemory(object);
				}
			};
		}
	}
}