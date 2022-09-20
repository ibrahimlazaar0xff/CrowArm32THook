#pragma once
#include <cstdio>
#include <cstdint>  
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>
#include <signal.h>
#include <sys/mman.h>
#include <asm/ptrace.h>
#include <sys/wait.h>
#include <sys/ptrace.h>

namespace crow
{
    
    // Crow Static Types
    typedef uint64_t DWORD;
    typedef uint32_t WORD;
    typedef uint16_t HWORD;
    typedef uint8_t BYTE;

    typedef void* VOIDPTR;
    typedef int*  INTPTR;

    typedef int I;

    typedef uint64_t U64;
    typedef uint32_t U32;
    typedef uint16_t U16;
    typedef uint8_t  U8;

    typedef int64_t Sint64;
    typedef int32_t Sint32;
    typedef int16_t Sint16;
    typedef int8_t  Sint8;

    
}