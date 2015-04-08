#include "MemoryPool/ObjectPool.hpp"
#include "MemoryPool/ThreadSafeObjectPool.hpp"

struct Test1
{
	int a : 2;
	int b : 3;
	virtual void foo(int i) = 0;
	virtual void bar(char i) = 0;
	char c;
	bool d;
	int e;
	virtual ~Test1(){}
};

struct Test2 : public Test1
{
	Test2(int _f, char _g) : f(_f), g(_g) {}
	int f;
	virtual void foo(int i) { e = i; }
	virtual void bar(char i) { c = i; }
	char g;
	virtual ~Test2(){}
};

#include <list>
#include <chrono>
#include <iostream>

int main(int argc, char *argv[])
{
	using Clock = std::chrono::high_resolution_clock;
	using Duration = std::chrono::duration<float>;
	{
		Clock::time_point startTime = Clock::now();
		AO::MemoryPool::ObjectPool<Test2> pool;
		std::list<Test2 *> results;
		for (int i = 0; i < 3000000; ++i)
		{
			if (i % 2 == 0)
			{
				auto res = pool.create(i, 'p');
				res->foo(i);
				results.push_back(res);
			}
			else
			{
				auto res = pool.create(i, 'i');
				res->bar('e');
				results.push_front(res);
			}

			if (i % 3 == 0 && !results.empty())
			{
				pool.destroy(results.front());
				results.pop_front();
			}

		}
		for (auto &e : results)
		{
			pool.destroy(e);
		}
		const Clock::time_point endTime = Clock::now();
		const Duration duration = std::chrono::duration_cast<Duration>(endTime - startTime);
		std::cout << "Unsafe took " << duration.count() << "s" << std::endl;
	}
	{
		Clock::time_point startTime = Clock::now();
		AO::MemoryPool::ObjectPool<Test2> pool;
		std::list<Test2 *> results;
		for (int i = 0; i < 3000000; ++i)
		{
			if (i % 2 == 0)
			{
				auto res = pool.create(i, 'p');
				res->foo(i);
				results.push_back(res);
			}
			else
			{
				auto res = pool.create(i, 'i');
				res->bar('e');
				results.push_front(res);
			}

			if (i % 5 == 0 && !results.empty())
			{
				pool.destroy(results.front());
				results.pop_front();
			}

		}
		for (auto &e : results)
		{
			pool.destroy(e);
		}
		const Clock::time_point endTime = Clock::now();
		const Duration duration = std::chrono::duration_cast<Duration>(endTime - startTime);
		std::cout << "Thread-Safe took " << duration.count() << "s" << std::endl;
	}
	std::cout << "Success!" << std::endl;
	return EXIT_SUCCESS;
}