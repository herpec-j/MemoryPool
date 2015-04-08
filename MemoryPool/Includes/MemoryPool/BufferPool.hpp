#pragma once

#include <cstddef>
#include <forward_list>
#include <cassert>

namespace BlindingTechnologies
{
	namespace BlindingEngine
	{
		inline namespace Version_1
		{
			class BufferPool
			{
			protected:
				// Inner classes
				class Link final
				{
				private:
					// Attributes
					Link *next = nullptr;

				public:
					// Constructors
					Link(void) = default;

					Link(Link const &) = default;

					Link(Link &&) = default;

					// Assignment Operators
					Link &operator=(Link const &) = default;

					Link &operator=(Link &&) = default;

					// Destructor
					~Link(void) = default;

					// Methods
					inline void initialize(void)
					{
						next = nullptr;
					}

					inline bool empty(void) const
					{
						return next == nullptr;
					}

					inline Link *pop(void)
					{
						Link *top = next;
						if (next != nullptr)
						{
							next = next->next;
						}
						return top;
					}

					inline void push(Link *newLink)
					{
						assert(newLink != nullptr && "Invalid link");
						newLink->next = next;
						next = newLink;
					}
				};

				class Chunk final
				{
					// Friendships
					friend BufferPool;

				private:
					// Attributes
					std::size_t numberOfEmptySlots = 0;

					Link emptySlotsList;

				public:
					// Constructors
					Chunk(void) = default;

					Chunk(Chunk const &) = default;

					Chunk(Chunk &&) = default;

					// Assignment Operators
					Chunk &operator=(Chunk const &) = default;

					Chunk &operator=(Chunk &&) = default;

					// Destructor
					~Chunk(void) = default;
				};

				class ChunkHeader
				{
					// Friendships
					friend BufferPool;

				private:
					// Attributes
					std::size_t index = 0;

				public:
					// Constructors
					ChunkHeader(void) = default;

					ChunkHeader(ChunkHeader const &) = default;

					ChunkHeader(ChunkHeader &&) = default;

					// Assignment Operators
					ChunkHeader &operator=(ChunkHeader const &) = default;

					ChunkHeader &operator=(ChunkHeader &&) = default;

					// Destructor
					~ChunkHeader(void) = default;
				};

				// Attributes
				std::forward_list<Chunk *> chunks;

				std::size_t numberOfObjectsPerChunk = 0;

				std::size_t objectSize = sizeof(Link);

				std::size_t numberOfChunks = 0;

				std::size_t numberOfFreeObjects = 0;

				std::size_t numberOfObjects = 0;

				std::size_t chunkAlignment = 0;

				std::size_t objectAlignment = 0;

				// Constructors
				BufferPool(void) = delete;

				inline BufferPool(std::size_t objectSize, std::size_t objectAlignment, std::size_t chunkSize)
					: numberOfObjectsPerChunk(chunkSize)
				{
					if (objectSize > this->objectSize)
					{
						this->objectSize = objectSize;
					}
					this->objectAlignment = objectAlignment - ((sizeof(ChunkHeader) + this->objectSize) % objectAlignment);
					if (this->objectAlignment == objectAlignment)
					{
						this->objectAlignment = 0;
					}
					chunkAlignment = objectAlignment - ((sizeof(ChunkHeader) + sizeof(Chunk)) % objectAlignment);
					if (this->chunkAlignment == objectAlignment)
					{
						chunkAlignment = 0;
					}
				}

				BufferPool(BufferPool const &) = delete;

				BufferPool(BufferPool &&) = default;

				// Assignment Operators
				BufferPool &operator=(BufferPool const &) = delete;

				BufferPool &operator=(BufferPool &&) = default;

				// Destructor
				inline virtual ~BufferPool(void)
				{
					destroy();
				}

				// Methods
				inline void allocateChunk(void)
				{
					const std::size_t chunkSize = getChunkSize();
					Chunk *newChunk = reinterpret_cast<Chunk *>(new unsigned char[chunkSize]);
					newChunk->numberOfEmptySlots = numberOfObjectsPerChunk;
					newChunk->emptySlotsList.initialize();
					chunks.push_front(newChunk);
					numberOfFreeObjects += numberOfObjectsPerChunk;
					++numberOfChunks;
					unsigned char *data = reinterpret_cast<unsigned char *>(newChunk + 1);
					data += chunkAlignment;
					for (std::size_t index = 0; index < numberOfObjectsPerChunk; ++index)
					{
						reinterpret_cast<ChunkHeader *>(data)->index = index;
						data += sizeof(ChunkHeader);
						newChunk->emptySlotsList.push(reinterpret_cast<Link *>(data));
						data += objectSize + objectAlignment;
					}
				}

				inline void *allocateMemory(void)
				{
					if (numberOfFreeObjects == 0)
					{
						allocateChunk();
					}
					for (Chunk *chunk : chunks)
					{
						if (chunk->numberOfEmptySlots != 0)
						{
							Link *address = chunk->emptySlotsList.pop();
							--chunk->numberOfEmptySlots;
							--numberOfFreeObjects;
							++numberOfObjects;
							return static_cast<void *>(address);
						}
					}
					assert(!"Never reached");
					return nullptr;
				}

				inline void deallocateMemory(void *address)
				{
					assert(address != nullptr && "Invalid address");
					unsigned char *data = static_cast<unsigned char *>(address) - sizeof(ChunkHeader);
					Chunk *chunk = reinterpret_cast<Chunk *>(data - (reinterpret_cast<ChunkHeader *>(data)->index * (sizeof(ChunkHeader) + objectSize + objectAlignment)) - chunkAlignment) - 1;
					++chunk->numberOfEmptySlots;
					chunk->emptySlotsList.push(reinterpret_cast<Link *>(address));
					++numberOfFreeObjects;
					--numberOfObjects;
					if (chunk != chunks.front())
					{
						chunks.remove(chunk);
						chunks.push_front(chunk);
					}
					if (chunk->numberOfEmptySlots == numberOfObjectsPerChunk)
					{
						delete[] reinterpret_cast<unsigned char const *>(chunk);
						chunks.pop_front();
						numberOfFreeObjects -= numberOfObjectsPerChunk;
					}
				}

				inline void destroy(void)
				{
					for (Chunk const *chunk : chunks)
					{
						delete[] reinterpret_cast<unsigned char const *>(chunk);
					}
				}

				inline std::size_t getChunkSize(void) const
				{
					return chunkAlignment + sizeof(Chunk) + numberOfObjectsPerChunk * (objectSize + objectAlignment + sizeof(ChunkHeader));
				}
			};
		}
	}
}
