#include <iostream>
#include "mycomplex.h"

MyComplex::MyComplex(float real, float img) : m_real(real), m_img(img) {}

MyComplex operator+(const MyComplex &a, const MyComplex &b)
{
	MyComplex c;
	c.m_real = a.m_real + b.m_real;
	c.m_img = a.m_img + b.m_img;

	return c;
}

void MyComplex::show()
{
	std::cout << this->m_real << "+" << this->m_img << "i" << std::endl;
}

// 全局函数实现运算符重载
MyComplex operator-(const MyComplex &a, const MyComplex &b)
{
	MyComplex c;

	c.m_real = a.m_real - b.m_real;
	c.m_img = a.m_img - b.m_img;

	return c;
}

/*
 为什么返回引用?
 	连续输出
 */
ostream &operator<< (ostream &out, const MyComplex &c)
{
	out << c.m_real << "+" << c.m_img << "i";

	return out; 
}


istream &operator>> (istream &in,  MyComplex &c)
{
	in >> c.m_real >> c.m_img;

	return in;
}

MyComplex MyComplex::operator++()
{
	this->m_real++;
	this->m_img++;

	return *this;
}

MyComplex MyComplex::operator++(int)
{
	MyComplex ret;

	ret = *this;
	this->m_real++;
	this->m_img++;

	return ret;
}


