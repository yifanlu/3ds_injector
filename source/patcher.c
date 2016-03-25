#include <3ds.h>
#include "patcher.h"
#include "ifile.h"

#ifndef PATH_MAX
#define PATH_MAX 255
#endif

static int open_file(IFile *file, const char *path, int flags)
{
  FS_Archive archive;
  FS_Path ppath;
  u16 wpath[PATH_MAX+1];
  size_t len;
  size_t i;

  len = strnlen(path, PATH_MAX);
  if (len >= PATH_MAX)
  {
    return -1;
  }

  for (i = 0; i < len; i++)
  {
    wpath[i] = path[i];
  }
  wpath[i] = 0;
  archive.id = ARCHIVE_SDMC;
  archive.lowPath.type = PATH_EMPTY;
  archive.lowPath.size = 0;
  ppath.type = PATH_UTF16;
  ppath.data = wpath;
  ppath.size = 2 * (len + 1);
  return IFile_Open(file, archive, ppath, flags);
}

int patch_code(u64 progid, u8 *code, u32 size)
{
  if (progid == 0x0004003000008F02LL) // USA Menu
  {
    *(u32 *)(code + 0x00101B14) = 0xe3a00001;
    *(u32 *)(code + 0x00101B18) = 0xe12fff1e;
  }
  return 0;
}
