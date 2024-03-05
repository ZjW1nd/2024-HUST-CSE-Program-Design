#pragma once
#ifndef _STACK_
#define _STACK_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scan_tree.h"
#define STACK_SIZE 0xBFFFF
//�ڶ����ֶ�ʵ��һ��ջ�洢�����������ֹ�����ݹ鱬ջ
typedef struct Stack { //ջ�ṹ
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

Stack* initStack(void); //��ʼ��ջ
Stack2* initStack2(void);
void dropStack(Stack* stack);  //����ջ
void dropStack2(Stack2* stack);
void push(Stack* stack, const char* item); //��ջ
char* pop(Stack* stack); //��ջ
void push2(Stack2* stack, file_TreeNode* item);
file_TreeNode* pop2(Stack2* stack);

// void printStack(Stack* stack); //��ӡջ����
// Path: stack.c

#endif // !_STACK_