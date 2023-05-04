#include <iostream>
#include "mycomplex.h"

using namespace std;

int main(void)
{
	MyComplex c1(1.5, 2.9);
	MyComplex c2(1.8);
	MyComplex c4;

	MyComplex c3 = c1 + c2; // c1.operator+(c2)

	c3.show();
	cout << c3 << endl;

	c4 = c1 - c2;
	c4.show();

	/*
	 15.5 + c1
	15.5.operator+
	函数的参数传参的时候会调用转换构造方法

	运算符重载：
		没有严格要求什么运算符全局函数，或者类的成员函数
		像一切有对称性运算符尽量用全局函数，例如+ - * / 
		尽量使用类的成员函数实现
		只支持类的成员函数运算符重载的运算符有:-> [] = ()
		并不是所有的运算符都支持重载，例如. :: sizeof ?: 不支持重载
	 */
	// operator+(MyComplex(15.5), c2)
	MyComplex c5 = 15.5 + c2; // MyComplex(15.5)--->15.5+0.0i
	// operator<<(cout, c5)
	cout << c5 << endl;

	cin >> c5;
	cout << c5 << endl;

	MyComplex c6 = c5 ++;
	cout << "c6:" << c6 << endl;

	c5 = ++c6;
	cout << "c5:" << c5 << endl;


	return 0;
}

