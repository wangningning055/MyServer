#pragma once
///这是一个单例模板
template <typename T> class Singleton
{
public:
    static T& Instance()
    {
        static_assert(std::is_final_v<T>, "单例不可被继承，必须标识为final");

        static T instance;
        return instance;
    }
    //禁止拷贝
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

    //禁止移动
    Singleton(Singleton&&) = delete;
    Singleton&& operator=(Singleton&&) = delete;

protected:
    Singleton() = default;
    ~Singleton() = default;
private:
};


//这是一个单例类示范例
class SingltonTest final : public Singleton<SingltonTest>
{
    friend class Singleton<SingltonTest>;
public:
    int data;
    void DoSmothing(){}
private:
    SingltonTest(){}
};


