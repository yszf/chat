#include "cli_fun.h"

void main_menu()
{
	system("clear");
	printf("\n\n\n");
	printf("\t\t-----------------Chating main---------------\n");
	printf("\t\t\t1	regist									\n");
	printf("\t\t\t2	login									\n");
	printf("\t\t\t3	exit									\n");
	printf("\t\t--------------------------------------------\n\n");
	printf("\t\tchoose(1-3):");
}

void sub_menu()
{	
	printf("\n\n\n");
	printf("\t\t-------------------login success----------------\n");
	printf("\t\t\t1	talk_one									\n");
	printf("\t\t\t2	talk_group									\n");
	printf("\t\t\t3	get_list									\n");
	printf("\t\t\t4	exit										\n");
	printf("\t\t--------------------------------------------\n\n");
	printf("\t\tchoose(1-4):");
}


void myexit(int fd)
{
	Json::Value val;
	val["type"] = MSG_TYPE_EXIT;

	int optVal = 128;
	setsockopt(fd,SOL_SOCKET,SO_RCVBUF,(char*)&optVal,4);
	
	//发送消息
	if(send(fd,val.toStyledString().c_str(),strlen(val.toStyledString().c_str()),0) == -1)
	{
		cerr << "send fail;errno:" << errno << endl;
		return;
	}
//	cout << "val:" << val << endl;
	char buff[128] = {0};
	if(recv(fd,buff,127,0) > 0)
	{
		//解析json
		Json::Value res;
		Json::Reader read;

		if(read.parse(buff,res)==-1)
		{
			cerr << "read fail;errno:" << errno << endl;
			return;
		}
		if(strncmp(res["reply"].asString().c_str(),"success",6)==0)
		{
			printf("您已经成功退出聊天！！！\n");
		}	
	}
	close(fd);
}

void * pth_talkone(void *arg)
{
	int fd = (int)arg;
	//接收消息
	while(1)
	{
		char msg[128] = {0};
		if(recv(fd,msg,127,0) > 0)
		{
			//解析json
			Json::Value res;
			Json::Reader read;
			if(read.parse(msg,res)==-1)
			{
				cerr << "read fail;errno:" << errno << endl;
				break;
			}
			//判断是否退出
			if(strncmp(res["msg"].asString().c_str(),"exit",4)==0)
			{
		//		cout << "pthread talkone out." << endl;
				break;
			}

			if(strncmp(res["msg"].asString().c_str(),"fail",4)==0)
			{
				cout << "您的好友不存在！" << endl;
			}
			else if(strncmp(res["msg"].asString().c_str(),"success",6)==0)
			{
				cout << "消息发送成功，您的好友不在线！" << endl;
			}
			else
			{
				cout << "name:" << res["name"].asString() << ",msg:" << res["msg"].asString() << endl;
			}
		}
	}
}

void talk_one(int fd)
{
	system("clear");
	printf("\t\t欢迎进入私聊界面，哈哈\n\n");
	Json::Value val;

	pthread_t pid;
	pthread_create(&pid,NULL,pth_talkone,(void *)fd);

	while(1)
	{
		char msg[128] = {0};
		printf("please input friend name:\n");
		char name[20] = {0};
		cin >> name;
		val["name"] = name;
		val["type"] = MSG_TYPE_TALKONE;
		printf("plese input message:\n");
		cin >> msg;

		val["msg"] = msg;
		
		//发送消息
		if(send(fd,val.toStyledString().c_str(),strlen(val.toStyledString().c_str()),0)==-1)
		{
			cerr << "send fail;errno:" << errno << endl;
		}
		
		//退出私聊
		if(strncmp(msg,"exit",4)==0)
		{
			cout << "main talk one." << endl;
			break;
		}
	}
}

void * pth_talkgroup(void *arg)
{
	int fd = (int)arg;
	while(1)
	{
		char msg[128] = {0};
		if(recv(fd,msg,127,0) > 0)
		{
			//解析json
			Json::Value res;
			Json::Reader read;
			if(read.parse(msg,res)==-1)
			{
				cerr << "read fail;errno:" << errno << endl;
				break;
			}
			//判断是否退出
			if(strncmp(res["reply"].asString().c_str(),"exit",4)==0)
			{
		//		cout << "pthread talkgroup out." << endl;
				break;
			}
			cout << "name:" << res["name"].asString() << ",msg:" << res["msg"].asString() << endl;
		}

	}

}
void talk_group(int fd)
{
	system("clear");
	printf("\t\t欢迎进入群聊界面\n\n");
	Json::Value val;
	val["type"] = MSG_TYPE_TALKGROUP;
	pthread_t pid;
	pthread_create(&pid,NULL,pth_talkgroup,(void *)fd);
	while(1)
	{
		char msg[128] = {0};
		printf("plese input message:\n");
		cin >> msg;
		val["msg"] = msg;
		//发送消息
		if(send(fd,val.toStyledString().c_str(),strlen(val.toStyledString().c_str()),0)==-1)
		{
			cerr << "send fail;errno:" << errno << endl;
		}

		//退出群聊
		if(strncmp(msg,"exit",4)==0)
		{
			cout << "main talkgroup out." << endl;
			break;
		}
	}
}

void get_list(int fd)
{
	system("clear");
	printf("\t\t当前在线的朋友列表\n\n");
	Json::Value val;
	val["type"] = MSG_TYPE_GETLIST;

	//发送消息
	if(send(fd,val.toStyledString().c_str(),strlen(val.toStyledString().c_str()),0) == -1)
	{
		cerr << "send fail;errno:" << errno << endl;
	}

	//接收消息
	char buff[128] = {0};
	if(recv(fd,buff,127,0) > 0)
	{
		//解析json
		Json::Value res;
		Json::Reader read;

		char name[100] = {0};

		if(read.parse(buff,res)==-1)
		{
			cerr << "read fail;errno:" << errno << endl;
			return;
		}
		//接收离线消息	
		strcpy(name,res["name"].asString().c_str());
		char *str = strtok(name,"|");
		while(str != NULL)
		{
			cout << "name:" << str << endl;
			str = strtok(NULL,"|");
		}

		if(strncmp(res["reply"].asString().c_str(),"fail",4)==0)
		{
			cout << "没有在线好友！" << endl;
			return;
		}

	}
}

void login(int fd)
{
	system("clear");
	printf("\t\t当前是登录页面\n\n");
	Json::Value val;
	printf("please input name:\n");
	char name[20] = {0};
	cin >> name;
	val["name"] = name;
	printf("please input password:\n");
	char pw[20] = {0};
	cin >> pw;
	val["pw"] = pw;
	val["type"] = MSG_TYPE_LOGIN;

	//发送消息
	if(send(fd,val.toStyledString().c_str(),strlen(val.toStyledString().c_str()),0) == -1)
	{
		cerr << "send fail;errno:" << errno << endl;
		return;
	}

	//接收消息
	char buff[256] = {0};
	if(recv(fd,buff,255,0) > 0)
	{
		//解析json
		Json::Value res;
		Json::Reader read;

		char info[200] = {0};

		if(read.parse(buff,res)==-1)
		{
			cerr << "read fail;errno:" << errno << endl;
			return;
		}
		//接收离线消息
		strcpy(info,res["info"].asString().c_str());
		char *str = strtok(info,"|");
		while(str != NULL)
		{
			//解析json
			Json::Value res2;
			Json::Reader read;

			if(read.parse(str,res2)==-1)
			{
				cerr << "read fail;errno:" << errno << endl;
				return;
			}
			cout << "sname:" << res2["sname"].asString() << 
				",msg:" << res2["msg"] << endl;
			str = strtok(NULL,"|");
		}

		if(strncmp(res["reply"].asString().c_str(),"fail",4)==0)
		{
			cout << "登录失败！" << endl;
			return;
		}

		if(strncmp(res["reply"].asString().c_str(),"success",6)==0)
		{
			while(1)
			{
				sub_menu();
				int num;
				cin >> num;
				if(!cin.good())
				{
					exit(0);	
				}
				switch(num)
				{
					case 1:
						talk_one(fd);
						system("clear");
						break;
					case 2:
						talk_group(fd);
						system("clear");
						break;
					case 3:
						get_list(fd);
						break;
					case 4:
						myexit(fd);
						exit(0);
					default:
						printf("input error,please again!\n");
				}
			}
		}
	}
}

void regist(int fd)
{
	system("clear");
	printf("\t\t当前是注册页面\n\n");
	printf("please input name:\n");
	Json::Value val;
	char name[20];
	cin >> name;
	if(strncmp(name,"exit",4)==0)
	{
		return;
	}

	val["name"] = name;
	printf("please input password:\n");
	char pw[20];
	cin >> pw;	
	val["pw"] = pw;
	if(strncmp(pw,"exit",4)==0)
	{
		return;
	}

	val["type"] = MSG_TYPE_REGISTER;
	cout << "val:" << val;
	
	//发送数据
	if(send(fd,val.toStyledString().c_str(),strlen(val.toStyledString().c_str()),0)==-1)
	{
		cerr << "send fail;errno:" << errno << endl;
		return;
	}
	char buff[128] = {0};
	if(recv(fd,buff,127,0) > 0)
	{
		//解析json
		Json::Value res;
		Json::Reader read;
		if(read.parse(buff,res)==-1)
		{
			cerr << "read fail;errno:" << errno << endl;
			return;
		}

		if(strncmp(res["reply"].asString().c_str(),"success",6)==0)
		{
			printf("注册成功！\n");
		}
		else if(strncmp(res["reply"].asString().c_str(),"fail",4)==0)
		{
			cout << "此用户已注册\n" << endl;
		}
		else
		{
			cout << "注册失败\n" << endl;
		}
	}
}

void run(char* ip,unsigned short port)
{
//while(1)
//{
	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	assert(sockfd != -1);
	cout << "sockfd:" << sockfd << endl;
	struct sockaddr_in saddr;
	memset(&saddr,0,sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = inet_addr(ip);
	
	if(-1 == connect(sockfd,(struct sockaddr*)&saddr,sizeof(saddr)))
	{
		cerr << "cli_fd connect fail;errno:" << errno << endl;
		return;
	}
//}

	while(1)
	{
		main_menu();
		int num;
		cin >> num;
		if(!cin.good())
		{
			exit(0);
		}
		switch(num)
		{
			case 1:
				regist(sockfd);
				break;
			case 2:
				login(sockfd);
				break;
			case 3:
				myexit(sockfd);
				exit(0);
			default:
				printf("input error,please again!\n");
		}
	}
}
