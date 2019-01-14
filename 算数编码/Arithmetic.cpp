//
//  Arithmetic.cpp
//  Arithmetic
//

#include "Arithmetic.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include<math.h>
using namespace std;


Arithmetic::Arithmetic()
{

}


void Arithmetic::open(string input)   //读入目标文件地址
{
	FileAddress = input;
}


void Arithmetic::Press()  //进行压缩
{
	cout.setf(ios::fixed);
	cout.setf(ios::showpoint);
	cout.precision(10);    // 定义输出小数的精度为小数点后10位

	ifstream read;
	read.open(FileAddress, ios::in | ios::binary);
	if (!read)
	{
		cout << "文件读取错误" << endl << endl;
		return;
	}

	ofstream write;
	write.open(FileAddress + ".ari", ios::out | ios::binary);
	if (!write)
	{
		cout << "输出文件不能建立(*.lz)" << endl;
		return;
	}

	char *InChar = new char;
	
	int len(0);
	int BitList[8];
	long int reg(0);
	long double C(0.0);
	long double B(0.0);
	long double A(1);

	read.read((char*)InChar, sizeof(unsigned char));    //读第一个字节
	while (!read.eof())   //未读到末尾继续循环
	{
		TotalNumber += 8;   //每读入一个字符，总bit数加8
		BitList[0] = (*InChar & 0x80) == 0x80 ? 1 : 0;
		BitList[1] = (*InChar & 0x40) == 0x40 ? 1 : 0;
		BitList[2] = (*InChar & 0x20) == 0x20 ? 1 : 0;
		BitList[3] = (*InChar & 0x10) == 0x10 ? 1 : 0;
		BitList[4] = (*InChar & 0x08) == 0x08 ? 1 : 0;
		BitList[5] = (*InChar & 0x04) == 0x04 ? 1 : 0;
		BitList[6] = (*InChar & 0x02) == 0x02 ? 1 : 0;
		BitList[7] = (*InChar & 0x01) == 0x01 ? 1 : 0;    //将字节分割成bit形式

		for (int i(0); i < 8; i++)       //判断bit为0还是1
		{
			if (BitList[i] == 0)
			{
				NumList[0] ++;
			}
			if (BitList[i] == 1)
			{
				NumList[1] ++;
			}
		}

		read.read((char*)InChar, sizeof(unsigned char));   //处理完上个字节的情况下，读下一个字节
	}

	cout <<endl<< "压缩的总bit数为：" << TotalNumber << endl;
	cout << "  其中0的个数为：" << NumList[0] << endl;
	cout << "  其中1的个数为：" << NumList[1] << endl;
	ProbaList[0] = long double(NumList[0]) / TotalNumber;    //计算0和1出现的概率
	ProbaList[1] = long double(NumList[1]) / TotalNumber;
	
	P[0] = 0;    //计算0和1概率区间的下限
	P[1] = ProbaList[0];   

	cout << endl << "bit0出现的概率为：" << "	" << ProbaList[0];
	cout << endl << "bit1出现的概率为：" << "	" << ProbaList[1];
	


	read.clear();    //将指针回至文件头
	read.seekg(0);

	read.read((char*)InChar, sizeof(unsigned char)); //读第一个字节
	while (!read.eof())
	{	
		for (int i(0); i < 1; i++)
		{
			BitList[0] = (*InChar & 0x80) == 0x80 ? 1 : 0;
			BitList[1] = (*InChar & 0x40) == 0x40 ? 1 : 0;
			BitList[2] = (*InChar & 0x20) == 0x20 ? 1 : 0;
			BitList[3] = (*InChar & 0x10) == 0x10 ? 1 : 0;
			BitList[4] = (*InChar & 0x08) == 0x08 ? 1 : 0;
			BitList[5] = (*InChar & 0x04) == 0x04 ? 1 : 0;
			BitList[6] = (*InChar & 0x02) == 0x02 ? 1 : 0;
			BitList[7] = (*InChar & 0x01) == 0x01 ? 1 : 0;
			
			for (int j(0); j < 8; j++)   //将字符串中的一个字节编码
			{
				if (BitList[j] == 0)
				{
					C = C;
					A = A * ProbaList[0];
				}

				else
				{
					C = C + A * P[1];
					A = A * ProbaList[1];
				}
			}

			read.read((char*)InChar, sizeof(unsigned char)); //读一个字节
			if (read.eof())  break;
		}
		
		len = -log(A) / log(2) + 1;  //确定输出精度
		B = C * (pow(10, len));
		B += 0.5;
		C = B / (pow(10, len));

		write << setiosflags(ios::fixed) << setprecision(len) << C;  //将压缩后的数据输出至压缩文件中
		write << "	";
		C = 0;   //将C和A复位
		A = 1;
		
	}

	read.close();
	write.close();
}





void Arithmetic::Decode(string sourcefile, string dstfile)
{
	cout.setf(ios::fixed);
	cout.setf(ios::showpoint);
	cout.precision(10);

	ifstream readfile;
	ofstream putfile;
	readfile.open(sourcefile);
	if (!readfile)
	{
		cout << "解压文件读取错误(*.ari)" << endl << endl;
		return;
	}

	putfile.open(dstfile);
	if (!putfile)
	{
		cout << "解压文件建立错误" << endl;
	}

	long double temp(0);   //用以保存读入的数据
	int OutList[8];
	int count(0);
	int OutChar(0);

	count = TotalNumber;
	readfile >> temp;  //读入第一个数据

	while (!readfile.eof())
	{
		for (int i(0); i < 1; i++)
		{
			if (count == 0)		break;  //如果已经读完文件，则退出循环

			for (int j(0); j < 8; j++)
			{
				if (temp >= P[1])
				{
					count -= 1;
					OutList[j] = 1;
					temp = (temp - P[1]) / ProbaList[1];				
				}

				else
				{
					count -= 1;
					OutList[j] = 0;
					temp = (temp - P[0]) / ProbaList[0];
				}
			}

			OutChar = OutList[7] + OutList[6] * 2 + OutList[5] * 4 + OutList[4] * 8 + OutList[3] * 16 + OutList[2] * 32 + OutList[1] * 64 - OutList[0];
			putfile << char(OutChar);   //以字节为单位输出
		}

		
		readfile >> temp;   //读下一个数据
	}

	readfile.close();
	putfile.close();
}
