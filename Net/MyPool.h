#pragma once
#include<string>
#include<mutex>
#include<vector>
#include<iostream>
#include <functional>
#include<format>
#include<span>
typedef std::byte byte;


class MemoryBlock
{
public:
	MemoryBlock(size_t size) :block_size(size), ptr(std::make_unique<byte[]>(size))
	{
		//ptr = std::make_unique<byte[]>(size);
		m_span = std::span<byte>(ptr.get(), block_size + sizeof(uint32_t));
	}
	MemoryBlock() = delete;
	MemoryBlock(const MemoryBlock&) = delete;
	MemoryBlock& operator=(const MemoryBlock&) = delete;

	//
	//void WriteData(byte data, size_t pos)
	//{
	//	if (pos >= block_size)
	//	{
	//		std::string wrong = "写入块位置超过指定大小：位置：" + std::to_string(pos) + "大小： " + std::to_string(block_size);
	//		throw std::runtime_error(wrong);
	//	}
	//	else
	//	{
	//		ptr[pos] = data;
	//	}
	//}

	byte* GetBytePtr() const
	{
		return ptr.get();
	}
	size_t GetWriteSize() const
	{
		return write_size;
	}

	void WriteData(const std::string& msg)
	{

		uint32_t len = static_cast<uint32_t>(msg.size());
		size_t total_size = sizeof(len) + msg.size();
		if (total_size >= block_size)
		{
			std::cout << "写入的数据超过块的大小，请重新分配块的大小" << std::endl;
			return;
		}
		std::memcpy(m_span.data(), &len, sizeof(len));
		std::memcpy(m_span.data(), &msg, msg.size());
		write_size = total_size;
	}
private:
	size_t block_size;
	size_t write_size = 0; //已写入的长度
	std::unique_ptr<byte[]> ptr;
	std::span<byte> m_span;
};

class MemoryPool
{
public:
	MemoryPool(size_t size, size_t num) : block_size(size), blick_num(num)
	{
		recycle_block = [this](MemoryBlock* ptr) {this->Recycle_Block(ptr); };
	}
	~MemoryPool() = default;

	MemoryPool() = delete;
	MemoryPool(const MemoryPool&) = delete; //禁止拷贝
	MemoryPool& operator=(const MemoryPool&) = delete; //禁止赋值

	std::function<void(MemoryBlock*)> recycle_block;

	std::unique_ptr<MemoryBlock, std::function<void(MemoryBlock*)>> Allocate()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		if (!m_pool.empty())
		{
			auto ptr = std::move(m_pool.back());
			m_pool.pop_back();
			std::cout << "分配了一块已有内存,当前内存池大小：" << m_pool.size() << std::endl;
			std::cout << std::format("asdasdasdas{}, asdasdasd:{}", m_pool.size(), m_pool.size()) << std::endl;

			return std::unique_ptr<MemoryBlock, std::function<void(MemoryBlock*)>>(ptr, recycle_block);
		}
		else
		{
			block_size++;
			if (block_size > max_Size)
			{
				throw std::runtime_error("内存池超过指定大小：" + max_Size);
			}
			std::cout << "分配了一块新的内存,当前内存池大小：" << m_pool.size() << std::endl;
			MemoryBlock* block = new MemoryBlock(block_size);
			return std::unique_ptr<MemoryBlock, std::function<void(MemoryBlock*)>>(block, recycle_block);
		}
	}
	void Recycle_Block(MemoryBlock* ptr)
	{
		if (!ptr) return;
		std::lock_guard<std::mutex> lock(m_mutex);
		m_pool.push_back(ptr);
		std::cout << "归还了一块内存,当前内存池大小：" << m_pool.size() << std::endl;
	}
	size_t BlockSize() const
	{
		return  block_size;
	}


private:
	size_t max_Size = 100000;
	size_t block_size;				//内存池的块大小
	size_t blick_num;
	mutable std::mutex m_mutex;		//互斥锁，标记为mutable表示在const函数中也是可修改的
	std::vector<MemoryBlock*> m_pool; //内存池，vector持有块的指针
};


class TestMemory
{
public:
	TestMemory() : m_memoryPool(MemoryPool(2048, 1024))
	{
	}
	void Test()
	{
		std::cout << "内存池测试" << std::endl;
		auto aaa = m_memoryPool.Allocate();
		for (size_t i = 0; i < m_memoryPool.BlockSize(); i++)
		{
			//aaa->WriteData(byte(2), i);
		}
	}
private:
	MemoryPool m_memoryPool;
};
