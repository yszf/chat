#pragma once
#include <stdio.h>
#include <cstring>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <json/json.h>
#include <pthread.h>
#include "public.h"
#include <iostream>

using namespace std;

void run(char* ip,unsigned short port);

void regist(int fd);

void login(int fd);

void myexit(int fd);

void talk_one(int fd);

void talk_group(int fd);

void get_list(int fd);
