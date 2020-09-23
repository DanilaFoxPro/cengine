#ifndef __CENGINE_DEBUG_RECOVERY_H_
#define __CENGINE_DEBUG_RECOVERY_H_

#include <stdint.h> // uint32_t

void recovery_setup();

void recovery_safeprint( const char* format, ... );

void recovery_funclog_push(const char* name, const char* file, uint32_t line );

#define RECOVERY_FUNCLOG_SIZE 32

#define mark_important_func() recovery_funclog_push(__func__, __FILE__, __LINE__)

#endif
