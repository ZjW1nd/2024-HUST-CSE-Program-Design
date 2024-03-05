#pragma once
#ifndef _STACK_
#define _STACK_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scan_tree.h"
#define STACK_SIZE 0xBFFFF
//在堆上手动实现一个栈存储遍历结果，防止函数递归爆栈
typedef struct Stack { //栈结构
    char* items[STACK_SIZE];
    int top;
} Stack;
typedef struct Stack_for_node {
    file_TreeNode* items[STACK_SIZE];
    int top;
} Stack2;
typedef struct Stack_for_nodeDepth {
    NodeDepth* items[STACK_SIZE];
    int top;
} Stack3;

Stack3* initStack3(void);
void dropStack3(Stack3* stack);
void push3(Stack3* stack, NodeDepth* item);
NodeDepth* pop3(Stack3* stack);

Stack* initStack(void); //初始化栈
Stack2* initStack2(void);
void dropStack(Stack* stack);  //销毁栈
void dropStack2(Stack2* stack);
void push(Stack* stack, const char* item); //入栈
char* pop(Stack* stack); //出栈
void push2(Stack2* stack, file_TreeNode* item);
file_TreeNode* pop2(Stack2* stack);

// void printStack(Stack* stack); //打印栈内容
// Path: stack.c

#endif // !_STACK_