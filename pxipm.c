#include <3ds.h>

static Handle pxipmHandle;
static int pxipmRefCount;

Result pxipmInit(void)
{
  Result ret = 0;

  if (AtomicPostIncrement(&pxipmRefCount)) return 0;

  ret = srvGetServiceHandle(&pxipmHandle, "PxiPM");

  if (R_FAILED(ret)) AtomicDecrement(&pxipmRefCount);
  return ret;
}

void pxipmExit(void)
{
  if (AtomicDecrement(&pxipmRefCount)) return;
  svcCloseHandle(pxipmHandle);
}
