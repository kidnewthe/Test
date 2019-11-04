#pragma once
#include<string.h>
#include<cstdio>
#define STRING_PARSER_FMT_TEMP 4096
#define STRING_PARSER_BUFF_TEMP 4096
#define LOG_BUFF_TEMP 4096



namespace StringFormatUtility
{
	template<typename T>
	bool BasicValue2String(char* dest, int&pos, int size, const char* fmt, T t)
	{
		if (pos >= size - 1)
			return false;
		int r = std::snprintf(dest+pos,size-pos,fmt,t);
		if (r >= 0 && pos + r < size)
			pos += r;
		else
			pos = size - 1;
		return (r >= 0 ? true : false);
	}

	template<typename T>
	struct _impl_for_format_check
	{
		enum{ value = 0 };
		static inline bool format(char* dest, int&pos, int size, const char* fmt, T t, int argIndex)
		{
			int len = strlen(fmt);
			if (len <= 0)
			{
				bool bRet = BasicValue2String(dest, pos, size, "@format:%d,unexpected_value ", argIndex);
				return bRet;
			}
			else if (fmt[len - 1] == 's' || fmt[len - 1] == 'S' || fmt[len - 1] == 'n' || fmt[len - 1] == 'p')
			{
				bool bRet = BasicValue2String(dest, pos, size, "@format:%d,value_as_pointer ", argIndex);
				return bRet;
			}
			else
			{
				return BasicValue2String(dest, pos, size, fmt, t);
			}
		}
	};


	template<typename T>
	struct _impl_for_format_check<T*>
	{
		enum{ value = 1};
		static inline bool format(char* dest, int&pos, int size, const char* fmt, T* t, int argIndex)
		{
			int len = strlen(fmt);
			if (len <= 0)
			{
				bool bRet = BasicValue2String(dest, pos, size, "@format:%d,unexpected_pointer ", argIndex);
				return bRet;
			}
			else if (fmt[len - 1] != 's' && fmt[len - 1] != 'S' && fmt[len - 1] != 'n' && fmt[len - 1] != 'p')
			{
				bool bRet = BasicValue2String(dest, pos, size, "@format:%d,pointer_as_value ", argIndex);
				return bRet;
			}
			else
			{
				return BasicValue2String(dest, pos, size, fmt, t);
			}
		}
	};

	template<typename T>
	bool GeneralValue2String(char* dest, int& pos, int size, const char* fmt, T t, int argIndex)
	{
		return _impl_for_format_check<T>::format(dest, pos, size, fmt, t, argIndex);
	}

	template<typename T,int k>
	struct _impl_for_64
	{
		static inline bool format(char* dest, int& pos, int size, const char* fmt, T t,int argindex)
		{
			return GeneralValue2String(dest, pos, size, fmt, t, argindex);
		}
	};

	template<typename T>
	struct _impl_for_64<T*,8>
	{
		static inline bool format(char* dest, int& pos, int size, const char* fmt, T* t, int argindex)
		{
			return GeneralValue2String(dest, pos, size, fmt, t, argindex);
		}
	};
	template<>
	struct _impl_for_64<double, 8>
	{
		static inline bool format(char* dest, int& pos, int size, const char* fmt, double t, int argindex)
		{
			return GeneralValue2String(dest, pos, size, fmt, t, argindex);
		}
	};

	template<typename T>
	bool Value2String(char* dest, int&pos, int size, const char* fmt,T t, int argindex)
	{
		return _impl_for_64<T, sizeof(T)>::format(dest, pos, size, fmt, t, argindex);
	}
}
class StringParser
{
public:
	void Format()
	{
		ParseToEnd();
	}

	template<typename... Tags>
	void Format(Tags... args)
	{
		_Format(args...);
		ParseToEnd();
	}


	template<typename T,typename... Targs>
	void _Format(T t1,Targs... args)
	{
		const char* fmt = ParseToNextFormat();
		if (NULL != fmt)
			StringFormatUtility::Value2String(m_pBuffer, m_BufPos, m_BufLen, fmt, t1, m_FmtIndex);
		else
			StringFormatUtility::Value2String(m_pBuffer, m_BufPos, m_BufLen, "", t1, m_FmtIndex);

		_Format(args...);
	}

	StringParser(char* buf, int size, const char* fmt); 


private:
	const char* ParseToNextFormat(void);
	void ParseToEnd(void);

	template<typename T>
	inline void _Format(T t)
	{
		const char* fmt = ParseToNextFormat();
		if (NULL != fmt)
			StringFormatUtility::Value2String(m_pBuffer, m_BufPos, m_BufLen, fmt, t, m_FmtIndex);
		else
			StringFormatUtility::Value2String(m_pBuffer, m_BufPos, m_BufLen, "", t, m_FmtIndex);
	}
private:
	char* m_pBuffer;
	const char* m_pFmt;
	char  m_TempFmt[STRING_PARSER_FMT_TEMP];
	int		m_BufPos;
	int		m_FmtPos;
	int		m_FmtIndex;
	int		m_BufLen;
	int		m_FmtLen;
};