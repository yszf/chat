#pragma once
#include "view.h"

class view_talkgroup:public view
{
	public:
		view_talkgroup();
		void process(Json::Value val,int cli_fd);
		void response();
	private:
		int _cli_fd;
		string _reply;
};
