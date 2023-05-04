#ifndef __COMPLEX_H__
#define __COMPLEX_H__

#include <iostream>

using namespace std;

class MyComplex {
	public:
		MyComplex(float real = 0.0, float img = 0.0);

		// 运算符的重载
		// MyComplex operator+(const MyComplex &b);

		// 转换构造方法
		MyComplex(double d) : m_real(d), m_img(0.0) {}

		friend MyComplex operator-(const MyComplex &a, const MyComplex &b);
		friend MyComplex operator+(const MyComplex &a, const MyComplex &b);
		friend ostream &operator<< (ostream &out, const MyComplex &c);
		friend istream &operator>> (istream &in, MyComplex &c);

		// 补充其他运算 + - * / += -=

		MyComplex operator++(); // 前置++
		MyComplex operator++(int); //后置++

		void show();

	private:
		float m_real;
		float m_img;
};

#endif

