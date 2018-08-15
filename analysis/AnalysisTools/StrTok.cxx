// StrTok.cpp: implementation of the CStrTok class.
//
//////////////////////////////////////////////////////////////////////

#include "StrTok.h"
#include "string.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStrTok::CStrTok()
{
	m_bDelimiterAsToken = m_bOneByOneDelimiter = m_bDelimitersInSequence = false;
	m_chDelimiter = 0;
	m_lpszNext = NULL;
}

CStrTok::~CStrTok()
{

}

char* CStrTok::GetFirst(char* lpsz, const char* lpcszDelimiters)
{
	Break();

	m_lpszNext = lpsz;
	return GetNext(lpcszDelimiters); 
}

char* CStrTok::GetNext(const char* lpcszDelimiters)
{
	if(m_lpszNext == NULL)
		return NULL;

	// return the saved delimiter
	if(m_chDelimiter != 0)
		*m_lpszNext = m_chDelimiter;

	char* lpsz = m_lpszNext;
	if(m_bDelimiterAsToken)
		// increment the next pointer to the next delimiter or token
		if(m_bDelimitersInSequence)
		{
			int nLength = strlen(lpcszDelimiters);
			if(memcmp(lpcszDelimiters, m_lpszNext, nLength) == 0)
				m_lpszNext += nLength;
			else
				m_lpszNext = strstr(m_lpszNext, lpcszDelimiters); // may be NULL
		}
		else
		{
			if(strchr(lpcszDelimiters, *m_lpszNext))
				m_lpszNext++;
			else
				m_lpszNext += strcspn(m_lpszNext, lpcszDelimiters);
		}
	else
		if(m_bDelimitersInSequence)
		{
			int nLength = strlen(lpcszDelimiters);
			// increment the token pointer
			while(memcmp(lpcszDelimiters, lpsz, nLength) == 0)
			{
				lpsz += nLength;
				if(m_bOneByOneDelimiter)
					break;
			}
			// increment the next pointer after the end of the token
			m_lpszNext = strstr(lpsz, lpcszDelimiters); // may be NULL
		}
		else
		{
			// increment the token pointer to the start of the token
			if(!m_bOneByOneDelimiter)
				lpsz += strspn(lpsz, lpcszDelimiters);
			else
				if(strchr(lpcszDelimiters, *lpsz) && m_chDelimiter)
					lpsz++;
			// increment the next pointer after the end of the token
			m_lpszNext = lpsz + strcspn(lpsz, lpcszDelimiters);
		}
	
	if(m_lpszNext == NULL || *m_lpszNext == 0)
	{	// reach the end of the buffer
		m_chDelimiter = 0;
		m_lpszNext = NULL;
	}
	else
	{	// save the delimiter and terminate the token by null
		m_chDelimiter = *m_lpszNext;
		*m_lpszNext = 0;
	}
	if(*lpsz == 0 && !m_bOneByOneDelimiter)
		return NULL;
	return lpsz;
}

void CStrTok::SetNext(const char* lpcszNext)
{
	if(m_chDelimiter != 0)
		*m_lpszNext = m_chDelimiter;
	m_chDelimiter = 0;
	m_lpszNext = (char*)lpcszNext;
}

bool CStrTok::IsEOB()
{
	return m_lpszNext == NULL;
}

bool CStrTok::IsDelimiter(char ch, const char* lpcszDelimiters)
{
	return strchr(lpcszDelimiters, ch) != NULL;
}

void CStrTok::Break()
{
	SetNext(NULL);
}

void CStrTok::TrimLeft(char* &lpsz, const char* lpcszDelimiters /*= NULL*/)
{
	if(m_lpszNext == NULL)
		return;
	if(lpcszDelimiters == NULL)
		lpcszDelimiters = " \t\r\n";
	while(strchr(lpcszDelimiters, *lpsz))
		lpsz++;
}

void CStrTok::TrimRight(const char* lpcszDelimiters /*= NULL*/)
{
	if(m_lpszNext == NULL)
		return;
	if(lpcszDelimiters == NULL)
		lpcszDelimiters = " \t\r\n";
	char* pNext = m_lpszNext-1;
	while(strchr(lpcszDelimiters, *pNext))
		*pNext = 0, pNext--;
}

