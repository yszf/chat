#include "mpthread.h"

void cli_cb(int fd,short event,void *arg)
{
	pMPthread mthis = (pMPthread)arg;
	char buff[128] = {0};
	int optVal = 0;  
	socklen_t optLen = sizeof(optVal);  
	if(getsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char*)&optVal, &optLen)==-1)
	{
		cout << "getsockopt error;errno:" << errno << endl;
	}
	printf("fd,old recv buf is %d\n", optVal);
   
	// 设置fd对应的内核接收缓冲区大小  
	optVal = 128;  
	if(setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char*)&optVal, optLen)==-1)
	{
		cout << "setsockopt errno;errno:" << errno << endl;	
	}  
	printf("fd,new recv buf is %d\n", optVal);

	if(recv(fd,buff,127,0) > 0)
	{
		//解析数据
		Json::Value val;
		Json::Reader read;
		if(read.parse(buff,val)==-1)
		{
			cerr << "read fail;errno:" << errno << endl;
			return;
		}
	
		mthis->_mControl->process(fd,buff);	
		if(val["type"].asInt() == MSG_TYPE_EXIT)
		{
			event_free(mthis->_event_map[fd]);////////////////////重点
			mthis->_event_map.erase(fd);
		}
	}
	else
	{
		cout << "recv fail" << endl;
		Json::Value val;
		val["type"] = MSG_TYPE_EXIT;
		strcpy(buff,val.toStyledString().c_str());
		mthis->_mControl->process(fd,buff);
		event_free(mthis->_event_map[fd]);
		mthis->_event_map.erase(fd);

	}
}
 
void sock_1_cb(int fd,short event,void *arg)
{
	pMPthread mthis = (pMPthread)arg;
	// 获取fd对应的内核接收缓冲区大小  
	int optVal = 0;  
	socklen_t optLen = sizeof(optVal);  
//	if(getsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char*)&optVal, &optLen)==-1)
//	{
//		cout << "getsockopt error;errno:" << errno << endl;
//	}
//	printf("fd,old recv buf is %d\n", optVal);
   
	// 设置fd对应的内核接收缓冲区大小  
	optVal = 256;  
	if(setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char*)&optVal, optLen)==-1)
	{
		cout << "setsockopt errno;errno:" << errno << endl;	
	}  
//	printf("fd,new recv buf is %d\n", optVal);
	
	char buff[256] = {0};
	if(recv(fd,buff,255,0) > 0)
	{
		cout << "sock_1 buff:" << buff << endl;
		char *str = strtok(buff,"|");
		while(str != NULL)
		{
			int cli_fd = atoi(str);
			cout << "cli_fd:" << cli_fd << endl;	
			//创建客户端事件
			struct event *cli_event = event_new(mthis->_base,cli_fd,EV_READ|EV_PERSIST,cli_cb,mthis);
			if(cli_event == NULL)
			{
				cerr << "cli event create fail;errno:" << errno << endl;
				exit(1);
			}
			//将客户端事件保
			mthis->_event_map[cli_fd] = cli_event;
		
			event_add(cli_event,NULL);
			str = strtok(NULL,"|");
		}

		int listen_num = mthis->_event_map.size();
		cout << "pthread listen_num:" << listen_num << endl;		
		//发送给主线程监听量
		char buff2[7] = {0};
		sprintf(buff2,"%d",listen_num);
		strcat(buff2,"|");
		//设置发送缓冲区大小
		setsockopt(mthis->_sock_1,SOL_SOCKET,SO_SNDBUF,(char*)&optVal,sizeof(int));
		if(-1 == send(mthis->_sock_1,buff2,strlen(buff2),0))
		{
			cerr << "send fail;errno:" << errno << endl;
			return;
		}
	}
}

void *pthread_run(void *arg)
{
	pMPthread mthis = (pMPthread)arg;
	//将sock_1加入到监听队列
	struct event *socket_1_event = event_new(mthis->_base,mthis->_sock_1,EV_READ|EV_PERSIST,sock_1_cb,mthis);
	event_add(socket_1_event,NULL);
	event_base_dispatch(mthis->_base);
}

mPthread::mPthread(int sock_1)
{
	_base = event_base_new();
	_sock_1 = sock_1;
	_mControl = new Control();
	//建立线程
	pthread_t pid;
	pthread_create(&pid,NULL,pthread_run,this);
}
