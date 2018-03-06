#include "cli_fun.h"

int main(int argc,char *argv[])
{
	if(argc < 3)
	{
		cerr << "arg errno:" << errno << endl;
		return -1;
	}

	char *ip = argv[1];
	unsigned int port = atoi(argv[2]);
	run(ip,port);

	return 0;
}
