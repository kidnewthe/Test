#include "pch.h"
#include "commonstruct.h"
#include <string.h>
namespace StringFormatUtility
{
	const char* format_string = "cCdiouxXeEfgGaAnpsS";
}

StringParser::StringParser(char* buf, int size, const char* fmt) :m_pBuffer(buf), m_BufLen(size), m_pFmt(fmt)
{
	m_FmtLen = strlen(fmt);
	m_BufPos = 0;
	m_FmtPos = 0;
	m_FmtIndex = 0;
}
const char* StringParser::ParseToNextFormat(void)
{
	int len = m_FmtLen;
	for (int i = m_FmtPos; i < len; i++)
	{
		if (m_pFmt[i] == '%')
		{
			if (i + 1 < len && m_pFmt[i + 1] == '%')	//"%%"中的情况
			{
				++i;
				if (m_BufPos < m_BufLen - 1)
				{
					m_pBuffer[m_BufPos] = m_pFmt[i];
					++m_BufPos;
				}
				else
				{
					break;
				}
			}
			else
			{
				++m_FmtIndex;
				for (int j = i; j < len; j++)
				{
					m_TempFmt[j - i] = m_pFmt[j];
					const char* p= strchr(StringFormatUtility::format_string, m_pFmt[j]);
					if (NULL != p)
					{
						m_FmtPos = j + 1;
						m_TempFmt[j - i + 1] = 0;
						return m_TempFmt;
					}
				}
				break;
			}
		}
		else
		{
			if (m_BufPos < m_BufLen - 1)
			{
				m_pBuffer[m_BufPos] = m_pFmt[i];
				++m_BufPos;
			}
			else
			{
				break;
			}
		}
	}
	m_FmtPos = len;
	return NULL;
}
void StringParser::ParseToEnd(void)
{
	int len = m_FmtLen;
	for (int i = m_FmtPos; i < len; ++i)
	{
		if (m_pFmt[i] == '%')	//格式化字符
		{
			if (i + 1 < len && m_pFmt[i + 1] == '%')	//"%%"中的情况
			{
				++i;
				if (m_BufPos < m_BufLen - 1)
				{
					m_pBuffer[m_BufPos] = m_pFmt[i];
					++m_BufPos;
				}
				else
				{
					break;
				}
			}
			else
			{
				++m_FmtIndex;
				for (int j = i; j < len; j++)
				{
					const char* p = strchr(StringFormatUtility::format_string, m_pFmt[j]);
					if (NULL != p)
					{
						if (false == StringFormatUtility::BasicValue2String(m_pBuffer, m_BufPos, m_BufLen, "@format:%d,unexpected_format", m_FmtIndex - 1))
						{
							m_FmtPos = len;
							return;
						}

						i = j;
						break;
					}
				}
			}
		}
		else
		{
			if (m_BufPos < m_BufLen - 1)
			{
				m_pBuffer[m_BufPos] = m_pFmt[i];
				++m_BufPos;
			}
			else
			{
				break;
			}
		}
	}

	m_FmtPos = len;
	if (m_BufPos < m_BufLen - 1)
		m_pBuffer[m_BufPos] = '\0';
	else
		m_pBuffer[m_BufLen - 1] = '\0';
}