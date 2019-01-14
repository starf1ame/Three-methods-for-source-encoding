//
//  main.cpp
//  Arithmetic
//
//

#include <iostream>
#include "Arithmetic.h"
#include <string>
#include <time.h>
using namespace std;

int main()
{
	Arithmetic test;   //实例化
	clock_t start, end;  
	start = clock();
	test.open("C:/Users/Administrator/Desktop/news.txt");  //打开文件
	test.Press();   //压缩文件
	end = clock();
	cout << endl<<endl<<"压缩用时：";
	cout << double (end - start) << "/ms" << endl << endl;   //显示压缩用时
	start = clock();
	test.Decode("C:/Users/Administrator/Desktop/news.txt.ari", "C:/Users/Administrator/Desktop/nextnews.txt");  //解压文件
	cout << "解压用时：" ;   
	cout << double (start - end)  << "/ms" << endl << endl;    //显示解压用时
	getchar();
	return 0;
}


