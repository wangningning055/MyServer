#pragma once
//这是System基类
class SystemDataBase
{

};

class SystemBase
{
public:
	virtual void Init() {}
	virtual void Start() {}
	virtual void Update() {}
	virtual void Stop() {}
	virtual void Destory() {}
protected:
};


