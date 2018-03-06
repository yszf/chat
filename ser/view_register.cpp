#include "view_register.h"

view_register::view_register(){}

void view_register::process(Json::Value val,int fd)
{	
	MYSQL *_mqcon = mysql_init((MYSQL *)0);
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
		return;
	}

	_cli_fd = fd;
	string name = val["name"].asString();
	MYSQL_RES *mq_res;
	//查看数据库中是否已存在该用户名
	string sql1 = "select * from usr where name='";
	sql1.append(name);
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
	}

	//得到查询的记录数
	int num = mysql_num_rows(mq_res);
	if(num != 0)
	{
		_reply ="fail";
	}
	else
	{
		string pw = val["pw"].asString();
		//插入新用户
		string sql2 = "insert into usr values('";
		sql2.append(name);
		sql2.append("','");
		sql2.append(pw);
		sql2.append("')");
		cout << "sql2:" << sql2 << endl;
		//执行插入语句
		if(mysql_real_query(_mqcon,sql2.c_str(),sql2.size()))
		{
			cerr << "insert fail;errno:" << errno << endl;
			return;
		}
		_reply = "success";
	}
	//释放连接
	mysql_close(_mqcon);
	//响应客户端
	response();
}

void view_register::response()
{
	Json::Value val;
	val["reply"] = _reply;
	if(-1 == send(_cli_fd,val.toStyledString().c_str(),strlen(val.toStyledString().c_str()),0))
	{
		cerr << "reply fail;errno:" << errno << endl;
	}
}
