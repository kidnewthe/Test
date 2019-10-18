// isbaseofTest.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>

typedef char(&yes)[1];
typedef char(&no)[2];

template <typename B, typename D>
struct Host
{
	operator B*() const;
	operator D*();
};

template <typename B, typename D>
struct is_base_of
{
	template <typename T>
	static yes check(D*, T);
	static no check(B*, int);

	static const bool value = sizeof(check(Host<B, D>(), int())) == sizeof(yes);
};
//13.3.3.1.2
//2 Implicit conversion sequences are concerned only with the type, 
//cv-qualiﬁcation, and value category of the argument and how these are converted to match 
//the corresponding properties of the parameter. Other properties,suchasthelifetime,storageclass,
//alignment,accessibilityoftheargument,whethertheargument is a bit-ﬁeld, and whether a function is deleted (8.4.3),
//are ignored. So, although an implicit conversion sequence can be deﬁned for a given argument-parameter pair, 
//the conversion from the argument to the parameter might still be ill-formed in the ﬁnal analysis.
//以下机翻
//2隐式转换序列只涉及参数的类型、cv质量和值类别，以及如何转换它们以匹配参数的相应属性。忽略其他属性，如寿命、存储类别、对齐、可访问性、听觉是否有点模糊，以及函数是否被删除(8.4.3)。
//因此，尽管对于给定的参数-参数对可以定义隐式转换序列，但是从参数到参数的转换在最终分析中可能仍然不正确。



//当B不是D的基类时
//static no check(B*, int);Host<B, D>()会选择operator B*() const;这个重载，
//static yes check(D*, T); Host<B, D>()会选择operator D*();这个重载，此时扔无法确定唯一调用的函数，会继续看下面的参数
//int是确定类型，而T是模板，调用check(Host<B, D>(), int())时，由于static no check(B*, int)，匹配相同类型而非模板，
//所以次情况会选择static no check(B*, int);作为最终的匹配函数

//当B是D的基类时
//static yes check(D*, T);Host<B, D>()选择的重载函数没有变化仍然是operator D*();
//对于static no check(B*, int); 	operator B*() const;	B*可以隐式转换成D*, 有非const要好于const的类型，所以选择
//									operator D*();

//会选择operator D*();作为候选，又因为static yes check(D*, T)没有做过隐式转换，比经过一次隐式转换的static no check(B*, int); 
//更好，所以会调用static yes check(D*, T)


//Test sample
class Base {};
class NewBase {};
class Derived : private Base {};




int main()
{
    std::cout << "Hello World!\n"; 
	std::cout << is_base_of<NewBase, Derived>::value<<std::endl;
	std::cout << is_base_of<Base, Derived>::value << std::endl;

}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
