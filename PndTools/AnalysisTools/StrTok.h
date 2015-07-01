// StrTok.h
//
//////////////////////////////////////////////////////////////////////
#ifndef STRTOK_H
#define STRTOK_H

#if _MSC_VER > 1000
//#pragma once
#endif // _MSC_VER > 1000

class CStrTok
{
public:
	CStrTok();
	virtual ~CStrTok();

	// Attributes
public:
	bool m_bDelimiterAsToken;
	bool m_bOneByOneDelimiter;
	bool m_bDelimitersInSequence;

	char* m_lpszNext;
	char m_chDelimiter;
	// Operations
public:
	char* GetFirst(char* lpsz, const char* lpcszDelimiters);
	char* GetNext(const char* lpcszDelimiters);
	void SetNext(const char* lpcszNext);
	bool IsEOB();
	void Break();
	void TrimLeft(char* &lpsz, const char* lpcszDelimiters = 0);
	void TrimRight(const char* lpcszDelimiters = 0);

	static bool IsDelimiter(char ch, const char* lpcszDelimiters);
};

#endif
