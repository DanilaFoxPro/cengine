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
};

struct FunctionEntry last_function;

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
        }
        
        recovery_safeprint( "Recieved signal: %s\n", signal_name );
        recovery_safeprint(
                "Last called function: %s (file: '%s', line: %i)\n",
                last_function.name,
                last_function.file,
                last_function.line
        );
        
}

void recovery_setup()
{
        signal( SIGTERM, handle_signal );
        signal( SIGSEGV, handle_signal );
        signal( SIGINT,  handle_signal );
        signal( SIGILL,  handle_signal );
        signal( SIGABRT, handle_signal );
        signal( SIGFPE,  handle_signal );
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
        char* command = malloc( 256+strlen(prefix) );
        
        vsprintf( string, format, valist );
        
        strcpy( command, prefix );
        strcpy( command+strlen(prefix), string );
        
        system( command );
        
        va_end(valist);
}

void recovery_funclog_push(const char* name, const char* file, uint32_t line )
{
        // TODO: Proper function log.
        last_function.name = name;
        last_function.file = file;
        last_function.line = line;
}


