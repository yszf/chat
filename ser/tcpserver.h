#pragma once

#include <event.h>
#include <json/json.h>
#include <stdlib.h>
#include <map>
#include <vector>

using namespace std;

typedef struct Sock
{
	public:
		Sock(int arr[2])
		{
			_arr[0] = arr[0];
			_arr[1] = arr[1];
		}
//	private:
		int _arr[2];
//		friend class TcpServer;
}Sock;

typedef class tcpserver
{
	public:
		tcpserver(char *ip,unsigned short port,int pth_num);
	//	~tcpserver();
		void run();
		void create_socket_pair();
		void create_pth_base();
	private:
		struct event_base *_base;//libevent
		int _listen_fd;//监听套接字
		int _pth_num;//线程个数
		map<int,int> _pth_map;//线程压力统计
		vector<Sock> _socket_pair_base;//socket_pair

		friend void listen_cb(int fd,short event,void *arg);
		friend void socket_0_cb(int fd,short event,void *arg);
}TcpServer,*pTcpServer;
