#include "view_getlist.h"

using namespace std;

view_getlist::view_getlist(){}

void view_getlist::process(Json::Value val,int fd)
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

	string sql1 = "select * from online where fd != ";
	sql1.append(itos(fd));
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

	char buff[128] = {0};
	//存放查询结果
	while(mq_row = mysql_fetch_row(mq_res))
	{
		strcat(buff,mq_row[1]);
		strcat(buff,"|");
	}	
	Json::Value res;
	res["reply"] = "success";
	res["name"] = buff;
	_reply = res.toStyledString();
	//释放数据库连接
	mysql_close(_mqcon);
	//响应给客户端
	response();
}

void view_getlist::response()
{
	Json::Value val;
	Json::Reader read;

	if(read.parse(_reply.c_str(),val)==-1)
	{
		cerr << "read fail;errno:" << errno << endl;
	}
	//解析json，向客户端发送数据
	if(-1 == send(_cli_fd,val.toStyledString().c_str(),strlen(val.toStyledString().c_str()),0))
	{
		cerr << "reply fail;errno:" << errno << endl;
	}
}
