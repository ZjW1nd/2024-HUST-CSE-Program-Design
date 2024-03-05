#include "stack.h"

Stack* initStack(void) { //初始化栈
    Stack* stack = (Stack*)malloc(sizeof(Stack));
    stack->top = -1;
    return stack;
}

Stack2* initStack2(void) {
	Stack2* stack = (Stack2*)malloc(sizeof(Stack2));
	stack->top = -1;
	return stack;
}

void dropStack(Stack* stack) { //销毁栈
    for (int i = 0; i <= stack->top; i++) {
        free(stack->items[i]);
    }
    free(stack);
}

void dropStack2(Stack2* stack) {
	for (int i = 0; i <= stack->top; i++) {
		free(stack->items[i]);
	}
	free(stack);
}

void push(Stack* stack, const char* item) { //入栈
    if (stack->top == STACK_SIZE - 1) {
        printf("Stack is full\n");
        return;
    }
    stack->items[++stack->top] = _strdup(item);
}

void push2(Stack2* stack, file_TreeNode* item) {
	if (stack->top == STACK_SIZE - 1) {
		printf("Stack is full\n");
		return;
	}
	stack->items[++stack->top] = item;
}

char* pop(Stack* stack) { //出栈
    if (stack->top == -1) {
        printf("Stack is empty\n");
        return NULL;
    }
    return stack->items[stack->top--];
}

file_TreeNode* pop2(Stack2* stack) {
	if (stack->top == -1) {
		printf("Stack is empty\n");
		return NULL;
	}
	return stack->items[stack->top--];
}

Stack3* initStack3(void) {
    Stack3* stack = (Stack3*)malloc(sizeof(Stack3));
    stack->top = -1;
    return stack;
}

void dropStack3(Stack3* stack) {
    for (int i = 0; i <= stack->top; i++) {
        free(stack->items[i]);
    }
    free(stack);
}

void push3(Stack3* stack, NodeDepth* item) {
    if (stack->top == STACK_SIZE - 1) {
        printf("Stack is full\n");
        return;
    }
    stack->items[++stack->top] = item;
}

NodeDepth* pop3(Stack3* stack) {
    if (stack->top == -1) {
        printf("Stack is empty\n");
        return NULL;
    }
    return stack->items[stack->top--];
}

//void printStack(Stack* stack) {
//    if (stack->top == -1) {
//        printf("Stack is empty\n");
//        return;
//    }
//    printf("Stack contents:\n");
//    for (int i = stack->top; i >= 0; i--) {
//        printf("%s\n", stack->items[i]);
//    }
//}
