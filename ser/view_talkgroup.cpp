#include "view_talkgroup.h"

using namespace std;

view_talkgroup::view_talkgroup(){}

void view_talkgroup::process(Json::Value val,int fd)
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

	//接收发送的消息
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

	//查看群里是否有好友
	string sql1 = "select * from usr";
	cout << "sql1:" << sql1 << endl;
	//执行查询语句
	if(mysql_real_query(_mqcon,sql1.c_str(),sql1.size()))
	{
		cerr << "query error;errno:" << errno << endl;
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
	
	//查询不在线的好友
	string sql2 = "select name from usr where name not in (select name from online)";
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

	//查询发送人的name
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
	//接收返回值的一行
	mq_row2 = mysql_fetch_row(mq_res2);

	//查询结果
	while(mq_row = mysql_fetch_row(mq_res))
	{
		//将消息插入到离线表中
		string sql4 = "insert into offline values('";
		sql4.append(mq_row[0]);
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
	}

	//查询在线的好友
	MYSQL_RES *mq_res3;
	MYSQL_ROW mq_row3;
	string sql5 = "select usr.name,online.fd from online, usr where usr.name = online.name and online.fd != ";
	sql5.append(itos(fd));
	cout << "sql5:" << sql5 << endl;
	//执行查询语句
	if(mysql_real_query(_mqcon,sql5.c_str(),sql5.size()))
	{
		cerr << "query fail;errno:" << errno << endl;
		return;
	}
	//接收查询的返回值
	mq_res3 = mysql_store_result(_mqcon);
	if(mq_res3 == NULL)
	{
		cerr << "no resultset;errno:" << errno << endl;
		return;
	}
	
	//存放查询结果
	while(mq_row3 = mysql_fetch_row(mq_res3))
	{
		val["name"] = mq_row[0];
		
		cout << "name:" << val["name"].asString() << ",msg:" << val["msg"].asString() << endl;
		//解析json，向客户端发送数据
		if(-1 == send(atoi(mq_row3[1]),val.toStyledString().c_str(),strlen(val.toStyledString().c_str()),0))
		{
			cerr << "reply fail;errno:" << errno << endl;
		}
	}
	//释放数据库连接
	mysql_close(_mqcon);
}

void view_talkgroup::response()
{
	Json::Value val;
	val["reply"] = _reply;
	//解析json，向客户端发送数据
	if(-1 == send(_cli_fd,val.toStyledString().c_str(),strlen(val.toStyledString().c_str()),0))
	{
		cerr << "reply fail;errno:" << errno << endl;
	}
}
