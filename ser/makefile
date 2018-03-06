ser:main.o tcpserver.o mpthread.o\
		 control.o view_register.o view_login.o view_exit.o\
		 view_talkone.o view_talkgroup.o view_getlist.o
	g++ -o ser main.o tcpserver.o mpthread.o\
		control.o view_register.o view_login.o view_exit.o\
		view_talkone.o view_talkgroup.o view_getlist.o\
		-levent -lpthread -ljson -lmysqlclient -L/usr/lib/mysql\
		-I/usr/include/mysql -g
main.o:main.cpp
	g++ -c main.cpp -g
tcpserver.o:tcpserver.cpp
	g++ -c tcpserver.cpp -g
mpthread.o:mpthread.cpp
	g++ -c mpthread.cpp -g
control.o:control.cpp
	g++ -c control.cpp -g
view_register.o:view_register.cpp
	g++ -c view_register.cpp -g
view_login.o:view_login.cpp
	g++ -c view_login.cpp -g
view_exit.o:view_exit.cpp
	g++ -c view_exit.cpp -g
view_talkone.o:view_talkone.cpp
	g++ -c view_talkone.cpp -g
view_talkgroup.o:view_talkgroup.cpp
	g++ -c view_talkgroup.cpp -g
view_getlist.o:view_getlist.cpp
	g++ -c view_getlist.cpp -g
clean:
	rm *.o ser

