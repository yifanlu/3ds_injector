#include <3ds.h>
#include "patcher.h"

int patch_code(u64 progid, u8 *code, u32 size)
{
  if (progid == 0x0004003000008F02LL) // USA Menu
  {
    *(u32 *)(code + 0x00101B14) = 0xe3a00001;
    *(u32 *)(code + 0x00101B18) = 0xe12fff1e;
  }
  return 0;
}
