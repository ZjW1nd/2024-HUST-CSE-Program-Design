#pragma once
#ifndef _SCAN_TREE_
#define _SCAN_TREE_

#include <Windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
typedef struct DirStats {
    int totalFileCount;
    int totalDirCount;
    int maxDepth;
    char* longestPath;
} DirStats;
typedef struct file_TreeNode {
    char* name;
    unsigned long long size;
    time_t lastModified;
    BOOL is_dir;
    struct file_TreeNode* firstChild;
    struct file_TreeNode* nextSibling;
} file_TreeNode;
typedef struct NodeDepth {
    file_TreeNode* node;
    int depth;
} NodeDepth;
time_t FileTimeToTime_t(const FILETIME* filetime);
file_TreeNode* create_node(const char* name, long size, time_t lastModified, BOOL is_dir);
file_TreeNode* scan_into_tree(const char* directory);
void printTree(file_TreeNode* root);

#endif // !_SCAN_TREE_