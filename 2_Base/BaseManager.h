#pragma once
#include<2_Base/Singleton.h>
#include<5_System/NetSystem/Base/NetManager.h>
#include<5_System/PoolSystem/PoolSystem.h>
#include<vector>
class BaseManager
{
public:
    virtual void OnInit() {};

    virtual void OnEnable() {};

    virtual void OnStart() {};

    virtual void OnUpdate() {};

    virtual void OnStop() {};

    virtual void OnDisable() {};

private:
    BaseManager(){}

};

class SystemManager : public BaseManager
{
public:
    /// <summary>
    /// 初始化所有的System
    /// </summary>
    void InitAllSystem()
    {
        //NetSystem* netSystem = new NetSystem();
        //m_systemBase.push_back(netSystem);

        //PoolSystem* pool = new PoolSystem();
        //NetSystem* netSystem2 = pool->Alloc<NetSystem>();

    }

    void InitSystemByType();

    void StartSystem();

    void UpdateSystem();

    void PauseSystem();

    void StopSystem();

private:
    std::vector<SystemBase*> m_systemBase;

};