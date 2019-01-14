//
//  huffman.hpp or huffman.h
//
//  Created by xrosheart on 2018/11/16.
//  Copyright © 2018年 xrosheart. All rights reserved.
//

#ifndef HUFFMAN_H
#define HUFFMAN_H
#pragma warning(disable:4996)
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

using namespace std;

//哈夫曼节点
struct HufNode
{
	unsigned char uch;                      //记录字符
	unsigned long weight;                   //权重
	char *code;                             //存放编码的数组
	int parent, left_child, right_child;    //定义双亲，左孩子，右孩子
	struct HufNode *HufTree;
};

typedef struct HufNode HufNode, *HufTree;

//用于统计字符出现频度的节点
struct Node
{
	unsigned char uch;
	unsigned long weight;
};

//找到两个权值最小的子节点
void Find2Min(HufNode *huf_tree, unsigned int n, int *s1, int *s2)
{
	unsigned int i;
	unsigned long min = ULONG_MAX;
	for (i = 0; i < n; i++)
		if (huf_tree[i].parent == 0 && huf_tree[i].weight < min)
		{
			min = huf_tree[i].weight;
			*s1 = i;
		}
	huf_tree[*s1].parent = 1;

	min = ULONG_MAX;//赋值无穷大正数
	for (i = 0; i < n; i++)
		if (huf_tree[i].parent == 0 && huf_tree[i].weight < min)
		{
			min = huf_tree[i].weight;
			*s2 = i;
		}
}

//建立哈夫曼树
void BuildTree(HufNode *huf_tree, unsigned int char_sum, unsigned int node_num)
{
	unsigned int i;
	int s1, s2;
	for (i = char_sum; i < node_num; i++)
	{
		Find2Min(huf_tree, i, &s1, &s2);
		huf_tree[s1].parent = huf_tree[s2].parent = i;
		huf_tree[i].left_child = s1;
		huf_tree[i].right_child = s2;
		huf_tree[i].weight = huf_tree[s1].weight + huf_tree[s2].weight;
	}
}

//对完成排序的哈夫曼树进行编码
void HufCode(HufNode *huf_tree, unsigned char_sum)
{
	unsigned int i;
	int cur, next, index;
	char *code_tmp = (char *)malloc(256 * sizeof(char));
	code_tmp[256 - 1] = '\0';

	for (i = 0; i < char_sum; i++)
	{
		index = 256 - 1;
		for (cur = i, next = huf_tree[i].parent; next != 0;
			cur = next, next = huf_tree[next].parent)
			if (huf_tree[next].left_child == cur)
				code_tmp[--index] = '0';
			else
				code_tmp[--index] = '1';
		huf_tree[i].code = (char *)malloc((256 - index) * sizeof(char));
		strcpy(huf_tree[i].code, &code_tmp[index]);
	}
	free(code_tmp);
}

//压缩函数
int compress(char *ifname, char *ofname)
{
	unsigned int i, j;                    //用于循环计数
	unsigned int char_sum;		    //记录出现的字符种类数
	unsigned long file_len = 0;	//文件字节数	
	unsigned int node_num;       //哈夫曼树中节点数	
	unsigned char temp_char;

	FILE *infile, *outfile;
	Node node_temp;
	HufTree huf_tree;
	char code_buf[256] = "\0";
	unsigned int code_len;
	Node *tmp_nodes = (Node *)malloc(256 * sizeof(Node));

	//初始化节点
	for (i = 0; i < 256; i++)
	{
		tmp_nodes[i].weight = 0;
		tmp_nodes[i].uch = (unsigned char)i;
	}
	infile = fopen(ifname, "rb");
	if (infile == NULL)
		return -1;
	fread((char *)&temp_char, 1, 1, infile);
	while (!feof(infile))
	{
		tmp_nodes[temp_char].weight++;
		file_len++;			//统计文件字节数
		fread((char *)&temp_char, 1, 1, infile);
	}
	fclose(infile);
	for (i = 0; i < 256 - 1; i++)
		for (j = i + 1; j < 256; ++j)
			if (tmp_nodes[i].weight < tmp_nodes[j].weight)
			{
				node_temp = tmp_nodes[i];
				tmp_nodes[i] = tmp_nodes[j];
				tmp_nodes[j] = node_temp;
			}
	for (i = 0; i < 256; i++)
		if (tmp_nodes[i].weight == 0)
			break;
	char_sum = i;

	//考虑字符种类仅为1的特殊情况
	if (char_sum == 1)
	{
		outfile = fopen(ofname, "wb");
		fwrite((char *)&char_sum, 4, 1, outfile);
		fwrite((char *)&tmp_nodes[0].uch, 1, 1, outfile);
		fwrite((char *)&tmp_nodes[0].weight, 4, 1, outfile);
		free(tmp_nodes);
		fclose(outfile);
	}
	else
	{
		node_num = 2 * char_sum - 1;
		huf_tree = (HufNode *)malloc(node_num * sizeof(HufNode));
		for (i = 0; i < char_sum; i++)
		{
			huf_tree[i].uch = tmp_nodes[i].uch;
			huf_tree[i].weight = tmp_nodes[i].weight;
			huf_tree[i].parent = 0;			//初始化节点
		}
		free(tmp_nodes);
		for (; i < node_num; i++)
			huf_tree[i].parent = 0;

		BuildTree(huf_tree, char_sum, node_num);		//建立哈夫曼树
		HufCode(huf_tree, char_sum);							//对树进行编码
		outfile = fopen(ofname, "wb");
		fwrite((char *)&char_sum, 4, 1, outfile);

		//将编码表存入输出文件
		for (i = 0; i < char_sum; i++)
		{
			fwrite((char *)&huf_tree[i].uch, 1, 1, outfile);
			fwrite((char *)&huf_tree[i].weight, 4, 1, outfile);
		}
		fwrite((char *)&file_len, 4, 1, outfile);
		infile = fopen(ifname, "rb");
		fread((char *)&temp_char, 1, 1, infile);
		while (!feof(infile))
		{
			for (i = 0; i < char_sum; i++)
				if (temp_char == huf_tree[i].uch)
					strcat(code_buf, huf_tree[i].code);
			//当 code_buf中字符长度大于8时，一直处理写入，直至小于8
			while (strlen(code_buf) >= 8)
			{
				temp_char = '\0';
				for (i = 0; i < 8; i++)
				{
					temp_char <<= 1;
					if (code_buf[i] == '1')
						temp_char |= 1;
				}
				fwrite((char *)&temp_char, 1, 1, outfile);
				strcpy(code_buf, code_buf + 8);		//字符串前移8位
			}//当此循环结束时，表示 buf[]中已经小于8了，没到文件末尾，读下一个，否则退出
			fread((char *)&temp_char, 1, 1, infile);
		} //while 此层循环退出时，表示已到末尾,再判断 buf 中是否写完，若没写完，对其进行处理

		code_len = strlen(code_buf);
		if (code_len > 0)
		{
			temp_char = '\0';
			for (i = 0; i < code_len; i++)
			{
				temp_char <<= 1;
				if (code_buf[i] == '1')
					temp_char |= 1;
			}
			temp_char <<= 8 - code_len;				//将不足的位数补0
			fwrite((char *)&temp_char, 1, 1, outfile);
		}
		fclose(infile);
		fclose(outfile);
		for (i = 0; i < char_sum; i++)
			free(huf_tree[i].code);			//释放空间
		free(huf_tree);
	}
	return 0;
}

int extract(char *ifname, char *ofname)
{
	unsigned int i;
	unsigned long file_len;
	unsigned long writen_len = 0;
	FILE *infile, *outfile;
	unsigned int char_sum;
	unsigned int node_num;
	HufTree huf_tree;
	unsigned char code_temp;
	unsigned int root;
	infile = fopen(ifname, "rb");
	if (infile == NULL)
		return -1;
	fread((char *)&char_sum, 4, 1, infile);			//获得字符种类数
	if (char_sum == 1)
	{
		fread((char *)&code_temp, 1, 1, infile);
		fread((char *)&file_len, 4, 1, infile);
		outfile = fopen(ofname, "wb");
		while (file_len--)
			fwrite((char *)&code_temp, 1, 1, outfile);
		fclose(infile);
		fclose(outfile);
	}
	else
	{
		node_num = 2 * char_sum - 1;
		huf_tree = (HufNode *)malloc(node_num * sizeof(HufNode));
		//重新获得哈夫曼树信息
		for (i = 0; i < char_sum; i++)
		{
			fread((char *)&huf_tree[i].uch, 1, 1, infile);
			fread((char *)&huf_tree[i].weight, 4, 1, infile);
			huf_tree[i].parent = 0;
		}
		for (; i < node_num; i++)
			huf_tree[i].parent = 0;
		BuildTree(huf_tree, char_sum, node_num);		//建立哈夫曼树
		fread((char *)&file_len, 4, 1, infile);
		outfile = fopen(ofname, "wb");
		root = node_num - 1;			//获得根节点位置
		while (1)
		{
			fread((char *)&code_temp, 1, 1, infile);
			for (i = 0; i < 8; i++)
			{
				if (code_temp & 128)
					root = huf_tree[root].right_child;
				else
					root = huf_tree[root].left_child;

				if (root < char_sum)
				{
					//编码还原后写入文件中
					fwrite((char *)&huf_tree[root].uch, 1, 1, outfile);
					++writen_len;
					if (writen_len == file_len) break;
					root = node_num - 1;
				}
				code_temp <<= 1;
			}
			if (writen_len == file_len) break;
		}
		fclose(infile);
		fclose(outfile);
		free(huf_tree);
	}
	return 0;
}


void MainMeun()
{
	cout << "*********************压缩程序**************************" << endl;
	cout << "*                                                     *" << endl;
	cout << "*********************Q----Quit*************************" << endl;
	cout << "*********************C----Coding***********************" << endl;
	cout << "*********************D----Decoding*********************" << endl;

}


#endif
