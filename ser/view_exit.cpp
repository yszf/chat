#include "view_exit.h"


view_exit::view_exit()
{}

void view_exit::process(Json::Value val,int fd)
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

	MYSQL_RES *mq_res;
	MYSQL_ROW mq_row;
	//查询当前用户是否在线
	string sql1 = "select * from online where fd = ";
	sql1.append(itos(fd).c_str());
	cout << "sql1:" << sql1 << endl;
	//执行查询语句
	if(mysql_real_query(_mqcon,sql1.c_str(),sql1.size()))
	{
		cerr << "select fail;errno:" << errno << endl;
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

	if(num != 0)
	{
		//从在线表中删除当前登录的用户
		string sql2 = "delete from online where fd=";
		sql2.append(itos(fd).c_str());
		cout << "sql2:" << sql2 << endl;
		//执行删除语句
		if(mysql_real_query(_mqcon,sql2.c_str(),sql2.size()))
		{
			cerr << "delete fail;errno:" << errno << endl;
			return;
		}
	}
	_reply = "success";
	//释放数据库连接
	mysql_close(_mqcon);
	//响应客户端
	response();
	//关闭客户端
	close(fd);
}

void view_exit::response()
{
	Json::Value val;
	val["reply"] = _reply;
	if(-1 == send(_cli_fd,val.toStyledString().c_str(),strlen(val.toStyledString().c_str()),0))
	{
		cerr << "reply fail;errno:" << errno << endl;
	}
}
