
#include "testing.h"

#include <zsLib/helpers.h>

int main (int argc, char * const argv[]) {
  // insert code here...
  Testing::output();

  if (0 != Testing::getGlobalFailedVar()) {
    return -1;
  }
  return 0;
}
