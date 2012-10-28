//
//  StdCLib.h
//  pefdump
//
//  Created by Félix on 2012-10-28.
//  Copyright (c) 2012 Félix. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "PPCLibrary.h"

@interface StdCLib : NSObject <PPCLibrary>
{
	PPCAllocator* allocator;
	struct StdCLibGlobals* globals;
}

-(id)initWithAllocator:(PPCAllocator *)allocator;
-(NSString*)libraryName;
-(PPCLibraryFunction)resolve:(NSString *)functionName;

#pragma mark -
#pragma mark Data Symbols
-(void*)StdCLib___C_phase;
-(void*)StdCLib___loc;
-(void*)StdCLib___NubAt3;
-(void*)StdCLib___p_CType;
-(void*)StdCLib___SigEnv;
-(void*)StdCLib___target_for_exit;
-(void*)StdCLib___yd;
-(void*)StdCLib__CategoryLoc;
-(void*)StdCLib__DBL_EPSILON;
-(void*)StdCLib__DBL_MAX;
-(void*)StdCLib__DBL_MIN;
-(void*)StdCLib__exit_status;
-(void*)StdCLib__FLT_EPSILON;
-(void*)StdCLib__FLT_MAX;
-(void*)StdCLib__FLT_MIN;
-(void*)StdCLib__IntEnv;
-(void*)StdCLib__iob;
-(void*)StdCLib__lastbuf;
-(void*)StdCLib__LDBL_EPSILON;
-(void*)StdCLib__LDBL_MAX;
-(void*)StdCLib__LDBL_MIN;
-(void*)StdCLib__PublicTimeInfo;
-(void*)StdCLib__StdDevs;
-(void*)StdCLib_errno;
-(void*)StdCLib_MacOSErr;
-(void*)StdCLib_MoneyData;
-(void*)StdCLib_NoMoreDebugStr;
-(void*)StdCLib_NumericData;
-(void*)StdCLib_StandAlone;
-(void*)StdCLib_TimeData;

#pragma mark -
#pragma mark Code Symbols
-(void)StdCLib___abort:(PPCMachineState*)state;
-(void)StdCLib___assertprint:(PPCMachineState*)state;
-(void)StdCLib___DebugMallocHeap:(PPCMachineState*)state;
-(void)StdCLib___GetTrapType:(PPCMachineState*)state;
-(void)StdCLib___growFileTable:(PPCMachineState*)state;
-(void)StdCLib___NumToolboxTraps:(PPCMachineState*)state;
-(void)StdCLib___RestoreInitialCFragWorld:(PPCMachineState*)state;
-(void)StdCLib___RevertCFragWorld:(PPCMachineState*)state;
-(void)StdCLib___setjmp:(PPCMachineState*)state;
-(void)StdCLib___vec_longjmp:(PPCMachineState*)state;
-(void)StdCLib___vec_setjmp:(PPCMachineState*)state;
-(void)StdCLib__addDevHandler:(PPCMachineState*)state;
-(void)StdCLib__badPtr:(PPCMachineState*)state;
-(void)StdCLib__Bogus:(PPCMachineState*)state;
-(void)StdCLib__BreakPoint:(PPCMachineState*)state;
-(void)StdCLib__bufsync:(PPCMachineState*)state;
-(void)StdCLib__c2pstrcpy:(PPCMachineState*)state;
-(void)StdCLib__coClose:(PPCMachineState*)state;
-(void)StdCLib__coExit:(PPCMachineState*)state;
-(void)StdCLib__coFAccess:(PPCMachineState*)state;
-(void)StdCLib__coIoctl:(PPCMachineState*)state;
-(void)StdCLib__coRead:(PPCMachineState*)state;
-(void)StdCLib__coreIOExit:(PPCMachineState*)state;
-(void)StdCLib__coWrite:(PPCMachineState*)state;
-(void)StdCLib__cvt:(PPCMachineState*)state;
-(void)StdCLib__DoExitProcs:(PPCMachineState*)state;
-(void)StdCLib__doprnt:(PPCMachineState*)state;
-(void)StdCLib__doscan:(PPCMachineState*)state;
-(void)StdCLib__exit:(PPCMachineState*)state;
-(void)StdCLib__faccess:(PPCMachineState*)state;
-(void)StdCLib__filbuf:(PPCMachineState*)state;
-(void)StdCLib__findiop:(PPCMachineState*)state;
-(void)StdCLib__flsbuf:(PPCMachineState*)state;
-(void)StdCLib__fsClose:(PPCMachineState*)state;
-(void)StdCLib__fsFAccess:(PPCMachineState*)state;
-(void)StdCLib__fsIoctl:(PPCMachineState*)state;
-(void)StdCLib__fsRead:(PPCMachineState*)state;
-(void)StdCLib__FSSpec2Path:(PPCMachineState*)state;
-(void)StdCLib__fsWrite:(PPCMachineState*)state;
-(void)StdCLib__GetAliasInfo:(PPCMachineState*)state;
-(void)StdCLib__getDevHandler:(PPCMachineState*)state;
-(void)StdCLib__getIOPort:(PPCMachineState*)state;
-(void)StdCLib__memchr:(PPCMachineState*)state;
-(void)StdCLib__memcpy:(PPCMachineState*)state;
-(void)StdCLib__ResolveFileAlias:(PPCMachineState*)state;
-(void)StdCLib__rmemcpy:(PPCMachineState*)state;
-(void)StdCLib__RTExit:(PPCMachineState*)state;
-(void)StdCLib__RTInit:(PPCMachineState*)state;
-(void)StdCLib__SA_DeletePtr:(PPCMachineState*)state;
-(void)StdCLib__SA_GetPID:(PPCMachineState*)state;
-(void)StdCLib__SA_SetPtrSize:(PPCMachineState*)state;
-(void)StdCLib__syClose:(PPCMachineState*)state;
-(void)StdCLib__syFAccess:(PPCMachineState*)state;
-(void)StdCLib__syIoctl:(PPCMachineState*)state;
-(void)StdCLib__syRead:(PPCMachineState*)state;
-(void)StdCLib__syWrite:(PPCMachineState*)state;
-(void)StdCLib__uerror:(PPCMachineState*)state;
-(void)StdCLib__wrtchk:(PPCMachineState*)state;
-(void)StdCLib__xflsbuf:(PPCMachineState*)state;
-(void)StdCLib_abort:(PPCMachineState*)state;
-(void)StdCLib_abs:(PPCMachineState*)state;
-(void)StdCLib_access:(PPCMachineState*)state;
-(void)StdCLib_asctime:(PPCMachineState*)state;
-(void)StdCLib_atexit:(PPCMachineState*)state;
-(void)StdCLib_atof:(PPCMachineState*)state;
-(void)StdCLib_atoi:(PPCMachineState*)state;
-(void)StdCLib_atol:(PPCMachineState*)state;
-(void)StdCLib_atoll:(PPCMachineState*)state;
-(void)StdCLib_binhex:(PPCMachineState*)state;
-(void)StdCLib_bsearch:(PPCMachineState*)state;
-(void)StdCLib_calloc:(PPCMachineState*)state;
-(void)StdCLib_clearerr:(PPCMachineState*)state;
-(void)StdCLib_clock:(PPCMachineState*)state;
-(void)StdCLib_close:(PPCMachineState*)state;
-(void)StdCLib_ConvertTheString:(PPCMachineState*)state;
-(void)StdCLib_creat:(PPCMachineState*)state;
-(void)StdCLib_ctime:(PPCMachineState*)state;
-(void)StdCLib_difftime:(PPCMachineState*)state;
-(void)StdCLib_div:(PPCMachineState*)state;
-(void)StdCLib_dup:(PPCMachineState*)state;
-(void)StdCLib_ecvt:(PPCMachineState*)state;
-(void)StdCLib_exit:(PPCMachineState*)state;
-(void)StdCLib_faccess:(PPCMachineState*)state;
-(void)StdCLib_fclose:(PPCMachineState*)state;
-(void)StdCLib_fcntl:(PPCMachineState*)state;
-(void)StdCLib_fcvt:(PPCMachineState*)state;
-(void)StdCLib_fdopen:(PPCMachineState*)state;
-(void)StdCLib_feof:(PPCMachineState*)state;
-(void)StdCLib_ferror:(PPCMachineState*)state;
-(void)StdCLib_fflush:(PPCMachineState*)state;
-(void)StdCLib_fgetc:(PPCMachineState*)state;
-(void)StdCLib_fgetpos:(PPCMachineState*)state;
-(void)StdCLib_fgets:(PPCMachineState*)state;
-(void)StdCLib_fopen:(PPCMachineState*)state;
-(void)StdCLib_fprintf:(PPCMachineState*)state;
-(void)StdCLib_fputc:(PPCMachineState*)state;
-(void)StdCLib_fputs:(PPCMachineState*)state;
-(void)StdCLib_fread:(PPCMachineState*)state;
-(void)StdCLib_free:(PPCMachineState*)state;
-(void)StdCLib_freopen:(PPCMachineState*)state;
-(void)StdCLib_fscanf:(PPCMachineState*)state;
-(void)StdCLib_fseek:(PPCMachineState*)state;
-(void)StdCLib_fsetfileinfo:(PPCMachineState*)state;
-(void)StdCLib_fsetpos:(PPCMachineState*)state;
-(void)StdCLib_FSMakeFSSpec_Long:(PPCMachineState*)state;
-(void)StdCLib_FSp_creat:(PPCMachineState*)state;
-(void)StdCLib_FSp_faccess:(PPCMachineState*)state;
-(void)StdCLib_FSp_fopen:(PPCMachineState*)state;
-(void)StdCLib_FSp_freopen:(PPCMachineState*)state;
-(void)StdCLib_FSp_fsetfileinfo:(PPCMachineState*)state;
-(void)StdCLib_FSp_open:(PPCMachineState*)state;
-(void)StdCLib_FSp_remove:(PPCMachineState*)state;
-(void)StdCLib_FSp_rename:(PPCMachineState*)state;
-(void)StdCLib_FSp_unlink:(PPCMachineState*)state;
-(void)StdCLib_FSSpec2Path_Long:(PPCMachineState*)state;
-(void)StdCLib_ftell:(PPCMachineState*)state;
-(void)StdCLib_fwrite:(PPCMachineState*)state;
-(void)StdCLib_getc:(PPCMachineState*)state;
-(void)StdCLib_getchar:(PPCMachineState*)state;
-(void)StdCLib_getenv:(PPCMachineState*)state;
-(void)StdCLib_getIDstring:(PPCMachineState*)state;
-(void)StdCLib_getpid:(PPCMachineState*)state;
-(void)StdCLib_gets:(PPCMachineState*)state;
-(void)StdCLib_getw:(PPCMachineState*)state;
-(void)StdCLib_gmtime:(PPCMachineState*)state;
-(void)StdCLib_IEResolvePath:(PPCMachineState*)state;
-(void)StdCLib_ioctl:(PPCMachineState*)state;
-(void)StdCLib_isalnum:(PPCMachineState*)state;
-(void)StdCLib_isalpha:(PPCMachineState*)state;
-(void)StdCLib_isascii:(PPCMachineState*)state;
-(void)StdCLib_iscntrl:(PPCMachineState*)state;
-(void)StdCLib_isdigit:(PPCMachineState*)state;
-(void)StdCLib_isgraph:(PPCMachineState*)state;
-(void)StdCLib_islower:(PPCMachineState*)state;
-(void)StdCLib_isprint:(PPCMachineState*)state;
-(void)StdCLib_ispunct:(PPCMachineState*)state;
-(void)StdCLib_isspace:(PPCMachineState*)state;
-(void)StdCLib_isupper:(PPCMachineState*)state;
-(void)StdCLib_isxdigit:(PPCMachineState*)state;
-(void)StdCLib_labs:(PPCMachineState*)state;
-(void)StdCLib_ldiv:(PPCMachineState*)state;
-(void)StdCLib_llabs:(PPCMachineState*)state;
-(void)StdCLib_lldiv:(PPCMachineState*)state;
-(void)StdCLib_localeconv:(PPCMachineState*)state;
-(void)StdCLib_localtime:(PPCMachineState*)state;
-(void)StdCLib_longjmp:(PPCMachineState*)state;
-(void)StdCLib_lseek:(PPCMachineState*)state;
-(void)StdCLib_MakeResolvedFSSpec:(PPCMachineState*)state;
-(void)StdCLib_MakeResolvedFSSpec_Long:(PPCMachineState*)state;
-(void)StdCLib_MakeResolvedPath:(PPCMachineState*)state;
-(void)StdCLib_MakeResolvedPath_Long:(PPCMachineState*)state;
-(void)StdCLib_MakeTheLocaleString:(PPCMachineState*)state;
-(void)StdCLib_malloc:(PPCMachineState*)state;
-(void)StdCLib_mblen:(PPCMachineState*)state;
-(void)StdCLib_mbstowcs:(PPCMachineState*)state;
-(void)StdCLib_mbtowc:(PPCMachineState*)state;
-(void)StdCLib_memccpy:(PPCMachineState*)state;
-(void)StdCLib_memchr:(PPCMachineState*)state;
-(void)StdCLib_memcmp:(PPCMachineState*)state;
-(void)StdCLib_memcpy:(PPCMachineState*)state;
-(void)StdCLib_memmove:(PPCMachineState*)state;
-(void)StdCLib_memset:(PPCMachineState*)state;
-(void)StdCLib_mktemp:(PPCMachineState*)state;
-(void)StdCLib_mktime:(PPCMachineState*)state;
-(void)StdCLib_open:(PPCMachineState*)state;
-(void)StdCLib_ParseTheLocaleString:(PPCMachineState*)state;
-(void)StdCLib_perror:(PPCMachineState*)state;
-(void)StdCLib_PLpos:(PPCMachineState*)state;
-(void)StdCLib_PLstrcat:(PPCMachineState*)state;
-(void)StdCLib_PLstrchr:(PPCMachineState*)state;
-(void)StdCLib_PLstrcmp:(PPCMachineState*)state;
-(void)StdCLib_PLstrcpy:(PPCMachineState*)state;
-(void)StdCLib_PLstrlen:(PPCMachineState*)state;
-(void)StdCLib_PLstrncat:(PPCMachineState*)state;
-(void)StdCLib_PLstrncmp:(PPCMachineState*)state;
-(void)StdCLib_PLstrncpy:(PPCMachineState*)state;
-(void)StdCLib_PLstrpbrk:(PPCMachineState*)state;
-(void)StdCLib_PLstrrchr:(PPCMachineState*)state;
-(void)StdCLib_PLstrspn:(PPCMachineState*)state;
-(void)StdCLib_PLstrstr:(PPCMachineState*)state;
-(void)StdCLib_printf:(PPCMachineState*)state;
-(void)StdCLib_putc:(PPCMachineState*)state;
-(void)StdCLib_putchar:(PPCMachineState*)state;
-(void)StdCLib_puts:(PPCMachineState*)state;
-(void)StdCLib_putw:(PPCMachineState*)state;
-(void)StdCLib_qsort:(PPCMachineState*)state;
-(void)StdCLib_raise:(PPCMachineState*)state;
-(void)StdCLib_rand:(PPCMachineState*)state;
-(void)StdCLib_read:(PPCMachineState*)state;
-(void)StdCLib_realloc:(PPCMachineState*)state;
-(void)StdCLib_remove:(PPCMachineState*)state;
-(void)StdCLib_rename:(PPCMachineState*)state;
-(void)StdCLib_ResolveFolderAliases:(PPCMachineState*)state;
-(void)StdCLib_ResolveFolderAliases_Long:(PPCMachineState*)state;
-(void)StdCLib_ResolvePath:(PPCMachineState*)state;
-(void)StdCLib_ResolvePath_Long:(PPCMachineState*)state;
-(void)StdCLib_rewind:(PPCMachineState*)state;
-(void)StdCLib_scanf:(PPCMachineState*)state;
-(void)StdCLib_setbuf:(PPCMachineState*)state;
-(void)StdCLib_setenv:(PPCMachineState*)state;
-(void)StdCLib_setlocale:(PPCMachineState*)state;
-(void)StdCLib_setvbuf:(PPCMachineState*)state;
-(void)StdCLib_signal:(PPCMachineState*)state;
-(void)StdCLib_sprintf:(PPCMachineState*)state;
-(void)StdCLib_srand:(PPCMachineState*)state;
-(void)StdCLib_sscanf:(PPCMachineState*)state;
-(void)StdCLib_strcat:(PPCMachineState*)state;
-(void)StdCLib_strchr:(PPCMachineState*)state;
-(void)StdCLib_strcmp:(PPCMachineState*)state;
-(void)StdCLib_strcoll:(PPCMachineState*)state;
-(void)StdCLib_strcpy:(PPCMachineState*)state;
-(void)StdCLib_strcspn:(PPCMachineState*)state;
-(void)StdCLib_strerror:(PPCMachineState*)state;
-(void)StdCLib_strftime:(PPCMachineState*)state;
-(void)StdCLib_strlen:(PPCMachineState*)state;
-(void)StdCLib_strncat:(PPCMachineState*)state;
-(void)StdCLib_strncmp:(PPCMachineState*)state;
-(void)StdCLib_strncpy:(PPCMachineState*)state;
-(void)StdCLib_strpbrk:(PPCMachineState*)state;
-(void)StdCLib_strrchr:(PPCMachineState*)state;
-(void)StdCLib_strspn:(PPCMachineState*)state;
-(void)StdCLib_strstr:(PPCMachineState*)state;
-(void)StdCLib_strtod:(PPCMachineState*)state;
-(void)StdCLib_strtok:(PPCMachineState*)state;
-(void)StdCLib_strtol:(PPCMachineState*)state;
-(void)StdCLib_strtoll:(PPCMachineState*)state;
-(void)StdCLib_strtoul:(PPCMachineState*)state;
-(void)StdCLib_strtoull:(PPCMachineState*)state;
-(void)StdCLib_strxfrm:(PPCMachineState*)state;
-(void)StdCLib_system:(PPCMachineState*)state;
-(void)StdCLib_time:(PPCMachineState*)state;
-(void)StdCLib_tmpfile:(PPCMachineState*)state;
-(void)StdCLib_tmpnam:(PPCMachineState*)state;
-(void)StdCLib_toascii:(PPCMachineState*)state;
-(void)StdCLib_tolower:(PPCMachineState*)state;
-(void)StdCLib_toupper:(PPCMachineState*)state;
-(void)StdCLib_TrapAvailable:(PPCMachineState*)state;
-(void)StdCLib_ungetc:(PPCMachineState*)state;
-(void)StdCLib_unlink:(PPCMachineState*)state;
-(void)StdCLib_vec_calloc:(PPCMachineState*)state;
-(void)StdCLib_vec_free:(PPCMachineState*)state;
-(void)StdCLib_vec_malloc:(PPCMachineState*)state;
-(void)StdCLib_vec_realloc:(PPCMachineState*)state;
-(void)StdCLib_vfprintf:(PPCMachineState*)state;
-(void)StdCLib_vprintf:(PPCMachineState*)state;
-(void)StdCLib_vsprintf:(PPCMachineState*)state;
-(void)StdCLib_wcstombs:(PPCMachineState*)state;
-(void)StdCLib_wctomb:(PPCMachineState*)state;
-(void)StdCLib_write:(PPCMachineState*)state;

@end