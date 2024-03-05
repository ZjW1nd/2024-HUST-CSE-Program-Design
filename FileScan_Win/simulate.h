#pragma once
#ifndef _SIMULATE_
#define _SIMULATE_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"
#include "scan_tree.h"
#define STAT_COUNT 207
typedef struct dir_info {
	char* path;
	unsigned long long size;
	int file_count;
	char* earliest;
	char* latest;
} dir_info;
//������ɾ�Ĳ�����һ��ʵ�ʲ������������͸�û����
void insert_node(const char* parent_name, file_TreeNode* new_node);
void delete_node_cascade(const char* name);
void manipulate(char* path, char* opt, unsigned long long time, unsigned long long size);

//��������
char* get_parent_directory(const char* path);
void print_dirinfo(dir_info* mystat,int id);
file_TreeNode* find_node(const char* name);

//������ȡ�����ļ�����
void read_myfile(const char* filename);
void read_mystat(const char* filename);

//�ĸ�ͳ�ƺ���,û����
unsigned long long total_size(const char* directory);
int total_files(const char* directory);
file_TreeNode* earliest_file(const char* directory);
file_TreeNode* latest_file(const char* directory);

//�Ƚϡ����½��
void compare_dirinfo(dir_info mystat[], int i);

#endif