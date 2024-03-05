#define _CRT_SECURE_NO_WARNINGS
#include "simulate.h"
extern file_TreeNode* root;
dir_info my_stat[STAT_COUNT];
//----------目录树操作----------

//插入节点
void insert_node(const char* parent_name, file_TreeNode* new_node) {
    Stack2* stack = initStack2();
    push2(stack, root);
    while (stack->top != -1) {
        file_TreeNode* node = pop2(stack);

        if (strcmp(node->name, parent_name) == 0) {
            new_node->nextSibling = node->firstChild;
            node->firstChild = new_node;
            break;
        }

        if (node->firstChild != NULL) {
            push2(stack, node->firstChild);
        }
        if (node->nextSibling != NULL) {
            push2(stack, node->nextSibling);
        }
    }
    while (stack->top != -1)
    {
        pop2(stack);
    }
    dropStack2(stack);
}

//删除以指定节点为根节点的树
void delete_subtree(file_TreeNode* root) {
    Stack2* delete_stack = initStack2();
    push2(delete_stack, root);
    while (delete_stack->top != -1) {
        file_TreeNode* delete_node = pop2(delete_stack);
        if (delete_node->firstChild != NULL) {
            push2(delete_stack, delete_node->firstChild);
        }
        free(delete_node);
    }
    dropStack2(delete_stack);
}

//级联删除节点
void delete_node_cascade(const char* name) {
    Stack2* stack = initStack2();
    push2(stack, root);

    while (stack->top != -1) {
        file_TreeNode* node = pop2(stack);

        // 处理firstChild
        if (node->firstChild != NULL && strcmp(node->firstChild->name, name) == 0) {
            file_TreeNode* temp = node->firstChild;
            node->firstChild = node->firstChild->nextSibling;
            delete_subtree(temp);
            break;
        }

        // 处理nextSibling
        file_TreeNode* sibling = node->firstChild;
        while (sibling != NULL && sibling->nextSibling != NULL) {
            if (strcmp(sibling->nextSibling->name, name) == 0) {
                file_TreeNode* temp = sibling->nextSibling;
                sibling->nextSibling = sibling->nextSibling->nextSibling;
                delete_subtree(temp);
                break;
            }
            sibling = sibling->nextSibling;
        }

        if (node->firstChild != NULL) {
            push2(stack, node->firstChild);
        }
        if (node->nextSibling != NULL) {
            push2(stack, node->nextSibling);
        }
    }
    dropStack2(stack);
}

//----------文件信息操作----------

//读取myfile和mydir并执行操作
void read_myfile(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("无法打开文件: %s\n", filename);
        return;
    }
    char* option=(char*)malloc(1024);

    while (fgets(option, 1024, file)) {//不能用sizeof(option) 用1024
        option[strlen(option) - 1] = '\0';
        if (!strcmp(option, "selected files") || !strcmp(option,"selected dirs")) {
            continue;
        } else if (!strcmp(option, "end of files")||!strcmp(option,"end of dirs")) {
            break;
        } else {
        //分割字符串
            char* tmp;
            char* path, * opt;
            unsigned long long time, size;
            tmp = strtok(option, ",");
            path = malloc(strlen(tmp) + 1);
            strcpy(path, tmp);
            int l = strlen(path)-1;
            if (path[l] == '\\')
                path[l] = '\0';
            path[0] = toupper(path[0]);
            path[3] = toupper(path[3]);

            tmp = strtok(NULL, ",");
            opt = malloc(strlen(tmp) + 1);//一个字符的字符串，怕改成字符类型和数据库交互出问题
            strcpy(opt, tmp);
            if (!strcmp(opt, "D")) {
                time = NULL;
                size = NULL;
            }
            else {
                tmp = strtok(NULL, ",");
                time = strtoull(tmp, NULL, 10);

                tmp = strtok(NULL, ",");
                size = strtoull(tmp, NULL, 10);
            }
            manipulate(path, opt, time, size);
            //your action here
            free(path);
            free(opt);
        }
    }
    free(option);
    fclose(file);
}

//读取mystat并第一次统计有关信息并存储
void read_mystat(const char* filename)
{
    int i = 0;
    int l;
    FILE* file = fopen(filename, "r");
    const char* lable = "|/-\\";
    if (file == NULL) {
        printf("无法打开文件: %s\n", filename);
        return;
    }
    char* dir= (char*)malloc(255);
    while(fgets(dir, 255, file))
	{
		dir[strlen(dir) - 1] = '\0';
        l = strlen(dir) - 1;
        if (dir[l] == '\\')
            dir[l] = '\0';
        if(!strcmp(dir,"stat dirs")) {
			continue;
		}
		else if (!strcmp(dir, "end of dirs")) {
			break;
        }
        else  {
            dir[0] = 'C';
            dir[3] = 'W';
            //另存一份path
            char* tmp = (char*)malloc(strlen(dir) + 1);
            strcpy(tmp, dir);
            my_stat[i].path = tmp;
            my_stat[i].size = total_size(dir);
            my_stat[i].file_count = total_files(dir);
            if (earliest_file(dir) != NULL) {
                my_stat[i].earliest = earliest_file(dir)->name;
            }
			else {
				my_stat[i].earliest = "NULL";
			}
            if (latest_file(dir) != NULL) {
                my_stat[i].latest = latest_file(dir)->name;
            }
            else {
                my_stat[i].latest = "NULL";
            }
            printf("正在读取mystats并统计... [%d][%c]\r", i, lable[i % 4]);
            fflush(stdout);
            i++;
        }
	}
    free(dir);
    fclose(file);
}

//输出mystat中保存的目录信息
void print_dirinfo(dir_info mystat[],int id)
{
    if (id == 0) {
        for (int i = 0; i < STAT_COUNT; i++)
        {
            
            printf("目录大小:%llu\n", mystat[i].size);
            printf("文件总数:%llu\n", mystat[i].file_count);
            if (mystat[i].earliest==NULL) {
				printf("earliest:NULL\n");
			}
			else {
				printf("时间最早文件 :  %s\n", mystat[i].earliest);
			}
            if (mystat[i].latest == NULL) {
                printf("latest:NULL\n\n");
            }
            else {
                printf("时间最晚文件 :  %s\n\n", mystat[i].latest);
            }
        }
    }
	else {
        printf("目录 :  %s\n\n", mystat[id - 1].path);
		printf("目录大小:%llu\n", mystat[id-1].size);
		printf("文件总数:%d\n", mystat[id-1].file_count);
		printf("时间最早文件:%s\n", mystat[id-1].earliest);
		printf("时间最晚文件:%s\n\n", mystat[id-1].latest);
	}
}

//重新统计指定目录的信息，对比输出变化并更新
void compare_dirinfo(dir_info mystat[],int i) {
    unsigned long long new_size=total_size(mystat[i].path);
    int new_file_count=total_files(mystat[i].path);
    file_TreeNode* new_earliest_file=earliest_file(mystat[i].path);
    char* new_earliest;
    char* new_latest;
    int is_changed = 0;
    if (new_earliest_file != NULL) {
        new_earliest = earliest_file(mystat[i].path)->name;
    }
	else {
		new_earliest = "NULL";
	}
    file_TreeNode* new_latest_file = latest_file(mystat[i].path);
    if (new_latest_file != NULL) {
		new_latest = latest_file(mystat[i].path)->name;
	}
    else {
		new_latest = "NULL";
    }
    unsigned long long old_size = mystat[i].size;
    int old_file_count = mystat[i].file_count;
    char* old_earliest = mystat[i].earliest;
    char* old_latest = mystat[i].latest;

    if (new_size != mystat[i].size) {
		printf("目录 \"%s\" 的大小已从 %llu 改变为 %llu\n\n", mystat[i].path, mystat[i].size, new_size);
        mystat[i].size = total_size(mystat[i].path);
        getchar();
        is_changed = 1;
	}
    if (new_file_count != mystat[i].file_count) {
        printf("目录 \"%s\" 的文件总数已从 %d 改变为 %d\n\n", mystat[i].path, mystat[i].file_count, new_file_count);
        getchar();
        is_changed = 1;
    }
    if (strcmp(new_earliest, mystat[i].earliest)) {
        printf("目录 \"%s\" 下最早创建的文件已从 \"%s\" 改变为 \"%s\"\n\n", mystat[i].path, mystat[i].earliest, new_earliest);
        getchar();
        is_changed = 1;
    };
    if (strcmp(new_latest, mystat[i].latest)) {
		printf("目录 \"%s\" 下最晚创建的文件已从 \"%s\" 改变为 \"%s\"\n\n", mystat[i].path, mystat[i].latest, new_latest);
        getchar();
        is_changed = 1;
	}
    if(!is_changed) {
		printf("目录 \"%s\" 未发生变化\n\n", mystat[i].path);
	}

    mystat[i].file_count = new_file_count;
    mystat[i].size = new_size;
    mystat[i].earliest = new_earliest;
    mystat[i].latest = new_latest;
}

//辅助函数
char* get_parent_directory(const char* path) {
    char* path_copy = _strdup(path);
    char* last_slash = strrchr(path_copy, '\\');
    if (last_slash != NULL) {
        *last_slash = '\0';
    }
    return path_copy;
}

//执行读取到的操作，封装
void manipulate(char* path, char* opt, unsigned long long time, unsigned long long size)
{
    if (!strcmp(opt, "A")) {
        printf("操作：将文件 \"%s\" 插入对应路径下, 文件大小：%ull, 修改时间： %s \n",path,size,ctime(&time));
        file_TreeNode* new_node = create_node(path, size, time, FALSE);
        char* parent_path = get_parent_directory(path);
        insert_node(parent_path, new_node);
    }
    else if (!strcmp(opt, "D")) {
        printf("操作：删除目录或文件 \"%s\" ，若是目录执行级联删除\n", path);
        delete_node_cascade(path);
    }
    else if (!strcmp(opt, "M")) {
        printf("操作：修改文件 \"%s\" 的大小为 %llu, 修改时间为 %s\n", path, size, ctime(&time));
        file_TreeNode* node= find_node(path);
        if (node == NULL) {
            printf("未找到节点，可能节点已删除\n");
            return;
        }
        else {
            node->lastModified= time;
            node->size = size;
        }
        //modify_node_iterative(path, time, size);
    }
    else {
        printf("myfile中存在无效的操作\n");
        return;
    }
}

//----------统计信息----------
// 根据路径查找对应的节点
file_TreeNode* find_node(const char* directory) {
    Stack2* stack = initStack2();
    push2(stack, root);
    file_TreeNode* foundNode = NULL;

    while (stack->top != -1) {
        file_TreeNode* node = pop2(stack);

        if (strcmp(node->name, directory) == 0) {
            foundNode = node;
        }

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
    return foundNode;
}

//计算指定节点及其子节点的总大小
unsigned long long calculate_total_size(file_TreeNode* node) {
    unsigned long long total = 0;
    Stack2* stack = initStack2();
    push2(stack, node);
    while (stack->top != -1) {
        file_TreeNode* current = pop2(stack);
        if (!current->is_dir) {
            total += current->size;
        }
        file_TreeNode* child = current->firstChild;
        while (child != NULL) {
            push2(stack, child);
            child = child->nextSibling;
        }
    }
    while (stack->top != -1) {
        pop2(stack);
    }
    dropStack2(stack);
    return total;
}

//计算指定节点及其子节点的文件总数
int calculate_total_files(file_TreeNode* node) {
    int count = 0;
    Stack2* stack = initStack2();
    push2(stack, node);
    while (stack->top != -1) {
        file_TreeNode* current = pop2(stack);
        if (!current->is_dir) {
            count++;
        }
        file_TreeNode* child = current->firstChild;
        while (child != NULL) {
            push2(stack, child);
            child = child->nextSibling;
        }
    }
    while (stack->top != -1) {
        pop2(stack);
    }
    dropStack2(stack);
    return count;
}

//找到指定节点及其子节点中最早创建的文件
file_TreeNode* find_earliest_file(file_TreeNode* node) {
    file_TreeNode* earliest = NULL;
    Stack2* stack = initStack2();
    push2(stack, node);
    while (stack->top != -1) {
        file_TreeNode* current = pop2(stack);
        if (!current->is_dir && (earliest == NULL || current->lastModified < earliest->lastModified)) {
            earliest = current;
        }
        file_TreeNode* child = current->firstChild;
        while (child != NULL) {
            push2(stack, child);
            child = child->nextSibling;
        }
    }
    while (stack->top != -1) {
        pop2(stack);
    }
    dropStack2(stack);
    return earliest;
}

//找到指定节点及其子节点中最晚创建的文件
file_TreeNode* find_latest_file(file_TreeNode* node) {
    file_TreeNode* latest = NULL;
    Stack2* stack = initStack2();
    push2(stack, node);
    while (stack->top != -1) {
        file_TreeNode* current = pop2(stack);
        if (!current->is_dir && (latest == NULL || current->lastModified > latest->lastModified)) {
            latest = current;
        }
        file_TreeNode* child = current->firstChild;
        while (child != NULL) {
            push2(stack, child);
            child = child->nextSibling;
        }
    }
    while (stack->top != -1) {
        pop2(stack);
    }
    dropStack2(stack);
    return latest;
}

//统计指定目录下的文件总大小，接口封装
unsigned long long total_size(const char* directory) {
    unsigned long long total = 0;
    if (directory == NULL)
    {
        printf("传入目录为空\n");
        return total;
    }
    file_TreeNode* node = find_node(directory);
    if (node != NULL) {
        total = calculate_total_size(node);
    }
	else {
        printf("%s   目录未找到，请检查其是否已经删除！\n", directory);
	}
    return total;
}

//统计指定目录下的文件总数
int total_files(const char* directory) {
    int count = 0;
    if (directory == NULL)
    {
        printf("传入目录为空\n");
        return count;
    }
    file_TreeNode* node = find_node(directory);
    if (node != NULL) {
        count = calculate_total_files(node);
    }
    else {
        printf("%s   目录未找到，请检查其是否已经删除！\n", directory);
    }
    return count;
}

//找到指定目录下最早创建的文件
file_TreeNode* earliest_file(const char* directory) {
    file_TreeNode* earliest = NULL;
    if (directory == NULL)
    {
        printf("传入路径为空\n");
        return earliest;
    }
    file_TreeNode* node = find_node(directory);
    if (node != NULL) {
        earliest = find_earliest_file(node);
    }
    else {
        printf("%s   目录未找到，请检查其是否已经删除！\n",directory);
    }
    return earliest;
}

// 找到指定目录下最晚创建的文件
file_TreeNode* latest_file(const char* directory) {
    file_TreeNode* latest = NULL;
    if (directory == NULL)
    {
        printf("传入目录为空\n");
        return latest;
    }
    file_TreeNode* node = find_node(directory);
    if (node != NULL) {
        latest = find_latest_file(node);
    }
    else {
        printf("%s   目录未找到，请检查其是否已经删除！\n", directory);
    }
    return latest;
}