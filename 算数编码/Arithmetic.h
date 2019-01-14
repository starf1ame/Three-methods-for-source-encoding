//
//  Arithmetic.h
//  Arithmetic
//
//

#ifndef Arithmetic_h
#define Arithmetic_h

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <wchar.h>
using namespace std;

class Arithmetic
{
public:
	string FileAddress; //操作对象文件地址
	int TotalNumber = 0;  //记录总的bit数
	int NumList[2] = {0};   //存储0和1出现的次数
	long double ProbaList[2] = {0.0};   //存储0和1出现的概率
	long double P[2] = {0.0};     //存储0和1概率区间的下限
	Arithmetic();   //构造函数
	void open(string input);
	void Press();   //压缩文件
	void Decode(string sourcefile, string dstfile); //解压文件

};




#endif /* Arithmetic.h */
#pragma once
#pragma once
