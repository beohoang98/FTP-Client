#pragma once
#define MAX_LENGTH 1000
#include "resource.h"
#include <string>

#include <afxsock.h>
#include <string.h>
#include <vector>
#include<Windows.h>
#include <iomanip> 
using namespace std;

class FTPClient
{
private:
	CSocket cmdClient;
	//CSocket dataClient;
	//int		dataPort;
	string user;
	string password;
	string hostIP;
	string request;
	string respone;
	vector<string> argument;

	bool isLogged;
	bool isConnected;
public:
	bool login();
	bool connect();
	//
	void send();
	void receive();
	void displayMessage();
	void action() { this->send(); this->receive(); this->displayMessage(); }
	//cmd
	void cmd_ls();
	void cmd_pwd();
	void cmd_cd();
	void cmd_lcd();
	void cmd_get();
	void cmd_put();
	void cmd_mget();
	void cmd_mput();
	void cmd_del();
	void cmd_mdel();
	void cmd_mkdir();
	void cmd_rmdir();
	void cmd_pasv();
	void cmd_quit();
	void cmd_user();
	void cmd_pass();
	void cmd_clear() { system("cls"); getCmd(); }
	void cmd_help();
	//support function
	string standardizedCMD(string);
	int defineOrder(string);
	void getClauses(string cmd);

	FTPClient();
	FTPClient(string mHostIP, int dataPort);
	~FTPClient();
	void getCmd();
	int getDataPort();
};