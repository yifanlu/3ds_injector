#include <3ds.h>
#include <string.h>
#include "patcher.h"
#include "ifile.h"

#ifndef PATH_MAX
#define PATH_MAX 255
#endif

static int open_file(IFile *file, const char *path, int flags)
{
  FS_Archive archive;
  FS_Path ppath;
  size_t len;

  len = strnlen(path, PATH_MAX);
  archive.id = ARCHIVE_SDMC;
  archive.lowPath.type = PATH_EMPTY;
  archive.lowPath.size = 1;
  archive.lowPath.data = (u8 *)"";
  ppath.type = PATH_ASCII;
  ppath.data = path;
  ppath.size = len+1;
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
