#include <3ds.h>
#include <string.h>
#include "patcher.h"
#include "ifile.h"

#ifndef PATH_MAX
#define PATH_MAX 255
#endif

static char secureinfo[0x111] = {0};

static int file_open(IFile *file, FS_ArchiveID id, const char *path, int flags)
{
  FS_Archive archive;
  FS_Path ppath;
  size_t len;

  len = strnlen(path, PATH_MAX);
  archive.id = id;
  archive.lowPath.type = PATH_EMPTY;
  archive.lowPath.size = 1;
  archive.lowPath.data = (u8 *)"";
  ppath.type = PATH_ASCII;
  ppath.data = path;
  ppath.size = len+1;
  return IFile_Open(file, archive, ppath, flags);
}

static int patch_secureinfo()
{
  IFile file;
  Result ret;
  u64 total;

  if (secureinfo[0] == 0xFF)
  {
    return 0;
  }
  ret = file_open(&file, ARCHIVE_SDMC, "/SecureInfo_A", FS_OPEN_READ);
  if (R_SUCCEEDED(ret))
  {
    ret = IFile_Read(&file, &total, secureinfo, sizeof(secureinfo));
    IFile_Close(&file);
    if (R_SUCCEEDED(ret) && total == sizeof(secureinfo))
    {
      ret = file_open(&file, ARCHIVE_NAND_RW, "/sys/SecureInfo_C", FS_OPEN_WRITE | FS_OPEN_CREATE);
      if (R_SUCCEEDED(ret))
      {
        ret = IFile_Write(&file, &total, secureinfo, sizeof(secureinfo), FS_WRITE_FLUSH);
        IFile_Close(&file);
      }
      secureinfo[0] = 0xFF; // we repurpose this byte as status
    }
  }
  else // get file from NAND
  {
    ret = file_open(&file, ARCHIVE_NAND_RW, "/sys/SecureInfo_C", FS_OPEN_READ);
    if (R_SUCCEEDED(ret))
    {
      ret = IFile_Read(&file, &total, secureinfo, sizeof(secureinfo));
      IFile_Close(&file);
      if (R_SUCCEEDED(ret) && total == sizeof(secureinfo))
      {
        secureinfo[0] = 0xFF;
      }
    }
  }
  return ret;
}

int patch_code(u64 progid, u8 *code, u32 size)
{
  if (progid == 0x0004003000008F02LL) // USA Menu
  {
    *(u32 *)(code + 0x00101B14) = 0xe3a00001; // region free
    *(u32 *)(code + 0x00101B18) = 0xe12fff1e;
  }
  else if (progid == 0x0004013000002C02LL) // NIM
  {
    static const char country_patch[] = 
    {
      0x06, 0x9A, 0x03, 0x20, 
      0x90, 0x47, 0x55, 0x21, 
      0x01, 0x70, 0x53, 0x21, 
      0x41, 0x70, 0x00, 0x21, 
      0x81, 0x70, 0x60, 0x61, 
      0x00, 0x20
    };
    const char *country;
    *(u16 *)(code + 0x0000EA00) = 0xa0e3; // stop updates
    *(u32 *)(code + 0x0000DD28) = 0x60082000; // eshop update
    *(u16 *)(code + 0x0000DD2C) = 0x4770;
    if (R_SUCCEEDED(patch_secureinfo()))
    {
      switch (secureinfo[0x100])
      {
        case 1: country = "US"; break;
        case 2: country = "GB"; break; // sorry rest-of-Europe, you have to change this
        case 3: country = "AU"; break;
        case 4: country = "CN"; break;
        case 5: country = "KR"; break;
        case 6: country = "TW"; break;
        default: case 0: country = "JP"; break;
      }
      // patch XML response Country
      memcpy(code + 0x000314F8, country_patch, sizeof(country_patch));
      *(code + 0x000314F8 + 6) = country[0];
      *(code + 0x000314F8 + 10) = country[1];
    }
  }
  else if (progid == 0x2004013000008002LL) // NS
  {
    *(u32 *)(code + 0x00102ACC) = 0xD8E1180C;
    *(u32 *)(code + 0x001894F4) = 0xD8E1180C;
  }
  else if (progid == 0x0004013000001702LL) // CFG
  {
    *(u16 *)(code + 0x000094A0) = 0x2600; // SecureInfo_A sig check
    if (R_SUCCEEDED(patch_secureinfo()))
    {
      *(u16 *)(code + 0x0000C06C) = 0x43; // use SecureInfo_C
      *(u16 *)(code + 0x0000C09E) = 0x43; // use SecureInfo_C
    }
  }
  return 0;
}
