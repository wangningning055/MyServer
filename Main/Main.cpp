#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0A00
#include<iostream>
#include<string>
#include<boost/asio.hpp>
#include<deque>
#include <unordered_map>
#include "Net/MyPool.h"

using tcp = boost::asio::ip::tcp;

//这是一个连接类，一个客户端一个链接
class Session : public std::enable_shared_from_this<Session>
{
public:
	Session(tcp::socket socket, int id) :_socket(std::move(socket)), _id(id) {}
	int id() {}
	void Start()
	{
		//开启第一次读取等待
		do_read();
	}
	void Send(std::string msg)
	{
		bool isQueueEmpty = write_queue.empty();
		write_queue.push_back(msg);
		//如果队列为空了，就再次开启新的等待
		if (isQueueEmpty)
		{
			do_write();
		}
	}
	std::function<void(int)> on_disconnect;
private:
	tcp::socket _socket;
	int _id;
	const int maxLength = 1024;
	char _data[1024];
	std::deque<std::string> write_queue;

	void do_read()
	{
		//因为这个类是通过智能指针来创建的，所以为了保证回调的时候类依然存在，需要用回调把自己捕获
		auto self = shared_from_this();


		//read_some表示自定义读，read表示自动读
		//这里使用read_some，只要读到数据就会触发，不用read是因为read填满一个buffer才触发，write同理，不过write追求完整，不用some
		_socket.async_read_some(
			boost::asio::buffer(_data, maxLength),
			[this, self](boost::system::error_code e, std::size_t length)
			{
				if (!e)
				{
					//读取成功
					std::string msg(_data, length);
					std::cout << "客户端:" << _id << "接收到数据：" << msg << std::endl;
					//立即开启下一次读取等待
					do_read();
				}
				else
				{
					if (e == boost::asio::error::eof)
					{
						std::cout << "对方断开了连接" << std::endl;
					}
					//读取失败
					std::cout << "客户端:" << _id << "读取失败：" << std::endl;
					if (on_disconnect)
						on_disconnect(_id);

				}
			}
		);
	}
	void do_write()
	{
		if (write_queue.empty()) return;
		auto self = shared_from_this();
		//这里写操作一般比较稳定，所以直接用write而不是write_some
		boost::asio::async_write(
			_socket,
			boost::asio::buffer(write_queue.front()),
			[this, self](boost::system::error_code e, std::size_t size)
			{
				if (!e)
				{
					write_queue.pop_front();
					if (!write_queue.empty())
					{
						do_write();
					}
				}
				else
				{
					//写入失败
					std::cout << "客户端:" << _id << "写入失败：" << std::endl;
					if (on_disconnect)
						on_disconnect(_id);
				}
			}
		);
	}

};

//服务器类，管理所有的链接
class Server
{
public:
	Server(boost::asio::io_context& io, short port) : _io(io), _acceptor(io, tcp::endpoint(tcp::v4(), port)) {

		//开始等待连接
		Do_accept();
	}

	//广播消息
	void Broadcast(const std::string msg)
	{
		for (auto& [id, session] : _sessions)
		{
			session->Send(msg);
		}
	}

private:
	boost::asio::io_context& _io;

	tcp::acceptor _acceptor;

	//这里存着所有的连接
	std::unordered_map<int, std::shared_ptr<Session>> _sessions;
	int next_id = 0;

	//开始等待客户端连接
	void Do_accept()
	{
		std::cout << "开始等待客户端连接，监听端口：8080" << std::endl;
		_acceptor.async_accept(
			[this](boost::system::error_code e, tcp::socket socket)
			{
				if (!e)
				{
					int id = next_id++;
					auto session = std::make_shared<Session>(std::move(socket), id);
					session->on_disconnect = [this](int sid)
						{
							_sessions.erase(sid);
							std::cout << "移除客户端：" << sid << std::endl;
						};
					_sessions[id] = session;
					session->Start();
					std::cout << "正式建立客户端连接，id：" << id << std::endl;
				}
				else
				{
					std::cout << "客户端尝试连接出错" << std::endl;
				}
			}
		);
	}
};




int main()
{
	//SetConsoleOutputCP(CP_UTF8);  // 设置控制台输出为 UTF-8
	std::string input;
	std::cout << "这是我的服务器:" << std::endl;

	try
	{
		TestMemory memory;
		memory.Test();
		std::cout << "测试1结束" << std::endl;
		memory.Test();
		std::cout << "测试2结束" << std::endl;
		//boost::asio::io_context io;
		//Server server(io, 8080);
		//std::thread t([&io]() { io.run(); });
		//while (true) {
		//	std::cout << ">";
		//	std::getline(std::cin, input);
		//	if (input == "exit")
		//	{
		//		std::cout << "服务器终止" << std::endl;
		//		break;
		//	}
		//	else
		//	{
		//		std::cout << "广播消息：" << input << std::endl;
		//		server.Broadcast(input);
		//	}
		//}
		//io.stop();
		////等待t线程执行完毕
		//t.join();

	}
	catch (std::exception& e)
	{
		std::cerr << "服务器出现异常,已退出：" << e.what() << std::endl;

	}
	system("pause");
	return 0;
}
