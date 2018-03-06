#pragma once
#include <string>
#include <mysql/mysql.h>
#include <errno.h>
#include <json/json.h>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>

using namespace std;

string itos(int i);

class view
{
	public:
		virtual void process(Json::Value val,int cli_fd)=0;
		virtual void response() = 0;
};

