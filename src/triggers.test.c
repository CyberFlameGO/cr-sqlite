#include "crsqlite.h"
#include "tableinfo.h"
#include "triggers.h"
#include "util.h"
#include "consts.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// This would be more testable if we could test
// query construction rather than actual table creation.
// testing actual table creation requires views and base crr to
// be in place.
static void testCreateTriggers()
{
  printf("CreateTriggers\n");

  sqlite3 *db = 0;
  crsql_TableInfo *tableInfo;
  char *errMsg = 0;
  int rc = sqlite3_open(":memory:", &db);

  // TODO enumerate various table types
  rc = sqlite3_exec(
      db,
      "CREATE TABLE \"foo\" (\"a\" PRIMARY KEY, \"b\", \"c\")",
      0,
      0,
      &errMsg);
  rc = crsql_getTableInfo(db, "foo", &tableInfo, &errMsg);

  if (rc == SQLITE_OK)
  {
    rc = crsql_createInsertTrigger(db, tableInfo, &errMsg);
  }
  if (rc == SQLITE_OK)
  {
    rc = crsql_createUpdateTrigger(db, tableInfo, &errMsg);
  }
  if (rc == SQLITE_OK)
  {
    rc = crsql_createDeleteTrigger(db, tableInfo, &errMsg);
  }

  crsql_freeTableInfo(tableInfo);
  if (rc != SQLITE_OK)
  {
    sqlite3_close(db);
    printf("err: %s | rc: %d\n", errMsg, rc);
    sqlite3_free(errMsg);
    assert(0);
  }

  sqlite3_free(errMsg);
  sqlite3_close(db);

  printf("\t\e[0;32mSuccess\e[0m\n");
}

static void testDeleteTriggerQuery()
{
  printf("DeleteTriggerQuery\n");
  sqlite3 *db = 0;
  crsql_TableInfo *tableInfo;
  char *errMsg = 0;
  int rc = sqlite3_open(":memory:", &db);

  rc += sqlite3_exec(
      db,
      "CREATE TABLE \"foo\" (\"a\" PRIMARY KEY, \"b\", \"c\")",
      0,
      0,
      &errMsg);
  rc += crsql_getTableInfo(db, "foo", &tableInfo, &errMsg);
  rc += sqlite3_exec(
      db,
      "DROP TABLE foo",
      0,
      0,
      &errMsg);

  char *query = crsql_deleteTriggerQuery(tableInfo);
  assert(strcmp("CREATE TRIGGER \"foo__crsql_dtrig\"      AFTER DELETE ON \"foo\"    BEGIN      INSERT OR REPLACE INTO \"foo__crsql_clock\" (        \"a\",        __crsql_col_num,        __crsql_version,        __crsql_site_id      ) SELECT         OLD.\"a\",        -1,        crsql_nextdbversion(),        0      WHERE crsql_internal_sync_bit() = 0;    END;", query) == 0);

  sqlite3_close(db);
  sqlite3_free(query);
  assert(rc == SQLITE_OK);

  printf("\t\e[0;32mSuccess\e[0m\n");
}

void crsqlTriggersTestSuite()
{
  printf("\e[47m\e[1;30mSuite: crsqlTriggers\e[0m\n");

  testDeleteTriggerQuery();
  testCreateTriggers();
  // testInsertTriggers();
  // testTriggerSyncBitInteraction();
}