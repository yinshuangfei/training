#ifndef __DEBUG_H__
#define __DEBUG_H__

// 使用debug模式则设置为1，否则设置为0
#define __DEBUG__       1   

#if (0 < __DEBUG__)
    #define debug_info_fmt(fmt)  "[ysffs_info] func=%s line=%d \t"fmt
    #define debug_warn_fmt(fmt)  "[ysffs_warn] func=%s line=%d \t"fmt
    #define debug_info(fmt, ...)     printk(KERN_INFO debug_info_fmt(fmt),__func__,__LINE__,##__VA_ARGS__)
    #define debug_warn(fmt, ...)     printk(KERN_ALERT debug_warn_fmt(fmt),__func__,__LINE__,##__VA_ARGS__)
#else  
    #define debug_info(format,...)  
    #define debug_warn(format,...)  
#endif /*#define __DEBUG__*/

#endif /*#define __DEBUG_H__*/
