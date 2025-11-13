#pragma once
//这里是整个服务器开始的地方
#include<2_Base/Singleton.h>
#include<iostream>
#include<5_System/PoolSystem/PoolSystem.h>
#include<5_System/NetSystem/Base/NetManager.h>
class Server final : public Singleton<Server>
{
    friend class Singleton<Server>;
public:
    void Start()
    {
        std::cout << "服务器开始" << std::endl;
       
        std::cout<<"池大小：" << m_poolSystem.PoolSize<NetSystem>() << std::endl;
        auto netPtr = m_poolSystem.Alloc<NetSystem>();
        std::cout << "池大小：" << m_poolSystem.PoolSize<NetSystem>() << std::endl;
        netPtr->Init();
        auto netPtr2 = m_poolSystem.Alloc<NetSystem>(2);
        std::cout << "池大小：" << m_poolSystem.PoolSize<NetSystem>() << std::endl;
        netPtr2->Init();

    }
    void Update()
    {
        static auto lastTick = std::chrono::steady_clock::now();
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(now - lastTick).count() >= 1)
        {
            std::cout << "服务器刷新" << std::endl;// 每秒执行一次逻辑
            std::cout << "更新中查看池大小：" << m_poolSystem.PoolSize<NetSystem>() << std::endl;
            lastTick = now;
        }
    }
    void Stop()
    {
        std::cout << "服务器停止" << std::endl;
    }
private:
    PoolSystem m_poolSystem;
    Server() {}
};