#include <iostream>
#include <errno.h>
#include "tcpserver.h"
#include <stdlib.h>

using namespace std;

int main(int argc,char **argv)
{
	//ip port pth_num
	if(argc != 4)
	{
		cerr << "arg errro:" << errno << endl;
		return -1;
	}

	char *ip = argv[1];
	unsigned short port = atoi(argv[2]);
	int pth_num = atoi(argv[3]);
	//构造服务器对象
	TcpServer ser(ip,port,pth_num);

	//运行服务器
	ser.run();

	return 0;
}
