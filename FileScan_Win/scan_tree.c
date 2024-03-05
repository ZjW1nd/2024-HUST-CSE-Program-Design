#define _CRT_SECURE_NO_WARNINGS
// findfirstfile返回一个操作当前目录的句柄，findnextfile返回一个bool值
// 统一字符类型 wchar/char
#include "scan_tree.h"
#include "stack.h"
#define MAX_PATH_LENGTH 1000//255爆了

//将文件时间转换为time_t类型
time_t FileTimeToTime_t(const FILETIME* filetime)
{
    ULARGE_INTEGER ull;
    ull.LowPart = filetime->dwLowDateTime;
    ull.HighPart = filetime->dwHighDateTime;

    return (ull.QuadPart / 10000000ULL - 11644473600ULL);
}

//创建节点
file_TreeNode* create_node(const char* name, unsigned long size, time_t lastModified, BOOL is_dir) {
	file_TreeNode* node = (file_TreeNode*)malloc(sizeof(file_TreeNode));
	node->name = _strdup(name);
	node->size = size;
	node->lastModified = lastModified;
    node->is_dir = is_dir;
	node->firstChild = NULL;
	node->nextSibling = NULL;
    //printf("%s\n", node->name); 调试用

	return node;
}

//计算树的深度
int calculate_filetree_depth(file_TreeNode* root) {
    Stack3* stack = initStack3();
    NodeDepth* rootDepth = (NodeDepth*)malloc(sizeof(NodeDepth));
    rootDepth->node = root;
    rootDepth->depth = 1;
    push3(stack, rootDepth);
    int maxDepth = 0;

    while (stack->top != -1) {
        NodeDepth* current = pop3(stack);
        if (current->depth > maxDepth) {
            maxDepth = current->depth;
        }
        file_TreeNode* child = current->node->firstChild;
        while (child != NULL) {
            NodeDepth* childDepth = (NodeDepth*)malloc(sizeof(NodeDepth));
            childDepth->node = child;
            childDepth->depth = current->depth + 1;
            push3(stack, childDepth);
            child = child->nextSibling;
        }
        free(current);
    }
    while (stack->top != -1) {
        NodeDepth* current = pop3(stack);
        free(current);
    }
    dropStack3(stack);
    return maxDepth;
}

int calculate_bintree_depth(file_TreeNode* root) {
    if (root == NULL) {
        return 0;
    }
    Stack3* stack = initStack3();
    NodeDepth* rootDepth = (NodeDepth*)malloc(sizeof(NodeDepth));
    rootDepth->node = root;
    rootDepth->depth = 1;
    push3(stack, rootDepth);

    int maxDepth = 0;
    while (stack->top != -1) {
        NodeDepth* current = pop3(stack);
        file_TreeNode* node = current->node;
        int depth = current->depth;
        free(current);

        if (node != NULL) {
            maxDepth = maxDepth > depth ? maxDepth : depth;
            if (node->firstChild != NULL) {
                NodeDepth* childDepth = (NodeDepth*)malloc(sizeof(NodeDepth));
                childDepth->node = node->firstChild;
                childDepth->depth = depth + 1;
                push3(stack, childDepth);
            }
            if (node->nextSibling != NULL) {
                NodeDepth* siblingDepth = (NodeDepth*)malloc(sizeof(NodeDepth));
                siblingDepth->node = node->nextSibling;
                siblingDepth->depth = depth + 1;
                push3(stack, siblingDepth);
            }
        }
    }

    dropStack3(stack);
    return maxDepth;
}

//扫描目录并建树，核心函数
file_TreeNode* scan_into_tree(const char* directory) {
    //栈1用来存放目录，栈2用来存放节点
    Stack* dirPathStack = initStack();
    Stack2* dirNodeStack = initStack2();
    push(dirPathStack, directory);
    file_TreeNode* root = create_node(directory, 0, 0, TRUE);
    push2(dirNodeStack, root);
    //树的信息
    DirStats* root_stats = (DirStats*)malloc(sizeof(DirStats));
    root_stats->longestPath = _strdup(directory);
    root_stats->maxDepth = 0;
    root_stats->totalDirCount = 0;
    root_stats->totalFileCount = 0;
    //进度条
    long long i = 0;
    const char* lable = "|/-\\";
    while (dirPathStack->top != -1) {
        file_TreeNode* cur_dir_node = pop2(dirNodeStack);
        char* cur_dir = pop(dirPathStack);
        WIN32_FIND_DATAA file_info;

        file_TreeNode* last_child = NULL;
        char search_path[MAX_PATH_LENGTH];
        strcpy(search_path, cur_dir);
        strcat(search_path, "\\*.*");

        HANDLE hFind = FindFirstFileA(search_path, &file_info);
        if (hFind == INVALID_HANDLE_VALUE) {
            DWORD err_code = GetLastError();
            //会报错的几个，我自己手动去看也是没有，不知道怎么查出来的文件名
            //printf("No files found\nError code: %ul\n", err_code);
            continue;
        }
        do {
            if (strcmp(file_info.cFileName, ".") != 0 && strcmp(file_info.cFileName, "..") != 0) {
                char full_path[MAX_PATH_LENGTH];
                memcpy(full_path, cur_dir, strlen(cur_dir) + 1);
                strcat(full_path, "\\");
                strcat(full_path, file_info.cFileName);

                unsigned long long size = ((unsigned long long)file_info.nFileSizeHigh * ((unsigned long long)MAXDWORD + 1)) + (unsigned long long)file_info.nFileSizeLow;
                time_t lastModified = FileTimeToTime_t(&file_info.ftLastWriteTime);
                BOOL is_dir = file_info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;

                file_TreeNode* cur_node = create_node(full_path, size, lastModified, is_dir);
                /*在这里实现一个进度条*/
                printf("正在扫描... 已扫描:[%lld][%c]\r", i, lable[i % 4]);
                fflush(stdout);
                i++;
                //建树
                if (cur_node != NULL) {
                    if (cur_dir_node->firstChild == NULL) {
                        cur_dir_node->firstChild = cur_node;
                    }
                    else {
                        last_child->nextSibling = cur_node;
                    }
                    last_child = cur_node;
                }
                int pathLength = strlen(full_path);
                if (pathLength > strlen(root_stats->longestPath)) {
                    free(root_stats->longestPath);
                    root_stats->longestPath = _strdup(full_path);
                }
                if (is_dir) {
                    root_stats->totalFileCount++;
                    push(dirPathStack, full_path);
                    push2(dirNodeStack, cur_node);
                }
                else {
                    root_stats->totalDirCount++;
                }
            }
        } while (FindNextFileA(hFind, &file_info));
        FindClose(hFind);
    }
    putchar('\n');
    putchar('\n');
    root_stats->maxDepth = calculate_filetree_depth(root);
    printf("文件总数: %d\n\n", root_stats->totalFileCount);
    printf("目录总数: %d\n\n", root_stats->totalDirCount);
    printf("目录最大深度（目录层数）: %d\n\n", root_stats->maxDepth);
    printf("最长路径名: %s\n\n", root_stats->longestPath);
    printf("最长路径名长度: %d\n\n", strlen(root_stats->longestPath));
    printf("文件二叉树最大深度：%d\n\n", calculate_bintree_depth(root));
    dropStack(dirPathStack);
    dropStack2(dirNodeStack);
    free(root_stats);
    return root;
}

//打印树
void printTree(file_TreeNode* root) {
    if (root == NULL) {
        return;
    }
    Stack2* stack = initStack2();//Stack* 的指针赋成stack2了，但是也能用，毕竟指针类型
    push2(stack, root);

    while (stack->top != -1) {
        file_TreeNode* node = (file_TreeNode*)pop2(stack);
        if (!strcmp(node->name, "C:\\Windows\\addins"))
            getchar();
        printf("路径： %s\n\n", node->name);
        printf("文件大小： %lu\n", node->size);
        printf("修改时间： %s", ctime(&node->lastModified));
        if (node->firstChild != NULL) {
            push2(stack, node->firstChild);
        }
        if (node->nextSibling != NULL) {
            push2(stack, node->nextSibling);
        }

    }
    while (stack->top != -1) {
        pop2(stack);
    }
    dropStack2(stack);
}