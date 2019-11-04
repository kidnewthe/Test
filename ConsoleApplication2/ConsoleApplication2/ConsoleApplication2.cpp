#include "pch.h"
#include "commonstruct.h"
#include <iostream>




void tprintf(const char* format) // 基础函数
{
	std::cout << format;
}

template<typename T, typename... Targs>
void tprintf(const char* format, T value, Targs... Fargs) // 递归变参函数
{
	for (; *format != '\0'; format++) {
		if (*format == '%') {
			std::cout << value;
			tprintf(format + 1, Fargs...); // 递归调用
			return;
		}
		std::cout << *format;
	}
}

template<typename... Targs>
void cachelog(const char* msg, Targs... args)
{
	char szbuff[LOG_BUFF_TEMP] = { 0 };
	StringParser log(szbuff, LOG_BUFF_TEMP, msg);
	log.Format(args...);
	szbuff[LOG_BUFF_TEMP - 1] = '\0';
	std::cout << szbuff <<std::endl;

}

template<typename... T>
void Test(int n,T... t)
{
	std::cout << "n=" << n << std::endl;
	return;

}




int main()
{
	tprintf("% world% %\n", "Hello", '!', 123);
	void* P;
	char p[6] = "abb";
	int tr = 45334;
	cachelog("test=%s,%d,%s",p,p, tr);
	return 0;
}