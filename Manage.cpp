#include "pch.h"
#include "Manage.h"
#include "ServerChatDlg.h"
Manage::Manage(CServerChatDlg* p)
{
	dlg = p;
	//vs.resize(1);
}

Manage::~Manage()
{
	closesocket(s);
	WSACleanup();
}

UINT Manage::StartListen(CString s_ip, CString s_port)
{
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		error(L"WSAStartup");
		return -1;
	}
	dlg->ShowMessage(L"WSAStarup completed.");
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		error(L"socket");
		WSACleanup();
		return -1;
	}
	dlg->ShowMessage(L"socket completed.");
	int port = _ttoi(s_port);
	CT2A a(s_ip);
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;//inet_addr(a);
	server.sin_port = htons(port);
	if (bind(s, (struct sockaddr*) & server, sizeof(server)) == SOCKET_ERROR)
	{
		error(L"bind");
		closesocket(s);
		WSACleanup();
		return -1;
	}
	dlg->ShowMessage(L"bind completed.");
	listen(s, 100);
	dlg->ShowMessage(L"Listening...");
	int c = sizeof(struct sockaddr_in);
	while ((news = accept(s, (struct sockaddr*) & client, &c)) != INVALID_SOCKET)
	{
		char* client_ip = inet_ntoa(client.sin_addr);
		cc_ip =(CString)client_ip;
		int client_port = ntohs(client.sin_port);
		cc_port.Format(_T("%d"), client_port);
		vs.push_back(psa(news, cc_ip, cc_port));
		dlg->showListClientMsg(cc_ip, cc_port);
		dlg->ShowMessage(L"Accept connection from " + cc_ip + L":"+cc_port);
		CWinThread* cTh = AfxBeginThread(Manage::StaticDataFunc, this, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
	}
	return 0;
}

void Manage::error(const WCHAR* x)
{
	CString temp;
	temp.Format(_T(" %d"), WSAGetLastError());
	AfxMessageBox(x + temp);
}

UINT __cdecl Manage::StaticDataFunc(LPVOID pParam)
{
	Manage* ss = reinterpret_cast <Manage*> (pParam);
	UINT retcode = ss->DataFunction();
	return retcode;
}

UINT Manage::DataFunction()
{
	psa ss = vs.back();
	WCHAR temp_msg[] = L"Welcome to the chatroom...";
	char* p = reinterpret_cast<char*>(temp_msg);
	send(ss.soc, p, sizeof(temp_msg), 0);
	int recv_size;
	char client_rep[1000] = { 0 };
	memset(client_rep, 0, sizeof(client_rep));
	while ((recv_size = recv(ss.soc, client_rep, 998, 0)) != SOCKET_ERROR)
	{
		for (auto i : vs)
		{
			//more here
			if (send(i.soc, client_rep, recv_size, 0) < 0)
			{
				return -1;
			}
		}
	}
	if (WSAGetLastError() == 10054)
	{
		dlg->ShowMessage(L"Client " + ss.ip + L":" + ss.port + L" disconnected");
		for (std::vector<psa>::iterator it = vs.begin(); it!=vs.end();it++)
			if (*it == ss)
			{
				it = vs.erase(it);
				dlg->updateList(vs);
				break;
			}
		return 0;
	}
	dlg->ShowMessage(L"Fail to recv from client " + ss.ip + L":" + ss.port);
	for (std::vector<psa>::iterator it = vs.begin(); it != vs.end(); it++)
		if (*it == ss)
		{
			it = vs.erase(it);
			dlg->updateList(vs);
			break;
		}
	return -1;
}


