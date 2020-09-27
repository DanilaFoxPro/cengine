#include <cengine/cengine.h>
#include <cengine/debug/recovery.h>

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

struct FunctionEntry {
        const char* name;
        const char* file;
        uint32_t    line;
        uint32_t    count; // How many times in a row the function was called.
};

struct StageMark {
        const char* name;
        const char* file;
        uint32_t    line;
};

/** Function log as a circular array.
 *  So when it grows past maximum length -- it loops back.
 *  Offset keeps track of the current 'first' item.
 */
struct {
        struct FunctionEntry* pointer;
        unsigned int offset;
        unsigned int length;
        
        struct StageMark stage;
} function_log;

uint8_t function_entry_equal( const struct FunctionEntry A, const struct FunctionEntry B )
{
        if( A.name == B.name ) {
                if( A.file != B.file ) {
                        printf( "%s(): Function name pointers equal, but the files are different!\n", __func__ );
                }
                return 1;
        } else {
                return 0;
        }
}

static void funclog_setup(void)
{
        function_log.pointer = malloc( sizeof(struct FunctionEntry)*RECOVERY_FUNCLOG_SIZE );
        function_log.offset  = 0;
        function_log.length  = 0;
        function_log.stage.name = NULL;
}

static void funclog_push( const struct FunctionEntry new )
{
        
        // Reset stage. Because we're entering a new function.
        function_log.stage.name = NULL;
        
        // If previous entry is the same function -- just increment the count.
        if( function_log.length != 0 ) {
                const unsigned int last = (function_log.offset-1) % function_log.length;
                if( function_entry_equal( function_log.pointer[last], new ) ) {
                        function_log.pointer[last].count++;
                        return;
                }
        }
        
        // Insert new function entry.
        memcpy( function_log.pointer+function_log.offset, &new, sizeof(struct FunctionEntry) );
        
        function_log.offset++;
        function_log.offset %= RECOVERY_FUNCLOG_SIZE;
        
        function_log.length++;
        clampui( &function_log.length, 0, RECOVERY_FUNCLOG_SIZE );
}

static void funclog_print(void)
{
        for( unsigned int i = 0; i < function_log.length; i++ )
        {
                const unsigned int position = (i+function_log.offset) % function_log.length;
                const struct FunctionEntry entry = function_log.pointer[position];
                
                if( entry.count == 1 ) {
                        recovery_safeprint( "%s\n", entry.name );
                } else {
                        recovery_safeprint( "%s [x%i]\n", entry.name, entry.count );
                }
                
                recovery_safeprint(
                        "  ('%s', line: '%i')\n",
                        entry.file,
                        entry.line
                );
                
                recovery_safeprint(".\n");
                
        }
        
        if( function_log.stage.name != NULL ) {
                recovery_safeprint(
                        "Failed at stage: '%s'. ('%s', line: %i.)\n",
                        function_log.stage.name,
                        function_log.stage.file,
                        function_log.stage.line
                );
        }
        
}

void handle_signal( int signal )
{
        char* signal_name = NULL;
        
        switch( signal ) {
                case SIGTERM:{
                        signal_name = "termination request";
                        break;
                }
                case SIGSEGV:{
                        signal_name = "invalid memory access";
                        break;
                }
                case SIGINT:{
                        signal_name = "external interrupt";
                        break;
                }
                case SIGILL:{
                        signal_name = "invalid instruction";
                        break;
                }
                case SIGABRT:{
                        signal_name = "abnormal termination";
                        break;
                }
                case SIGFPE:{
                        signal_name = "erroneous arithmetic operation";
                        break;
                }
                default:{
                        signal_name = "unknown signal";
                        break;
                }
        }
        
        recovery_safeprint( "Recieved signal: %s\n", signal_name );
        recovery_safeprint( "Function log:         (older calls first)\n" );
        recovery_safeprint( ".\n" );
        funclog_print();
        
}

void recovery_setup()
{
        signal( SIGTERM, handle_signal );
        signal( SIGSEGV, handle_signal );
        signal( SIGINT,  handle_signal );
        signal( SIGILL,  handle_signal );
        signal( SIGABRT, handle_signal );
        signal( SIGFPE,  handle_signal );
        
        funclog_setup();
}

/**
 * @brief Safe alternative to printf that hopefully works in any conditions.
 * @note Currently only supports printing one line.
 */
void recovery_safeprint(const char* format, ... )
{
        
        // Forward input to 'vsprintf' which formats it,
        // and then print the result using system 'echo'
        // command.
        
        va_list valist;
        va_start( valist, format );
        
        char string[256];
        const char* prefix = "echo ";
        char command[256+5];
        
        vsprintf( string, format, valist );
        
        strcpy( command, prefix );
        strcpy( command+strlen(prefix), string );
        
        system( command );
        
        va_end(valist);
}

/** Push a new function entry to the function log. */
void recovery_funclog_push( const char* name, const char* file, uint32_t line )
{
        struct FunctionEntry new;
        new.name = name;
        new.file = file;
        new.line = line;
        new.count = 1;
        
        funclog_push( new );
}

/**
 * @brief Push a new stage entry to the function log.
 * @note Stage gets reset when a new function is marked.
 */
void recovery_funclog_push_stage( const char* name, const char* file, uint32_t line )
{
        function_log.stage.name = name;
        function_log.stage.file = file;
        function_log.stage.line = line;
}
