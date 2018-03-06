#pragma once
#include <pthread.h>
#include <event.h>
#include <json/json.h>
#include <errno.h>
#include <string.h>
#include "control.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "public.h"
#include <stdlib.h>
#include <iostream>

using namespace std;

typedef class mPthread
{
	public:
		mPthread(int sock_1);
		~mPthread()
		{
			delete _mControl;
			_mControl = NULL;
		}
	private:
		struct event_base *_base;//libevent
		int _sock_1;//socketpair 1
		map<int,struct event *> _event_map;//保存事件的map
		pControl _mControl;//控制台

		friend void *pthread_run(void *arg);
		friend void sock_1_cb(int fd,short event,void *arg);
		friend void cli_cb(int fd,short event,void *arg);
}mPthread,*pMPthread;
