#include <stdio.h>
#include <sqlite3.h> 

int main(int argc, char* argv[])
{
   sqlite3 *db;
   char *zErrMsg = 0;
   int rc;

   rc = sqlite3_open("test.db", &db);

   if( rc ){
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      return 0;
   }else{
      fprintf(stderr, "Opened database successfully\n");
   }
   sqlite3_close(db);
}


// lets make all the accessing and objects its own file and precompile it into an object, see the old burns stuff for info