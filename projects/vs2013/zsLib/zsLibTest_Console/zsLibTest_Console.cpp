// zsLibTest_Console.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <zsLibTest\testing.h>

int _tmain(int argc, _TCHAR* argv[])
{
  Testing::runAllTests();
  Testing::output();

  if (0 != Testing::getGlobalFailedVar()) {
    return -1;
  }
  return 0;
}
