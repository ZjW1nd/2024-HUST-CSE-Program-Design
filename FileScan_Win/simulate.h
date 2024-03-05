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
//三个增删改操作，一个实际操作函数，增和改没问题
void insert_node(const char* parent_name, file_TreeNode* new_node);
void delete_node_cascade(const char* name);
void manipulate(char* path, char* opt, unsigned long long time, unsigned long long size);

//辅助函数
char* get_parent_directory(const char* path);
void print_dirinfo(dir_info* mystat,int id);
file_TreeNode* find_node(const char* name);

//两个读取数据文件函数
void read_myfile(const char* filename);
void read_mystat(const char* filename);

//四个统计函数,没问题
unsigned long long total_size(const char* directory);
int total_files(const char* directory);
file_TreeNode* earliest_file(const char* directory);
file_TreeNode* latest_file(const char* directory);

//比较、更新结果
void compare_dirinfo(dir_info mystat[], int i);

#endif