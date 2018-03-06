#include "view_talkone.h"

using namespace std;

view_talkone::view_talkone()
{}

void view_talkone::process(Json::Value val,int fd)
{	
	MYSQL *_mqcon = mysql_init((MYSQL*)0);
	//建立连接
	if(!mysql_real_connect(_mqcon,"127.0.0.1","root","root",NULL,3306,NULL,0))
	{
		cerr << "sql connect fail;errno:" << errno << endl;
		return;
	}
	//选择数据库
	if(mysql_select_db(_mqcon,"chat"))
	{
		cerr << "select database fail:errno:" << errno << endl;
	}
	_cli_fd = fd;
	string name = val["name"].asString();
	string msg = val["msg"].asString();

	if(strncmp(msg.c_str(),"exit",4)==0)
	{
		_reply = "exit";
		mysql_close(_mqcon);
		response();
		return;
	}

	MYSQL_RES *mq_res;
	MYSQL_ROW mq_row;

	string sql1 = "select * from usr where name='";
	sql1.append(name.c_str());
	sql1.append("'");
	cout << "sql1:" << sql1 << endl;
	//执行查询语句
	if(mysql_real_query(_mqcon,sql1.c_str(),sql1.size()))
	{
		cerr << "query fail;errno:" << errno << endl;
		return;
	}
	//接收查询的返回值
	mq_res = mysql_store_result(_mqcon);
	if(mq_res == NULL)
	{
		cerr << "no resultset;errno:" << errno << endl;
		return;
	}
	//得到查询的记录数
	int num = mysql_num_rows(mq_res);
	if(num == 0)
	{
		_reply = "fail";
		//释放数据库连接
		mysql_close(_mqcon);
		//响应给客户端
		response();	
		return;
	}
	string sql2 = "select * from online where name='";
	sql2.append(name.c_str());
	sql2.append("' and fd != ");
	sql2.append(itos(fd));
	cout << "sql2:" << sql2 << endl;
	
	//执行查询语句
	if(mysql_real_query(_mqcon,sql2.c_str(),sql2.size()))
	{
		cerr << "query fail;errno:" << errno << endl;
		return;
	}

	//接收查询的返回值
	mq_res = mysql_store_result(_mqcon);
	if(mq_res == NULL)
	{
		cerr << "no resultset;errno:" << errno << endl;
		return;
	}

	//查询发送人的名字
	MYSQL_RES *mq_res2;
	MYSQL_ROW mq_row2;
	string sql3 = "select name from online where fd = ";
	sql3.append(itos(fd));
	cout << "sql3:" << sql3 << endl;
	//执行查询语句
	if(mysql_real_query(_mqcon,sql3.c_str(),sql3.size()))
	{
		cerr << "query fail;errno:" << errno << endl;
		return;
	}
	//接收查询的返回值
	mq_res2 = mysql_store_result(_mqcon);
	if(mq_res2 == NULL)
	{
		cerr << "no resultset;errno:" << errno << endl;
		return;
	}
	//接收返回值的一行，存放自己的名字
	mq_row2 = mysql_fetch_row(mq_res2);
	val["name"] = mq_row2[0];


	//得到查询的记录数
	num = mysql_num_rows(mq_res);
	if(num == 0)
	{
		//将消息插入到离线表中
		string sql4 = "insert into offline values('";
		sql4.append(name.c_str());
		sql4.append("','");
		sql4.append(msg.c_str());
		sql4.append("','");
		sql4.append(mq_row2[0]);
		sql4.append("')");
		cout << "sql4:" << sql4 << endl;
		//执行插入操作
		if(mysql_real_query(_mqcon,sql4.c_str(),sql4.size()))
		{
			cerr << "insert fail;errno:" << errno << endl;
			return;
		}
		_reply = "success";
		//响应给客户端
		response();
	}
	else
	{
		//存放在线的朋友查询结果
		mq_row = mysql_fetch_row(mq_res);
		//解析json，向好友发送数据
		printf("mq_row[0]:%d,msg:%s\n",atoi(mq_row[0]),msg.c_str());

		if(-1 == send(atoi(mq_row[0]),val.toStyledString().c_str(),strlen(val.toStyledString().c_str()),0))
		{
			cerr << "send fail;errno:" << errno << endl;
		}
	}
	//释放数据库连接
	mysql_close(_mqcon);
}

void view_talkone::response()
{
	Json::Value val;
	val["msg"] = _reply;
	if(-1 == send(_cli_fd,val.toStyledString().c_str(),strlen(val.toStyledString().c_str()),0))
	{
		cerr << "reply fail;errno:" << errno << endl;
	}
}
