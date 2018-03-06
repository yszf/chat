#pragma once
#include "view.h"

class view_exit:public view
{
	public:
		view_exit();
		void process(Json::Value val,int fd);
		void response();
	private:
		int _cli_fd;
		string _reply;
};
