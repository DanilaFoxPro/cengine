#ifndef __CENGINE_DEBUG_RECOVERY_H_
#define __CENGINE_DEBUG_RECOVERY_H_

#include <stdint.h> // uint32_t

void recovery_setup();

void recovery_safeprint( const char* format, ... );

void recovery_funclog_push(const char* name, const char* file, uint32_t line );
void recovery_funclog_push_stage( const char* name, const char* file, uint32_t line );

#define RECOVERY_FUNCLOG_SIZE 32

/** Mark the function as important, so calls to this function will be included in the recovery log. */
#define mark_important_func() recovery_funclog_push(__func__, __FILE__, __LINE__)
/** Records the supplied string to the recovery log as a stage. Latest recorded stage is displayed. */
#define mark_important_stage(x) recovery_funclog_push_stage(x, __FILE__, __LINE__);

#endif
