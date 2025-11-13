#pragma once
#include<5_System/1_Base/SystemBase.h>
#include<iostream>
class NetSystem final : public SystemBase
{
public:
	NetSystem()
	{
		std::cout << "我是网络系统构造1" << std::endl;

	}
	NetSystem(int id)
	{
		std::cout << "我是网络系统构造2" << std::endl;

	}
	void Init() override
	{
		std::cout << "我是网络系统初始化" << std::endl;
	}
	void Start() override
	{

	}
	void Stop() override
	{

	}
	void Destory() override
	{

	}
	~NetSystem()
	{
		std::cout << "网络系统销毁了" << std::endl;
	}
private:

};

class NetSystemData final : public SystemDataBase
{
public:


private:
};


