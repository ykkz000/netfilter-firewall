#include <generated/uapi/linux/version.h>
#include <linux/uaccess.h>
#include "uac.h"

static mm_segment_t old_fs;

void FS_UACCESS_BEGIN()
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 0, 0)
    old_fs = get_fs();
    set_fs(get_ds());
#elif LINUX_VERSION_CODE < KERNEL_VERSION(5, 10, 0)
    old_fs = get_fs();
    set_fs(KERNEL_DS);
#else
    old_fs = force_uaccess_begin();
#endif
}

void FS_UACCESS_END()
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 10, 0)
    set_fs(old_fs);
#else
    force_uaccess_end(old_fs);
#endif
}