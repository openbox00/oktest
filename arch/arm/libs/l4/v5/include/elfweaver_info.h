#ifndef __L4__ARM__V5__ELFWEAVER_INFO_H__
#define __L4__ARM__V5__ELFWEAVER_INFO_H__

#define L4_Info_Uncached        {0xffff, 0x0}
#define L4_Info_Cached          {0x0, 0xf}
#define L4_Info_IOMemory        {0x0, 0xf}
#define L4_Info_IOMemoryShared  {0x0, 0xf}
#define L4_Info_WriteThrough    {0xffff, 0x2}
#define L4_Info_WriteBack       {0xffff, 0x3}
#define L4_Info_Shared          {0x0, 0xf}
#define L4_Info_NonShared       {0x0, 0xf}
#define L4_Info_Custom          {0x0, 0xf}
#define L4_Info_Strong          {0x0, 0xf}
#define L4_Info_Buffered        {0xffff, 0x1}

#endif /* !__L4_ARM__V5__ELFWEAVER_INFO_H__ */
