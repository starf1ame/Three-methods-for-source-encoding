//
//  lz78.hpp or lz78.h
//
//  Created by xrosheart on 2018/11/17.
//  Copyright © 2018年 xrosheart. All rights reserved.
//


#ifndef LZ78_h
#define LZ78_h
#include <string>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

class LZ78
{
public:
	struct Dictionary //设置一个动态字典的单元
	{
		unsigned short preIndex; //2字节,前缀段号
		unsigned char lastChar; //最后一位字符
		unsigned int Index; //序列段号
		vector<unsigned char> stringInDic; //完整符号串
	};


	void lz_compress(string originfile); //压缩文件
	void lz_decompress(string sourcefile, string dstfile); //解压文件
private:
	bool IfStringInDic(vector<Dictionary*> CurrentString, vector<unsigned char> DataDic, unsigned int &Index);
	//判断字符串是否在字典中
	vector<unsigned char> FindPreString(vector<Dictionary> DataDic, unsigned int preindex);
	//找到并返回前缀字符串
};




#endif /* LZ78_hpp */



using namespace std;

void LZ78::lz_compress(string originfile)  //进行压缩
{
	ifstream read;
	read.open(originfile, ios::in | ios::binary);
	if (!read)
	{
		cout << "文件读取错误" << endl << endl;
		return;
	}

	ofstream write;
	write.open(originfile + ".lz", ios::out | ios::binary);
	if (!write)
	{
		cout << "输出文件不能建立(*.lz)" << endl;
	}

	unsigned char *firstchar = new unsigned char;
	read.read((char*)firstchar, sizeof(unsigned char)); //读1字节
	vector<Dictionary*> DataDic;    //使用vector建立字典
	while (!read.eof()) //未读到末尾继续循环
	{
		//分两类：第一个和其他
		if (DataDic.size() == 0)
		{
			Dictionary *firstDic = new Dictionary;
			firstDic->Index = 1;
			firstDic->preIndex = 0;
			firstDic->lastChar = *firstchar;
			firstDic->stringInDic.push_back(*firstchar);//用于后续查找
			DataDic.push_back(firstDic); //存入字典

			write.write((char*)firstDic, 3);
			//输出字典元前3字节的数据，即前缀段号和末尾符号
		}
		else
		{
			unsigned char *now = new unsigned char; //用于读取的字符
			unsigned char *reallast = new unsigned char;
			vector<unsigned char> CurrentString;
			unsigned int index = 0;  //字符串存在在字典中的位置， 初始设为0
			Dictionary *currentInfo = new Dictionary;   //存储当前的单词到字典单元中

			int EOFflag = 0;

			do
			{
				read.read((char*)now, sizeof(unsigned char));
				if (read.eof())
				{
					EOFflag = 1;  //标记是否到文件的结尾
					break;
				}
				else
				{
					CurrentString.push_back(*now);
				}
			} while (IfStringInDic(DataDic, CurrentString, index));
			//上面循环到遇到第一个前缀在字典出现的新字符串
			if (EOFflag == 1) //说明已经读取完毕
			{
				if (CurrentString.size() == 0)
				{
					break;  //如果当前字符串中没有字符，直接跳出循环
				}
				else
				{     //如果当前字符串中有字符，对这段字符进行压缩。
					*reallast = CurrentString[CurrentString.size() - 1]; //取末位
					CurrentString.erase(CurrentString.end() - 1);
				}

			}
			else
			{
				*reallast = *now;
			}
			currentInfo->Index = DataDic.size() + 1;
			currentInfo->lastChar = *reallast;
			currentInfo->preIndex = index;
			currentInfo->stringInDic = CurrentString;
			DataDic.push_back(currentInfo);
			write.write((char*)currentInfo, 3);
			delete[]now;
			delete[]reallast;
			//delete[]currentInfo;
		}

	}
	delete[]firstchar;
	read.close();
	write.close();
}

bool LZ78::IfStringInDic(vector<Dictionary*> DataDic, vector<unsigned char> CurrentString, unsigned int &Index)
{
	int flag = 0;
	for (int i = 0; i < DataDic.size(); i++)
	{
		if (CurrentString == DataDic[i]->stringInDic)
		{
			Index = DataDic[i]->Index;
			flag = 1;
			return true;
		}

	}
	return false;
}



void LZ78::lz_decompress(string sourcefile, string dstfile)
{
	ifstream readfile;
	ofstream putfile;
	readfile.open(sourcefile, ios::in | ios::binary);
	putfile.open(dstfile, ios::out | ios::binary);
	Dictionary *now = new Dictionary;
	readfile.read((char*)now, 3);
	vector<Dictionary> DataDic;    //建立字典
	Dictionary *spacefirst = new Dictionary;
	spacefirst->Index = 0;
	spacefirst->lastChar = '0';
	spacefirst->preIndex = 0;
	DataDic.push_back(*spacefirst);
	while (!readfile.eof())
	{
		now->Index = DataDic.size();
		vector<unsigned char> preString;  //存储前一个的字符串
		if (now->preIndex != 0)
		{ //如果preIndex等于0那么此字符是新出现的字符，否则在前面字典中找
			preString = FindPreString(DataDic, now->preIndex);
		}
		preString.push_back(now->lastChar);
		now->stringInDic = preString;  //获取此单词的字符串。
		DataDic.push_back(*now);
		for (int i = 0; i < preString.size(); i++)
		{
			putfile.write((char*)&preString[i], sizeof(unsigned char));
		}
		readfile.read((char*)now, 3);

	}
	readfile.close();
	putfile.close();
}

vector<unsigned char>  LZ78::FindPreString(vector<Dictionary> DataDic, unsigned int preindex)
{
	for (int i = 0; i < DataDic.size(); i++)
	{
		if (preindex == DataDic[i].Index)
		{
			return DataDic[i].stringInDic;
		}
	}
	return DataDic[0].stringInDic;
}
