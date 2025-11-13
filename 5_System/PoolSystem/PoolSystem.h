#pragma once
//这是一个内存池管理类，所有的堆分配都要从这里拿
//
#include<memory>
#include<5_System/1_Base/SystemBase.h>
#include <mutex>
#include<unordered_map>
#include <vector>
#include <typeindex>
#include <functional>
class PoolSystem final : public SystemBase
{
public:
	template <typename T, typename... Args> 
	std::shared_ptr<T> Alloc(Args&&... args)
	{
		auto& pool = GetPool<T>();
		std::lock_guard<std::mutex> lock(pool.mutex);

		T* target = nullptr;
		if (!pool.pool.empty())
		{
			target = pool.pool.back();
			pool.pool.pop_back();
			new (target) T(std::forward<Args>(args)...);
		}
		else
		{
			target = new T(std::forward<Args>(args)...);
		}

		auto deleter = [&pool](T* ptr)
			{
				std::lock_guard<std::mutex> lock(pool.mutex);
				ptr -> ~T();
				pool.pool.push_back(ptr);
			};

		return std::shared_ptr<T>(target, deleter);
	}

	template <typename T>
	size_t PoolSize() const
	{
		auto& pool = GetPool<T>();
		std::lock_guard<std::mutex> lock(pool.mutex);
		return pool.pool.size();
	}
	
private:
	template<typename T>
	struct TypePool
	{
		std::vector<T*> pool;
		std::mutex mutex;
	};

	template<typename T>
	static TypePool<T>& GetPool()
	{
		static TypePool<T> pool;
		return pool;
	}
	mutable std::mutex m_mutex;
	mutable std::unordered_map<std::type_index, std::vector<void*>*> m_pools;
};

class PoolSystemData final : public SystemDataBase
{

};