#pragma once
#include "view.h"

class view_getlist:public view
{
	public:
		view_getlist();
		void process(Json::Value val,int cli_fd);
		void response();
	private:
		int _cli_fd;
		string _reply;
};
