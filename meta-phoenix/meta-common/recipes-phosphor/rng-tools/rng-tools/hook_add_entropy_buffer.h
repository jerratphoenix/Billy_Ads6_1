/* */

#ifndef RNGD_HOOK__H
#define RNGD_HOOK__H

#include <sys/types.h>

#define CMD_NOOP          0
#define CMD_CLR_BUFFER    1
#define CMD_FILL_BUFFER   2
#define CMD_SHRINK_BUFFER 3
#define CMD_WHITEN_BUFFER 4


size_t hook_add_entropy_buffer(int command, void *buf, size_t size, void *optional_buf, size_t optional_size);

#endif /* RNGD_HOOK__H */

