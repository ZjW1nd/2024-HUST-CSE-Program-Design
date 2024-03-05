#define _CRT_SECURE_NO_WARNINGS
#include "simulate.h"
extern file_TreeNode* root;
dir_info my_stat[STAT_COUNT];
//----------Ŀ¼������----------

//����ڵ�
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

//ɾ����ָ���ڵ�Ϊ���ڵ����
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

//����ɾ���ڵ�
void delete_node_cascade(const char* name) {
    Stack2* stack = initStack2();
    push2(stack, root);

    while (stack->top != -1) {
        file_TreeNode* node = pop2(stack);

        // ����firstChild
        if (node->firstChild != NULL && strcmp(node->firstChild->name, name) == 0) {
            file_TreeNode* temp = node->firstChild;
            node->firstChild = node->firstChild->nextSibling;
            delete_subtree(temp);
            break;
        }

        // ����nextSibling
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

//----------�ļ���Ϣ����----------

//��ȡmyfile��mydir��ִ�в���
void read_myfile(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("�޷����ļ�: %s\n", filename);
        return;
    }
    char* option=(char*)malloc(1024);

    while (fgets(option, 1024, file)) {//������sizeof(option) ��1024
        option[strlen(option) - 1] = '\0';
        if (!strcmp(option, "selected files") || !strcmp(option,"selected dirs")) {
            continue;
        } else if (!strcmp(option, "end of files")||!strcmp(option,"end of dirs")) {
            break;
        } else {
        //�ָ��ַ���
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
            opt = malloc(strlen(tmp) + 1);//һ���ַ����ַ������¸ĳ��ַ����ͺ����ݿ⽻��������
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

//��ȡmystat����һ��ͳ���й���Ϣ���洢
void read_mystat(const char* filename)
{
    int i = 0;
    int l;
    FILE* file = fopen(filename, "r");
    const char* lable = "|/-\\";
    if (file == NULL) {
        printf("�޷����ļ�: %s\n", filename);
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
            //���һ��path
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
            printf("���ڶ�ȡmystats��ͳ��... [%d][%c]\r", i, lable[i % 4]);
            fflush(stdout);
            i++;
        }
	}
    free(dir);
    fclose(file);
}

//���mystat�б����Ŀ¼��Ϣ
void print_dirinfo(dir_info mystat[],int id)
{
    if (id == 0) {
        for (int i = 0; i < STAT_COUNT; i++)
        {
            
            printf("Ŀ¼��С:%llu\n", mystat[i].size);
            printf("�ļ�����:%llu\n", mystat[i].file_count);
            if (mystat[i].earliest==NULL) {
				printf("earliest:NULL\n");
			}
			else {
				printf("ʱ�������ļ� :  %s\n", mystat[i].earliest);
			}
            if (mystat[i].latest == NULL) {
                printf("latest:NULL\n\n");
            }
            else {
                printf("ʱ�������ļ� :  %s\n\n", mystat[i].latest);
            }
        }
    }
	else {
        printf("Ŀ¼ :  %s\n\n", mystat[id - 1].path);
		printf("Ŀ¼��С:%llu\n", mystat[id-1].size);
		printf("�ļ�����:%d\n", mystat[id-1].file_count);
		printf("ʱ�������ļ�:%s\n", mystat[id-1].earliest);
		printf("ʱ�������ļ�:%s\n\n", mystat[id-1].latest);
	}
}

//����ͳ��ָ��Ŀ¼����Ϣ���Ա�����仯������
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
		printf("Ŀ¼ \"%s\" �Ĵ�С�Ѵ� %llu �ı�Ϊ %llu\n\n", mystat[i].path, mystat[i].size, new_size);
        mystat[i].size = total_size(mystat[i].path);
        getchar();
        is_changed = 1;
	}
    if (new_file_count != mystat[i].file_count) {
        printf("Ŀ¼ \"%s\" ���ļ������Ѵ� %d �ı�Ϊ %d\n\n", mystat[i].path, mystat[i].file_count, new_file_count);
        getchar();
        is_changed = 1;
    }
    if (strcmp(new_earliest, mystat[i].earliest)) {
        printf("Ŀ¼ \"%s\" �����紴�����ļ��Ѵ� \"%s\" �ı�Ϊ \"%s\"\n\n", mystat[i].path, mystat[i].earliest, new_earliest);
        getchar();
        is_changed = 1;
    };
    if (strcmp(new_latest, mystat[i].latest)) {
		printf("Ŀ¼ \"%s\" �����������ļ��Ѵ� \"%s\" �ı�Ϊ \"%s\"\n\n", mystat[i].path, mystat[i].latest, new_latest);
        getchar();
        is_changed = 1;
	}
    if(!is_changed) {
		printf("Ŀ¼ \"%s\" δ�����仯\n\n", mystat[i].path);
	}

    mystat[i].file_count = new_file_count;
    mystat[i].size = new_size;
    mystat[i].earliest = new_earliest;
    mystat[i].latest = new_latest;
}

//��������
char* get_parent_directory(const char* path) {
    char* path_copy = _strdup(path);
    char* last_slash = strrchr(path_copy, '\\');
    if (last_slash != NULL) {
        *last_slash = '\0';
    }
    return path_copy;
}

//ִ�ж�ȡ���Ĳ�������װ
void manipulate(char* path, char* opt, unsigned long long time, unsigned long long size)
{
    if (!strcmp(opt, "A")) {
        printf("���������ļ� \"%s\" �����Ӧ·����, �ļ���С��%ull, �޸�ʱ�䣺 %s \n",path,size,ctime(&time));
        file_TreeNode* new_node = create_node(path, size, time, FALSE);
        char* parent_path = get_parent_directory(path);
        insert_node(parent_path, new_node);
    }
    else if (!strcmp(opt, "D")) {
        printf("������ɾ��Ŀ¼���ļ� \"%s\" ������Ŀ¼ִ�м���ɾ��\n", path);
        delete_node_cascade(path);
    }
    else if (!strcmp(opt, "M")) {
        printf("�������޸��ļ� \"%s\" �Ĵ�СΪ %llu, �޸�ʱ��Ϊ %s\n", path, size, ctime(&time));
        file_TreeNode* node= find_node(path);
        if (node == NULL) {
            printf("δ�ҵ��ڵ㣬���ܽڵ���ɾ��\n");
            return;
        }
        else {
            node->lastModified= time;
            node->size = size;
        }
        //modify_node_iterative(path, time, size);
    }
    else {
        printf("myfile�д�����Ч�Ĳ���\n");
        return;
    }
}

//----------ͳ����Ϣ----------
// ����·�����Ҷ�Ӧ�Ľڵ�
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

//����ָ���ڵ㼰���ӽڵ���ܴ�С
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

//����ָ���ڵ㼰���ӽڵ���ļ�����
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

//�ҵ�ָ���ڵ㼰���ӽڵ������紴�����ļ�
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

//�ҵ�ָ���ڵ㼰���ӽڵ������������ļ�
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

//ͳ��ָ��Ŀ¼�µ��ļ��ܴ�С���ӿڷ�װ
unsigned long long total_size(const char* directory) {
    unsigned long long total = 0;
    if (directory == NULL)
    {
        printf("����Ŀ¼Ϊ��\n");
        return total;
    }
    file_TreeNode* node = find_node(directory);
    if (node != NULL) {
        total = calculate_total_size(node);
    }
	else {
        printf("%s   Ŀ¼δ�ҵ����������Ƿ��Ѿ�ɾ����\n", directory);
	}
    return total;
}

//ͳ��ָ��Ŀ¼�µ��ļ�����
int total_files(const char* directory) {
    int count = 0;
    if (directory == NULL)
    {
        printf("����Ŀ¼Ϊ��\n");
        return count;
    }
    file_TreeNode* node = find_node(directory);
    if (node != NULL) {
        count = calculate_total_files(node);
    }
    else {
        printf("%s   Ŀ¼δ�ҵ����������Ƿ��Ѿ�ɾ����\n", directory);
    }
    return count;
}

//�ҵ�ָ��Ŀ¼�����紴�����ļ�
file_TreeNode* earliest_file(const char* directory) {
    file_TreeNode* earliest = NULL;
    if (directory == NULL)
    {
        printf("����·��Ϊ��\n");
        return earliest;
    }
    file_TreeNode* node = find_node(directory);
    if (node != NULL) {
        earliest = find_earliest_file(node);
    }
    else {
        printf("%s   Ŀ¼δ�ҵ����������Ƿ��Ѿ�ɾ����\n",directory);
    }
    return earliest;
}

// �ҵ�ָ��Ŀ¼�����������ļ�
file_TreeNode* latest_file(const char* directory) {
    file_TreeNode* latest = NULL;
    if (directory == NULL)
    {
        printf("����Ŀ¼Ϊ��\n");
        return latest;
    }
    file_TreeNode* node = find_node(directory);
    if (node != NULL) {
        latest = find_latest_file(node);
    }
    else {
        printf("%s   Ŀ¼δ�ҵ����������Ƿ��Ѿ�ɾ����\n", directory);
    }
    return latest;
}