#include "view_login.h"
#include <sstream>

using namespace std;

string itos(int i)
{
	stringstream s;
	s << i;
	return s.str();
}

view_login::view_login(){}

void view_login::process(Json::Value val,int fd)
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
	string pw = val["pw"].asString();

	MYSQL_RES *mq_res;
	MYSQL_ROW mq_row;

	string sql1 = "select * from usr where name='";
	sql1.append(name.c_str());
	sql1.append("' and pw='");
	sql1.append(pw.c_str());
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
	//存放查询结果
	mq_row = mysql_fetch_row(mq_res);
	
	//登录成功，则在在线表中插入一条数据
	string sql2 = "insert into online values(";
	sql2.append(itos(fd).c_str());
	sql2.append(",'");
	sql2.append(mq_row[0]);
	sql2.append("')");
	cout << "sql2:" << sql2 << endl;
	//执行插入操作
	if(mysql_real_query(_mqcon,sql2.c_str(),sql2.size()))
	{
		cerr << "insert fail;errno:" << errno << endl;
		return;
	}

	MYSQL_RES *mq_res2;
	MYSQL_ROW mq_row2;

	//查询是否有离线消息
	string sql3 = "select msg,sname from offline where rname = '";
	sql3.append(mq_row[0]);
	sql3.append("'");
	cout << "sql3:" << sql3 << endl;
	
	//执行查询语句
	if(mysql_real_query(_mqcon,sql3.c_str(),sql3.size()))
	{
		cerr << "query fail;errno:" << errno << endl;
		return;
	}

	//接收查询的返回值
	mq_res2 = mysql_store_result(_mqcon);
	if(mq_res == NULL)
	{
		cerr << "no resultset;errno:" << errno << endl;
		return;
	}

	//得到查询的记录数
	num = mysql_num_rows(mq_res2);
	if(num == 0)
	{
		_reply = "success";
		//释放数据库连接
		mysql_close(_mqcon);
		//响应给客户端
		response();	
		return;
	}
	
	//获取结果
	char buff[256] = {0};
	while(mq_row2 = mysql_fetch_row(mq_res2))
	{
		Json::Value val;
		val["msg"] = mq_row2[0];
		val["sname"] = mq_row2[1];
		strcat(buff,val.toStyledString().c_str());
		strcat(buff,"|");
	}

	Json::Value res;
	res["reply"] = "success";
	res["info"] = buff;
	if(-1 == send(_cli_fd,res.toStyledString().c_str(),strlen(res.toStyledString().c_str()),0))
	{
		cerr << "reply fail;errno:" << errno << endl;
	}

	//删除离线消息
	string sql4 = "delete from offline where rname = '";
	sql4.append(mq_row[0]);
	sql4.append("'");
	cout << "sql4:" << sql4 << endl;
	
	//执行删除语句
	if(mysql_real_query(_mqcon,sql4.c_str(),sql4.size()))
	{
		cerr << "query fail;errno:" << errno << endl;
		return;
	}

	//释放数据库连接
	mysql_close(_mqcon);
}

void view_login:: response()
{
	Json::Value val;
	val["reply"] = _reply;	
	if(-1 == send(_cli_fd,val.toStyledString().c_str(),strlen(val.toStyledString().c_str()),0))
	{
		cerr << "reply fail;errno:" << errno << endl;
	}
}
