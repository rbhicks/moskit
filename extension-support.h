#ifndef __EXTENSION_SUPPORT_H__
#define __EXTENSION_SUPPORT_H__

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#pragma warning(disable : 4214)
#define SCHEME_DIRECT_EMBEDDED 0
#include "scheme.h"
#pragma warning(default : 4214)

#include "clips.h"


typedef struct {

   DWORD          installProcessId;
   char           enumerationId     [37];
   unsigned long  startingTime;

} PARENT_WINDOW_ENUMERATION_INFO;


typedef struct {

   HWND           parentHwnd;
   char           enumerationId  [37];
   unsigned long  startingTime;

} CHILD_WINDOW_ENUMERATION_INFO;


extern void    ProcessWindowData			               (HWND hWnd, void * theEnv);
extern char *  ExtractCharString                      (Scheme_Object * schemeCharString);
extern void    FreeExtractedCharString                (char * extractedCharString);
extern char *  ExtractByteString                      (Scheme_Object * schemeByteString);
extern void    FreeExtractedByteString                (char * extractedByteString);
extern char *  ExtractSymbolString                    (Scheme_Object * schemeSymbolString);
extern void    FreeExtractedSymbolString              (char * extractedSymbolString);
extern char *  ExtractKeywordString                   (Scheme_Object * schemeKeywordString);
extern void    FreeExtractedKeywordString             (char * extractedKeywordString);
extern void    EnableMessageLogging                   ();
extern void    DisableMessageLogging                  ();
extern void    EnableDebugLogging                     ();
extern void    DisableDebugLogging                    ();
extern void    PopulateClipsDataObectFromSchemeObject (void * theEnv, DATA_OBJECT_PTR dataObjectPtr, Scheme_Object * schemeObject);
extern void *  SchemeListToMultiField                 (void * theEnv, Scheme_Object * schemeList, int listLength);
extern void    RegisterProcessId                      (DWORD processId);
extern char *  ClipsValueToMappedSchemeLiteralString  (DATA_OBJECT_PTR dataObjectPtr);
extern void    FreeMappedSchemeLiteralString          (char * mappedSchemeLiteralString);

#endif //__EXTENSION_SUPPORT_H__
