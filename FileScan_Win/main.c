#define _CRT_SECURE_NO_WARNINGS
/*
该程序实现对Windows文件系统进行简单的操作，包括扫描磁盘获取文件信息，
通过sql语句将文件信息保存到数据库，内存中模拟目录树，数据库中模拟文件操作和属性变化检查等。
创新实现：SQL部分，mystat结构数组索引，显示进度
密码，路径请自行修改
*/
#include "scan_tree.h"
#include "stack.h"
#include "sql.h"
#include "simulate.h"
#define MAX_PATH_LENGTH 255
file_TreeNode* root;
extern dir_info my_stat[STAT_COUNT];
int main() {
//顺序：文件信息存树，输出总统计信息，文件存数据库，读mystat统计信息并保存
// 读myfile三条操作，统计三个目录信息用id对比，读mydir一条操作重复
	//char* root_dir = "";
	//char* myfile = "";
	//char* mystat = "";
	//char* mydir = "";
	//更换测试目录的时候记得修改STAT_COUNT和大小写转换部分
	char* root_dir = "C:\\Windows";
	char* myfile="C:\\Users\\Desktop\\FileScan_win\\myfile.txt";
	char* mystat="C:\\Users\\Desktop\\FileScan_win\\mystat.txt";
	char* mydir="C:\\Users\\Desktop\\FileScan_win\\mydir.txt";
	/*1. 文件信息存入内存目录树，统计深度等信息*/
	printf("---1. 文件信息存入内存目录树，统计深度等信息---\n");
	root=scan_into_tree(root_dir);
	//printTree(root); 

	/*2. 目录树转存数据库中,原定在数据库中进行统计最后放弃了,SQL部分是好的*/
	printf("---2. 目录树转存数据库---\n");
	SQLRETURN retcode = init_sql();
	if (!(retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)) {
		printf("连接数据库失败\n");
		check_error();
		return 1;
	}
	//Insert(root);
	//disconnect();
	/*3. 读mystat，统计其中子目录下的信息。然后读myfile进行操作，输出变化*/
	printf("---3. 读取mystat，统计其中子目录下的信息，然后读取myfile和mydir进行操作并输出变化---\n");
	read_mystat(mystat);
	read_myfile(myfile);
	//print_dirinfo(my_stat, 0); //输出mystat中所有目录的统计信息
	printf("第一次统计变化, 按回车继续：\n\n");
	getchar();
	for (int i = 0; i < STAT_COUNT; i++)
	{
		compare_dirinfo(my_stat, i);
	}

	/*4. 读mydir，输出变化*/	
	read_myfile(mydir);
	printf("第二次统计变化，按回车继续\n\n");
	getchar();
	for (int i = 0; i < STAT_COUNT; i++)
	{
		compare_dirinfo(my_stat, i);
	}
	printf("操作结束，按1打印全部目录树内容，按2打印当前mystat中子目录的统计信息，其他选项退出\n");
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