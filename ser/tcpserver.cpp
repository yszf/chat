#include "tcpserver.h"
#include "mpthread.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <string.h>

using namespace std;

TcpServer::tcpserver(char *ip,unsigned short port,int pth_num)
{
	_pth_num = pth_num;
	//初始化libevent
	_base = event_base_new();
	//创建服务器
	int listen_fd = socket(AF_INET,SOCK_STREAM,0);
	if(listen_fd == -1)
	{
		cerr << "listen_fd create fail;errno:" << endl;
		return;
	}
	_listen_fd = listen_fd;

	struct sockaddr_in saddr;
	memset(&saddr,0,sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = inet_addr(ip);
	
	//绑定
	int res = bind(listen_fd,(struct sockaddr *)&saddr,sizeof(saddr));
	if(res == -1)
	{
		cerr << "fd bind fail;errno:" << errno << endl;
		return;
	}

	//监听
	if(listen(_listen_fd,128)==-1)
	{
		cerr << "fd listen fail;errno:" << errno << endl;
		return;
	}
}

//监听回调函数
void listen_cb(int fd,short event,void *arg)
{
	pTcpServer mthis = (pTcpServer)arg;

	struct sockaddr_in caddr;
	socklen_t len = sizeof(caddr);
	int cli_fd = accept(fd,(struct sockaddr *)&caddr,&len);

	if(cli_fd == -1)
	{
		cerr << "accept fail;errno:" << errno << endl;
		exit(1);
	}

	//求监听量最小的socketpair的0端
	map<int,int>::iterator it = mthis->_pth_map.begin();
	int listen_num = it->second;
	int socket_1 = it->first;
	for(;it != mthis->_pth_map.end();++it)
	{
		if(it->second < listen_num)
		{
			socket_1 = it->first;
			break;
		}
	}	

	vector<Sock>::iterator it2 = mthis->_socket_pair_base.begin();
	int socket_0 = it2->_arr[0];
	for(;it2 != mthis->_socket_pair_base.end();++it2)
	{
		if(it2->_arr[1] == socket_1)
		{
			socket_0 = it2->_arr[0];
			break;
		}
	}

	//发送客户端套接字给监听量小的子线程
	char buff[7] = {0};
	sprintf(buff,"%d",cli_fd);
	strcat(buff,"|");
//	cout << "buff size:" << strlen(buff) << endl;
	cout << "tcpserver buff:" << buff << endl; 
	//设置发送缓冲区大小
	int optVal = 256;
	setsockopt(socket_0,SOL_SOCKET,SO_SNDBUF,(char*)&optVal,sizeof(int));
	if(-1 == send(socket_0,buff,strlen(buff),0))
	{
		cerr << "send fail;errno:" << errno <<endl;
		return;
	}	
}

void socket_0_cb(int fd,short event,void *arg)
{
	pTcpServer mthis = (pTcpServer)arg;
	//获取fd对应的内核缓冲区大小
	int optVal = 0;
	socklen_t optLen = sizeof(optVal);
//	if(getsockopt(fd,SOL_SOCKET,SO_RCVBUF,(char*)&optVal,&optLen)==-1)
//	{
//		cout << "getsockopt error;errno:" << errno << endl;
//	}
//	getsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char*)&optVal, &optLen);  
//	printf("fd,old recv buf is %d\n",optVal);

	//设置fd对应的内核接收缓冲区
	optVal = 256;
	if(setsockopt(fd,SOL_SOCKET,SO_RCVBUF,(char*)&optVal,optLen)==-1)
	{
		cout << "setsockopt errno;errno:" << errno << endl;
	}
	printf("fd,new recv buf is %d\n",optVal);

	char buff[256] = {0};
	if(recv(fd,buff,255,0) > 0)
	{
		int listen_num = 0;
		cout << "socket_0 buff:" << buff << endl;
		char *str = strtok(buff,"|");
		while(str != NULL)
		{
			cout << "sock_0 str:" << str << endl;
			listen_num = atoi(str);
			vector<Sock>::iterator it = mthis->_socket_pair_base.begin();
			for(;it != mthis->_socket_pair_base.end();++it)
			{
				if(it->_arr[0] == fd)
				{
					//更新监听量
					mthis->_pth_map[it->_arr[1]] = listen_num;
				}
			}
			cout << "sock_0 listen_num:" << listen_num << endl;
			str = strtok(NULL,"|");
		}
	}
}

void TcpServer::create_socket_pair()
{
	for(int i=0;i < _pth_num;++i)
	{
		int arr[2];
		int ret = socketpair(AF_UNIX,SOCK_STREAM,0,arr);
		Sock sock(arr);
		//将socketpair放入vector
		_socket_pair_base.push_back(sock);
		//将socketpair 1端加入到_map
		_pth_map[sock._arr[1]] = 0;
		//将socketpair 0端监听起来
		struct event *socket_0_event = event_new(_base,sock._arr[0],EV_READ|EV_PERSIST,socket_0_cb,this);
		//加入到libevent->socket_0_cb()
		event_add(socket_0_event,NULL);
	}
}

void TcpServer::create_pth_base()
{
	for(int i = 0;i < _pth_num;++i)
	{
		new mPthread(_socket_pair_base[i]._arr[1]);
	}
}

void TcpServer::run()
{
	//创建socket_pair
	create_socket_pair();

	//启动子线程
	create_pth_base();

	//将_listen_fd加入到libevent
	struct event *listen_event = event_new(_base,_listen_fd,EV_READ | EV_PERSIST,listen_cb,this);
	if(NULL == listen_event)
	{
		cerr << "event new fail;errno:" << errno << endl;
	}
	//添加监听事件
	event_add(listen_event,NULL);

	//启动循环监听
	event_base_dispatch(_base);
	event_free(listen_event);
	event_base_free(_base);
}
