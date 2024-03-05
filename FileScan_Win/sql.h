#pragma once
#ifndef _SQL_
#define _SQL_

#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <odbcss.h>
#include <stdio.h>
#include "scan_tree.h"

SQLRETURN init_sql();
void check_error();
void Insert(file_TreeNode*);
void disconnect();

#endif // !_SQL_

