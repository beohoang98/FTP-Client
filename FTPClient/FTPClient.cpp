// FTPClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "FTPClient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
vector<string> arrCmd = { "open","ls","put","get","mput","mget","cd","lcd","delete","mdelete","mkdir","rmdir","pwd","passive","quit","exit"};

// The one and only application object

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule != NULL)
	{
		// initialize MFC and print and error on failure
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
		{
			// TODO: change error code to suit your needs
			_tprintf(_T("Fatal Error: MFC initialization failed\n"));
			nRetCode = 1;
		}
		else
		{
			if (AfxSocketInit() == FALSE){
				cout << "Khong the khoi tao Socket Libraray";
				return FALSE;
			}
			// TODO: code your application's behavior here.
			FTPClient client;
			client.getCmd();
			
		}
	}
	else
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: GetModuleHandle failed\n"));
		nRetCode = 1;
	}

	return nRetCode;
}

//--------------------
string FTPClient::standardizedCMD(string cmd)
{
	while (cmd[0] == ' ')
		cmd.erase(0, 1);
	while (cmd[cmd.size() - 1] == ' ')
		cmd.erase(cmd.size() - 1, 1);

	for (int i = 0; i < cmd.size(); i++)
	{
		if (isalpha(cmd[i]) && isupper(cmd[i]))
			cmd[i] = tolower(cmd[i]);
		if (i < cmd.size()- 1) {
			if (cmd[i] == ' '&& cmd[i + 1] == ' ')
			{
				cmd.erase(i, 1);
				i--;
			}
		}
	}
	return cmd;
}
int FTPClient::defineOrder(string order)
{
	for (int i = 0; i < arrCmd.size(); i++)
		if (arrCmd[i].compare(order) == 0)
			return i;
}
void FTPClient::getCmd()
{
	string cmd = "", order = "", clause = "";
LOOP:cout << "FTP >> ";
	getline(cin, cmd);
	cmd = this->standardizedCMD(cmd);
	order = cmd.substr(0, cmd.find_first_of(' '));

	switch (defineOrder(order))
	{
	case 0: {				//open 127.0.0.1
		clause = cmd.substr(cmd.find_first_of(' ') + 1, cmd.size() - cmd.find_first_of(' ') - 1);
		if (clause == "localhost")
			clause = "127.0.0.1";
		this->hostIP = clause;
		this->connect();
	}break;
	case 1: this->cmd_ls();	break;
	case 2: this->cmd_put(); break;
	case 3: this->cmd_get(); break;
	case 4: this->cmd_mput(); break;
	case 5: this->cmd_mget(); break;
	case 6: this->cmd_cd(); break;
	case 7: this->cmd_lcd(); break;
	case 8: this->cmd_del(); break;
	case 9: this->cmd_mdel(); break;
	case 10: this->cmd_mkdir(); break;
	case 11: this->cmd_rmdir(); break;
	case 12: this->cmd_pwd(); break;
	case 13: this->cmd_pasv(); break;
	case 14: 
	case 15: {this->cmd_quit(); return;  break; }
	default: {cout << "error syntax\n"; goto LOOP; }
			 break;
	}
	goto LOOP;
}
//{ 0."open",1."ls",2."put",3."get",4."mput",5."mget",6."cd",7."lcd",8."delete",9."mdelete",10."mkdir",11."rmdir",12."pwd",13."passive",14."quit",15."exit"};
//-----------------
FTPClient::FTPClient(){

	//this->dataPort = 0;
	this->cmdClient.Create();//auto choose client port, TCP type,  CAsyncSocket instance should listen for client activity on all network interfaces.
	//this->dataClient.Create();
	//hostIP = "127.0.0.1";
}

FTPClient::FTPClient(string mHostIP, int dataPort){

	this->hostIP = mHostIP;
	this->cmdClient.Create();//auto choose client port, TCP type,  CAsyncSocket instance should listen for client activity on all network interfaces.
	wstring wstrHost;
	wstrHost.assign(hostIP.begin(), hostIP.end());
	if (cmdClient.Connect(wstrHost.c_str(), dataPort) == 0)
		cout << "-Fatal error!!";
}
FTPClient::~FTPClient()
{
	this->cmdClient.Close();
}
bool FTPClient::login() {
	cout << endl;
	cout << "USERNAME: ";	getline(cin, this->user);
	this->cmd_user();
	cout << "PASSWORD: ";	getline(cin, this->password);
	this->cmd_pass();
	this->getCmd();
	return true;
}
bool FTPClient::connect()
{
	wstring wstrHost;
	wstrHost.assign(hostIP.begin(), hostIP.end());
	if (cmdClient.Connect(wstrHost.c_str(), 21) != 0)
	{
		this->receive();
		this->displayMessage();
		this->login();
		return true;
	}
	return false;
}


void FTPClient::send()
{
	this->cmdClient.Send(request.c_str(), request.length()+1, 0);
}
void FTPClient::receive()
{
	respone.clear();
	char *temp = new char[MAX_LENGTH];
	int len = this->cmdClient.Receive(temp, MAX_LENGTH, 0);
	respone = string(temp).substr(0, len);

}
void FTPClient::displayMessage()
{
	cout << this->respone << endl;
}

void FTPClient::cmd_user()
{
	request = "USER " + user;
	this->action();
}
void FTPClient::cmd_pass()
{
	request = "PASS " + password;
	this->action();
	if (this->getServerCode() == 530)
	{
		cout << "- Moi dang nhap lai!!" << endl;
		this->login();
	}
}
void FTPClient::cmd_ls()
{
	this->cmd_pasv();
	FTPClient dataClient(this->hostIP, this->getDataPort());
	dataClient.request = "LIST\r\n";
	dataClient.action();
}
void FTPClient::cmd_pwd()
{
	request = "PWD";
	this->action();
}
void FTPClient::cmd_mkdir()
{
	request = "MKD " + argument.at(0);
	this->action();
}
void FTPClient::cmd_rmdir()
{
	request = "RMD " + argument.at(0);
	this->action();
}

void FTPClient::cmd_pasv()
{
	request = "pasv";
	this->action();
}
void FTPClient::cmd_cd()
{
}
void FTPClient::cmd_del()
{
}
void FTPClient::cmd_get()
{
}
void FTPClient::cmd_mget()
{
}

void FTPClient::cmd_lcd()
{
}
void FTPClient::cmd_mdel()
{
}
void FTPClient::cmd_mput()
{
}
void FTPClient::cmd_put()
{
}
void FTPClient::cmd_quit()
{
	request = "QUIT";
	this->action();
}


int FTPClient::getDataPort()
{
	string str = this->respone;
	int pos1 = 0; int count = 0;
	for (; pos1 < str.length() && count < 4; pos1++)
	{
		if (str.at(pos1) == ',')
			count++;
	}

	int pos2 = str.find_last_of(',');
	string a_str = str.substr(pos1, pos2 - pos1);
	pos1 = str.find_first_of(')');
	string b_str = str.substr(pos2 + 1, pos1 - pos2);
	int a = atoi(a_str.c_str());
	int b = atoi(b_str.c_str());
	return a * 256 + b;
}

int FTPClient::getServerCode()
{
	string str = this->respone.substr(0, this->respone.find_first_of(' '));
	return atoi(str.c_str());
}
