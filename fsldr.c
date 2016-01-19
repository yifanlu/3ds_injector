#include <3ds.h>

#define SDK_VERSION 0xA0000C8

static Handle fsldrHandle;
static int fsldrRefCount;

Result FSLDR_InitializeWithSdkVersion(Handle session, u32 version)
{
  u32 *cmdbuf = getThreadCommandBuffer();

  cmdbuf[0] = IPC_MakeHeader(0x861,1,2); // 0x8610042
  cmdbuf[1] = version;
  cmdbuf[2] = 32;

  Result ret = 0;
  if(R_FAILED(ret = svcSendSyncRequest(session))) return ret;

  return cmdbuf[1];
}

Result FSLDR_SetPriority(u32 priority)
{
  u32 *cmdbuf = getThreadCommandBuffer();

  cmdbuf[0] = IPC_MakeHeader(0x862,1,0); // 0x8620040
  cmdbuf[1] = priority;

  Result ret = 0;
  if(R_FAILED(ret = svcSendSyncRequest(fsldrHandle))) return ret;

  return cmdbuf[1];
}

Result fsldrInit(void)
{
  Result ret = 0;

  if (AtomicPostIncrement(&fsldrRefCount)) return 0;

  ret = srvGetServiceHandle(&fsldrHandle, "fs:LDR");
  if (R_SUCCEEDED(ret))
  {
    ret = FSLDR_InitializeWithSdkVersion(fsldrHandle, SDK_VERSION);
    if (R_SUCCEEDED(ret)) ret = FSLDR_SetPriority(0);
    if (R_FAILED(ret)) svcCloseHandle(fsldrHandle);
  }

  if (R_FAILED(ret)) AtomicDecrement(&fsldrRefCount);
  return ret;
}

void fsldrExit(void)
{
  if (AtomicDecrement(&fsldrRefCount)) return;
  svcCloseHandle(fsldrHandle);
}
