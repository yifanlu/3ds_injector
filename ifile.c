#include <3ds.h>

typedef struct
{
  Handle handle;
  u64 pos;
  u64 size;
} IFile;

Result IFile_Open(IFile *file, FS_Archive archive, FS_Path path, u32 flags)
{
  Result res;

  res = FSLDR_OpenFileDirectly(&file->handle, archive, path, flags, 0);
  file->pos = 0;
  file->size = 0;
  return res;
}

Result IFile_Close(IFile *file)
{
  return FSFILE_Close(file->handle);
}

Result IFile_GetSize(IFile *file, u64 *size)
{
  Result res;

  res = FSFILE_GetSize(file->handle, size);
  file->size = *size;
  return res;
}

Result IFile_Read(IFile *file, u64 *total, void *buffer, u32 len)
{
  u32 read;
  u32 left;
  char *buf;
  u64 cur;
  Result res;

  if (len == 0)
  {
    *total = 0;
    return 0;
  }

  while (1)
  {
    res = FSFILE_Read(file->handle, &read, file->pos, buffer, left);
    if (R_FAILED(res))
    {
      break;
    }

    cur += read;
    file->pos += read;
    if (read == left)
    {
      break;
    }
    buffer += read;
    left -= read;
  }

  *total = cur;
  return res;
}
