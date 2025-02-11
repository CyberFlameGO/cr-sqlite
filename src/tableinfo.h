#ifndef CRSQLITE_TABLEINFO_H
#define CRSQLITE_TABLEINFO_H

#include "sqlite3ext.h"
SQLITE_EXTENSION_INIT3

#include <ctype.h>
#include <stddef.h>

typedef struct crsql_ColumnInfo crsql_ColumnInfo;
struct crsql_ColumnInfo
{
  int cid;
  char *name;
  char *type;
  int notnull;
  int pk;
  char *versionOf;
};

typedef struct crsql_IndexInfo crsql_IndexInfo;
struct crsql_IndexInfo {
  int seq;
  char *name;
  int unique;
  char *origin;
  int partial;
  char **indexedCols;
  int indexedColsLen;
};

typedef struct crsql_TableInfo crsql_TableInfo;
struct crsql_TableInfo {
  // Name of the table. Owned by this struct.
  char *tblName;

  crsql_ColumnInfo *baseCols;
  int baseColsLen;

  crsql_ColumnInfo *pks;
  int pksLen;

  crsql_ColumnInfo *nonPks;
  int nonPksLen;

  crsql_IndexInfo *indexInfo;
  int indexInfoLen;
};

crsql_ColumnInfo *crsql_extractBaseCols(
    crsql_ColumnInfo *colInfos,
    int colInfosLen,
    int *pBaseColsLen);

void crsql_freeColumnInfoContents(crsql_ColumnInfo *columnInfo);

void crsql_freeTableInfo(crsql_TableInfo *tableInfo);

int crsql_getTableInfo(
    sqlite3 *db,
    const char *tblName,
    crsql_TableInfo **pTableInfo,
    char **pErrMsg);

char *crsql_asIdentifierList(crsql_ColumnInfo *in, size_t inlen, char *prefix);

int crsql_getIndexList(
    sqlite3 *db,
    const char *tblName,
    crsql_IndexInfo **pIndexInfos,
    int *pIndexInfosLen,
    char **pErrMsg);

void crsql_freeAllTableInfos(crsql_TableInfo **tableInfos, int len);
crsql_TableInfo *crsql_findTableInfo(crsql_TableInfo **tblInfos, int len, const char * tblName);
char *crsql_quoteConcat(crsql_ColumnInfo * cols, int len);
int crsql_pullAllTableInfos(
    sqlite3 *db,
    crsql_TableInfo ***pzpTableInfos,
    int *rTableInfosLen,
    char **errmsg);

#endif