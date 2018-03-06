#include "control.h"
#include "public.h"
#include "view_register.h"
#include "view_login.h"
#include "view_exit.h"
#include "view_talkone.h"
#include "view_talkgroup.h"
#include "view_getlist.h"

control::control()
{
	_model_map.insert(make_pair(MSG_TYPE_REGISTER,new view_register()));
	_model_map.insert(make_pair(MSG_TYPE_LOGIN,new view_login()));
	_model_map.insert(make_pair(MSG_TYPE_EXIT,new view_exit()));
	_model_map.insert(make_pair(MSG_TYPE_TALKONE,new view_talkone()));
	_model_map.insert(make_pair(MSG_TYPE_TALKGROUP,new view_talkgroup()));
	_model_map.insert(make_pair(MSG_TYPE_GETLIST,new view_getlist()));
}

void control::process(int fd,char *buff)
{
	//解析数据
	Json::Value val;
	Json::Reader read;
	if(read.parse(buff,val)==-1)
	{
		cerr << "read fail;errno:" << errno << endl;
		return;
	}
	_model_map[val["type"].asInt()]->process(val,fd);
}
