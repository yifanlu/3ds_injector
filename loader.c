#include <stdlib.h>
#include <3ds.h>

#define MAX_SESSIONS 1

static Handle g_handles[MAX_SESSIONS+2];
static int g_active_handles;

static void handle_commands(void)
{
  u32* cmdbuf;
  u16 cmdid;

  cmdbuf = getThreadCommandBuffer();
  cmdid = cmdbuf[0] >> 16;
  switch (cmdid)
  {
    case 1: // LoadProcess
    {
      break;
    }
    case 2: // RegisterProgram
    {
      break;
    }
    case 3: // UnregisterProgram
    {
      break;
    }
    case 4: // GetProgramInfo
    {
      break;
    }
    default: // error
    {
      cmdbuf[0] = 0x40;
      cmdbuf[1] = 0xD900182F;
    }
  }
}

int main(int argc, const char *argv[])
{
  Result ret;
  Handle handle;
  Handle *srv_handle;
  Handle *notification_handle;
  s32 index;
  int i;

  ret = 0;
  srvInit();
  fsldrInit();
  fsregInit();
  pxipmInit();

  srv_handle = &g_handles[1];
  notification_handle = &g_handles[0];

  if (R_FAILED(srvRegisterService(srv_handle, "Loader", 1)))
  {
    svcBreak(USERBREAK_ASSERT);
  }

  if (R_FAILED(srvEnableNotification(notification_handle)))
  {
    svcBreak(USERBREAK_ASSERT);
  }

  g_active_handles = 2;
  index = 1;

  reply_target = 0;
  do
  {
    ret = svcReplyAndReceive(&index, g_handles, g_active_handles, reply_target);
    reply_target = 0;

    // check if any handle has been closed
    if (ret == 0xC920181A)
    {
      if (index == -1)
      {
        for (i = 2; i < MAX_SESSIONS+2; i++)
        {
          if (g_handles[i] == reply_target)
          {
            index = i;
            break;
          }
        }
      }
      svcCloseHandle(reply_target);
      g_handles[index] = g_handles[g_active_handles-1];
      g_active_handles--;
    }
    else if (R_FAILED(ret))
    {
      svcBreak(USERBREAK_ASSERT);
    }

    // process responses
    switch (index)
    {
      case 0: // todo: notification
      {
        break;
      }
      case 1: // new session
      {
        if (R_FAILED(svcAcceptSession(&handle, srv_handle)))
        {
          svcBreak(USERBREAK_ASSERT);
        }
        if (g_active_handles < MAX_SESSIONS+2)
        {
          g_handles[g_active_handles] = handle;
          g_active_handles++;
        }
        else
        {
          svcCloseHandle(handle);
        }
        break;
      }
      default: // session
      {
        reply_target = g_handles[index];
        handle_commands();
        break;
      }
    }
  } while (1);

  pxipmExit();
  fsregExit();
  fsldrExit();
  srvExit();
  return 0;
}