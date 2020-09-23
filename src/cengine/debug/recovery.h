#ifndef __CENGINE_DEBUG_RECOVERY_H_
#define __CENGINE_DEBUG_RECOVERY_H_

void recovery_setup();

void recovery_safeprint( const char* format, ... );

void recovery_funclog_push( const char* Name );

#define RECOVERY_FUNCLOG_SIZE 32

#define mark_important_func() recovery_funclog_push(__func__)

#endif
