#include <stdio.h>
#include "sqlite3.h"

int main() {
  printf("%s\n", "Hello World!");
  printf("%s :: Testing whether sqlite3 is compiled...\n", SQLITE_VERSION);
  return 0;
}
