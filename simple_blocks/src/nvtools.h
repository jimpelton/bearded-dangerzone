#ifndef subvol_nvtools_h__
#define subvol_nvtools_h__


#ifdef USE_NV_TOOLS
#include <nvToolsExt.h>

const uint32_t nvtools_colors[] = { 0x0000ff00, 0x000000ff, 0x00ffff00, 0x00ff00ff, 0x0000ffff, 0x00ff0000, 0x00ffffff };
const int nvtools_num_colors = sizeof(nvtools_colors) / sizeof(uint32_t);

#define NVTOOLS_PUSH_RANGE(name,cid) { \
    int color_id = cid; \
    color_id = color_id % nvtools_num_colors;\
    nvtxEventAttributes_t eventAttrib = {0}; \
    eventAttrib.version = NVTX_VERSION; \
    eventAttrib.size = NVTX_EVENT_ATTRIB_STRUCT_SIZE; \
    eventAttrib.colorType = NVTX_COLOR_ARGB; \
    eventAttrib.color = nvtools_colors[color_id]; \
    eventAttrib.messageType = NVTX_MESSAGE_TYPE_ASCII; \
    eventAttrib.message.ascii = name; \
    nvtxRangePushEx(&eventAttrib); \
}
#define NVTOOLS_POP_RANGE nvtxRangePop();

#else

#define NVTOOLS_PUSH_RANGE(name,cid)
#define NVTOOLS_POP_RANGE

#endif// USE_NV_TOOLS

#endif // ! USE_NV_TOOLS
