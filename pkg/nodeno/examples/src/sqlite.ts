// @ts-ignore
import sqlite3 from "sqlite3";
// @ts-ignore
import { open } from "sqlite";
import { resolve } from "import-meta-resolve";
const modulePath = new URL(await resolve("@vlcn.io/crsqlite", import.meta.url))
  .pathname;

// open the database
const db = await open({
  filename: ":memory:",
  driver: sqlite3.Database,
});

let version = -1;
await db.loadExtension(modulePath);
await db.run("CREATE TABLE foo (a primary key, b)");
await db.run("SELECT crsql_as_crr('foo')");
await db.run("INSERT INTO foo VALUES (1, 2)");
version = (await db.get("select crsql_dbversion()"))!;
console.log(version);
const changes = await db.run("SELECT * FROM crsql_changes WHERE version > 0");

await db.run("select crsql_finalize()");
db.close();
