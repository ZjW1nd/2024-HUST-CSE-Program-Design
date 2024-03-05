#define _CRT_SECURE_NO_WARNINGS
/*
�ó���ʵ�ֶ�Windows�ļ�ϵͳ���м򵥵Ĳ���������ɨ����̻�ȡ�ļ���Ϣ��
ͨ��sql��佫�ļ���Ϣ���浽���ݿ⣬�ڴ���ģ��Ŀ¼�������ݿ���ģ���ļ����������Ա仯���ȡ�
����ʵ�֣�SQL���֣�mystat�ṹ������������ʾ����
���룬·���������޸�
*/
#include "scan_tree.h"
#include "stack.h"
#include "sql.h"
#include "simulate.h"
#define MAX_PATH_LENGTH 255
file_TreeNode* root;
extern dir_info my_stat[STAT_COUNT];
int main() {
//˳���ļ���Ϣ�����������ͳ����Ϣ���ļ������ݿ⣬��mystatͳ����Ϣ������
// ��myfile����������ͳ������Ŀ¼��Ϣ��id�Աȣ���mydirһ�������ظ�
	//char* root_dir = "";
	//char* myfile = "";
	//char* mystat = "";
	//char* mydir = "";
	//��������Ŀ¼��ʱ��ǵ��޸�STAT_COUNT�ʹ�Сдת������
	char* root_dir = "C:\\Windows";
	char* myfile="C:\\Users\\Desktop\\FileScan_win\\myfile.txt";
	char* mystat="C:\\Users\\Desktop\\FileScan_win\\mystat.txt";
	char* mydir="C:\\Users\\Desktop\\FileScan_win\\mydir.txt";
	/*1. �ļ���Ϣ�����ڴ�Ŀ¼����ͳ����ȵ���Ϣ*/
	printf("---1. �ļ���Ϣ�����ڴ�Ŀ¼����ͳ����ȵ���Ϣ---\n");
	root=scan_into_tree(root_dir);
	//printTree(root); 

	/*2. Ŀ¼��ת�����ݿ���,ԭ�������ݿ��н���ͳ����������,SQL�����Ǻõ�*/
	printf("---2. Ŀ¼��ת�����ݿ�---\n");
	SQLRETURN retcode = init_sql();
	if (!(retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)) {
		printf("�������ݿ�ʧ��\n");
		check_error();
		return 1;
	}
	//Insert(root);
	//disconnect();
	/*3. ��mystat��ͳ��������Ŀ¼�µ���Ϣ��Ȼ���myfile���в���������仯*/
	printf("---3. ��ȡmystat��ͳ��������Ŀ¼�µ���Ϣ��Ȼ���ȡmyfile��mydir���в���������仯---\n");
	read_mystat(mystat);
	read_myfile(myfile);
	//print_dirinfo(my_stat, 0); //���mystat������Ŀ¼��ͳ����Ϣ
	printf("��һ��ͳ�Ʊ仯, ���س�������\n\n");
	getchar();
	for (int i = 0; i < STAT_COUNT; i++)
	{
		compare_dirinfo(my_stat, i);
	}

	/*4. ��mydir������仯*/	
	read_myfile(mydir);
	printf("�ڶ���ͳ�Ʊ仯�����س�����\n\n");
	getchar();
	for (int i = 0; i < STAT_COUNT; i++)
	{
		compare_dirinfo(my_stat, i);
	}
	printf("������������1��ӡȫ��Ŀ¼�����ݣ���2��ӡ��ǰmystat����Ŀ¼��ͳ����Ϣ������ѡ���˳�\n");
	while (1)
	{
		char tmp = getchar();
		if (tmp == '1') {
			printTree(root);
		}
		else if (tmp == '2') {
			print_dirinfo(my_stat, 0);
		}
		else {
			break;
		}
	}

	//total_size();
	return 0;
}