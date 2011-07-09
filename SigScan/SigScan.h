#ifndef __SIGSCAN_H__
#define __SIGSCAN_H__

namespace SigScan
{
	DWORD_PTR Scan(const char* szPattern, int offset = 0);
	void InitializeSigScan(DWORD ProcessID, const TCHAR* Module);
	void TerminateSigScan();
}

#endif//__SIGSCANDLL_H__