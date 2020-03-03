// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here
#include "framework.h"
#include <WinSock2.h>
#include <vector>
#endif //PCH_H
typedef struct psa
{
	SOCKET soc;
	CString ip;
	CString port;
	psa(SOCKET a, CString b, CString c)
	{
		soc = a;
		ip = b;
		port = c;
	}
	bool operator ==(psa b)
	{
		if (b.soc != soc)
			return false;
		if (b.ip != ip)
			return false;
		if (b.port != port)
			return false;
		return true;
	}
};