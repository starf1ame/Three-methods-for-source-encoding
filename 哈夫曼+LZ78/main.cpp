#include "huffman.h"
#include "lz78.h"
#include<time.h>
using namespace std;

int main()
{
	while (1)
	{
		char infilename[256], outfilename[256];
		char choice1;
		int flag = 0;			//判断输入文件是否存在的标识符
		MainMeun();
		char choice2;
		cout << "[编码方式]:" << endl;
		cout << "1--哈夫曼编码" << endl;
		cout << "2--LZ-78编码" << endl;
		cin >> choice2;
		cout << "请输入您的选项(Q/C/D):";
		cin >> choice1;
		switch (choice1)
		{
		case 'C':
			cout << "[源文件]:";
			cin >> infilename;
			cout << "[目标文件]:";
			cin >> outfilename;
			switch (choice2)
			{case '1':
				clock_t start1, end1;
				cout << "压缩中……\n" << endl;
				start1 =clock();
			    flag = compress(infilename, outfilename);
				end1 = clock();
			    cout << "压缩完成\n" << endl;
				cout << "压缩文件用时：" << endl << endl;
				cout << double(end1 - start1)  << "/ms" << endl;
				break;
			case '2':
				LZ78 document;
				clock_t start2, end2;
				start2 = clock();
				cout << "压缩中……\n" << endl;
				document.lz_compress(infilename);   //压缩文件
				end2 = clock();
				cout << "压缩完成\n" << endl;
				cout << "压缩文件用时：" << endl << endl;
			    cout << double((end2 - start2)/CLOCKS_PER_SEC) << "/s" << endl;
				break;
			default:
				break;
			};
		    break;
		case 'D':
			cout << "[源文件]:";
			cin >> infilename;
			cout << "[目标文件]:";
			cin >> outfilename;
			switch (choice2)
			{
			case '1':
				clock_t start3, end3;
				start3 = clock();
				cout << "解压中……\n" << endl;
				flag = extract(infilename, outfilename);
				end3 = clock();
				cout << "解压完成\n" << endl;
				cout << "解压文件用时：" << endl << endl;
				cout << double(end3 - start3)  << "/ms" << endl;
				break;
			case '2':
				clock_t start4, end4;
				start4 = clock();
				LZ78 document2;
				cout << "解压中……\n" << endl;
				document2.lz_decompress(infilename, outfilename);
				cout << "压缩完成\n" << endl;
				end4 = clock();
				cout << "解压用时：" << endl << endl;
				cout << double((end4-start4)/CLOCKS_PER_SEC) << "/s" << endl;
				break;
			}
			break;
		case 'Q':
			exit(0); break;
			if (flag == -1)
				if (flag == -1)
					cout << "无法找到文件" << endl;

		}
	}
	return 0;
}
