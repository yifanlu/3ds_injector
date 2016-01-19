#include <3ds.h>

static Handle fsregHandle;
static int fsregRefCount;

Result fsregInit(void)
{
  Result ret = 0;

  if (AtomicPostIncrement(&fsregRefCount)) return 0;

  ret = srvGetServiceHandle(&fsregHandle, "fs:REG");

  if (R_FAILED(ret)) AtomicDecrement(&fsregRefCount);
  return ret;
}

void fsregExit(void)
{
  if (AtomicDecrement(&fsregRefCount)) return;
  svcCloseHandle(fsregHandle);
}
