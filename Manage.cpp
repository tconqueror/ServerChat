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
	for (auto i : vs)
	{
		closesocket(i.soc);
	}
	for (auto i : vt)
	{
		TerminateThread(i->m_hThread, 0);
	}
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
	server.sin_addr.s_addr = inet_addr(a);
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
		vt.push_back(cTh);
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
	char temp_rep[2000];
	CString temp = L"["+ss.ip + L":" + ss.port + L"] ";
	const WCHAR* r_client = (const WCHAR*)temp;
	memcpy_s(temp_rep, 2000, r_client, 2000);
	while ((recv_size = recv(ss.soc, client_rep, 998, 0)) != SOCKET_ERROR)
	{
		memcpy_s(temp_rep + wcslen(temp) * 2, 2000 - wcslen(temp) * 2, client_rep, 998);
		for (auto i : vs)
		{
			//more here
			if (send(i.soc, temp_rep, recv_size + wcslen(temp) * 2, 0) < 0)
			{
				dlg->ShowMessage(L"Msg to " + ss.ip + L":" + ss.port + L" failed");
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


