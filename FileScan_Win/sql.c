#define _CRT_SECURE_NO_WARNINGS
#include "sql.h"
#include "scan_tree.h"
#include "stack.h"
//��ܣ�AllocHandle->Connect->ExecDirect->Fetch->GetData->Disconnect->FreeHandle
SQLHENV henv;//�������
SQLHDBC hdbc;//���Ӿ��
SQLHSTMT hstmt;//�����
SQLRETURN retcode;//���ش���
SQLRETURN init_sql()
{
	retcode = SQLAllocHandle(SQL_HANDLE_ENV, NULL, &henv);//���뻷�����
	retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);
	retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);//�������ݿ����Ӿ��
	retcode = SQLConnect(hdbc, (SQLCHAR*)"sqlserver", SQL_NTS, (SQLCHAR*)"your_username", SQL_NTS, (SQLCHAR*)"your_password", SQL_NTS);
	if (!(retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)) {
		printf("�������ݿ�ʧ��!\n");
		printf("���ش��룺%hd", retcode);
		return retcode;
	}
	else printf("�������ݿ�ɹ�!\n");
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);//���������
	SQLCHAR sql[] = "use windows_dir";
	retcode = SQLExecDirect(hstmt, sql, SQL_NTS);
	return retcode;
}

void check_error()
{
	SQLCHAR SqlState[6], Msg[SQL_MAX_MESSAGE_LENGTH];
	SQLINTEGER NativeError;
	SQLSMALLINT MsgLen;
	while (SQLError(NULL, NULL, hstmt, SqlState, &NativeError, Msg, sizeof(Msg), &MsgLen) != SQL_NO_DATA) {
		printf("SQLState: %s\n", SqlState);
		printf("Native Error: %ld\n", (long)NativeError);
		printf("Message: %s\n", Msg);
	}
}

void Insert(file_TreeNode* root)
{
	if (root == NULL) {
		return;
	}
	Stack2* stack2 = initStack2();
	push2(stack2, root);
	const SQLCHAR fmt_file[] = "INSERT INTO [file](DirectoryId, FileName, FileSize, FileTime) VALUES (\'%d\',\'%s\', \'%ld\', \'%lld\'); ";
	const SQLCHAR fmt_dir[] = "INSERT INTO Directory(Path) VALUES (\'%s\'); ";
	SQLCHAR sql[400];
	int DirectoryID = 0;
	long long i = 1;
	const char* lable = "|/-\\";
	while (stack2->top != -1) {
		file_TreeNode* node = (file_TreeNode*)pop2(stack2);
		if (node->is_dir) {
			sprintf(sql, fmt_dir, node->name);
			DirectoryID++;
		}
		else
			sprintf(sql, fmt_file, DirectoryID, node->name, node->size, node->lastModified);
		retcode = SQLExecDirect(hstmt, sql, SQL_NTS);
		printf("���ڲ�������... �Ѳ���:[%lld][%c]\r", i, lable[i % 4]);
		fflush(stdout);
		i++;
		if (retcode == SQL_ERROR) {
			check_error();
		}
		if (!(retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)) {
			printf("��������ʧ��!\n");
			printf("���ش��룺%hd", retcode);
			return;
		}

		if (node->nextSibling != NULL) {
			push2(stack2, node->nextSibling);
		}
		if (node->firstChild != NULL) {
			push2(stack2, node->firstChild);
		}
	}
	printf("�������ݳɹ�!\n");
	dropStack2(stack2);
}

void disconnect()
{
	char* clear1 = "dbcc checkident('Directory', reseed, 0);";
	char* clear2 = "dbcc checkident('File', reseed, 0);";
	retcode = SQLExecDirect(hstmt, clear1, SQL_NTS);
	if (retcode == SQL_ERROR) {
		check_error();
	}
	retcode = SQLExecDirect(hstmt, clear2, SQL_NTS);
	if (retcode == SQL_ERROR) {
		check_error();
	}
	SQLDisconnect(hdbc);//�Ͽ����ݿ�����
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);//�ͷ������
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);//�ͷ����Ӿ��
	SQLFreeHandle(SQL_HANDLE_ENV, henv);//�ͷŻ������
}
