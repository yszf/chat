#pragma once
#include "view.h"

class view_talkone:public view
{
	public:
		view_talkone();
		void process(Json::Value val,int cli_fd);
		void response();
	private:
		int _cli_fd;
		string _reply;
};
