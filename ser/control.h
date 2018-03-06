#pragma once
#include <iostream>
#include "view.h"
#include <event.h>
#include <map>

using namespace std;

typedef class control
{
	public:
		control();
		void process(int fd,char *buff);
	private:
		map<int,view*> _model_map;//model
}Control,*pControl;
