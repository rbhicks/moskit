#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <common-utility.h>

#include "extension-support.h"
#include "clips.h"

static char *  g_falseString     = "FALSE";
static char *  g_trueString      = "TRUE";
static char *  g_buttonString    = "BUTTON";
static char *  g_comboBoxString  = "COMBOBOX";
static char *  g_editString      = "EDIT";

char * g_logEntryTypeStrings [] = {"'FILE", "'REGISTRY", "'MESSAGE", "'ERROR", "'DEBUG"};

BOOL CALLBACK EnumerateInstallParentWindowsCallback   (HWND hWnd, LPARAM lParam);
BOOL CALLBACK EnumerateInstallChildWindowsCallback    (HWND hWnd, LPARAM lParam);


BOOL CALLBACK EnumerateInstallParentWindowsCallback (HWND hWnd, LPARAM lParam) {

   PARENT_WINDOW_ENUMERATION_INFO * pParentWindowEnumerationInfo  = (PARENT_WINDOW_ENUMERATION_INFO *) lParam;
   DWORD windowProcessId	                                       = 0;
	DWORD windowGUIThreadId                                        = 0;


   windowGUIThreadId = GetWindowThreadProcessId (hWnd, &windowProcessId);

   if (windowProcessId == pParentWindowEnumerationInfo->installProcessId) {

      char           windowHandleConversionBuffer  [65];
      char 	         startingTimeConversionBuffer  [33];
      unsigned int   windowHandleConversionBufferLength  = 0;
      unsigned int   startingTimeConversionBufferLength  = 0;
      unsigned int   dataBufferSize                      = 0;
      char *         dataBuffer                          = NULL;
      char *         dataBufferCopyPointer               = NULL;

      
      _i64toa  ((LONG_PTR) hWnd,                            windowHandleConversionBuffer, 10);
      _itoa    (pParentWindowEnumerationInfo->startingTime, startingTimeConversionBuffer, 10);

      windowHandleConversionBufferLength  = strlen (windowHandleConversionBuffer);
      startingTimeConversionBufferLength  = strlen (startingTimeConversionBuffer);

      dataBufferSize =  windowHandleConversionBufferLength  + 1 +
                        37                                  +      // size of UUID with terminator
                        startingTimeConversionBufferLength  + 1;

      dataBuffer              = (char *) calloc (dataBufferSize, sizeof (*dataBuffer));
      dataBufferCopyPointer   = dataBuffer;

      memcpy ( dataBufferCopyPointer,
               windowHandleConversionBuffer,
               windowHandleConversionBufferLength);

      dataBufferCopyPointer   += windowHandleConversionBufferLength + 1;   // add 1 for embedded null separator

      memcpy ( dataBufferCopyPointer,
               pParentWindowEnumerationInfo->enumerationId,
               37);

      dataBufferCopyPointer   += 37;

      memcpy ( dataBufferCopyPointer,
               startingTimeConversionBuffer,
               startingTimeConversionBufferLength);

      SendEventData (WRANGLER_PARENT_WINDOW, dataBuffer, dataBufferSize);
      
      free (dataBuffer);
   }

   return TRUE;
}


BOOL CALLBACK EnumerateInstallChildWindowsCallback (HWND hWnd, LPARAM lParam) {
   
   CHILD_WINDOW_ENUMERATION_INFO *  pChildWindowEnumerationInfo               = (CHILD_WINDOW_ENUMERATION_INFO *) lParam;
   HWND                             parentHwnd                                = pChildWindowEnumerationInfo->parentHwnd;
   char 		                        windowHandleConversionBuffer        [65];
   char 		                        parentWindowHandleConversionBuffer  [65];
   char 	                           startingTimeConversionBuffer        [33];
   unsigned int                     windowHandleConversionBufferLength        = 0;
   unsigned int                     parentWindowHandleConversionBufferLength  = 0;
   unsigned int                     startingTimeConversionBufferLength        = 0;
   unsigned int                     dataBufferSize                            = 0;
   char *                           dataBuffer                                = NULL;
   char *                           dataBufferCopyPointer                     = NULL;


   _i64toa  ((LONG_PTR) hWnd,                            windowHandleConversionBuffer,       10);
   _i64toa  ((LONG_PTR) parentHwnd,                      parentWindowHandleConversionBuffer, 10);
   _itoa    (pChildWindowEnumerationInfo->startingTime,  startingTimeConversionBuffer,       10);

   windowHandleConversionBufferLength        = strlen (windowHandleConversionBuffer);
   parentWindowHandleConversionBufferLength  = strlen (parentWindowHandleConversionBuffer);
   startingTimeConversionBufferLength        = strlen (startingTimeConversionBuffer);

   dataBufferSize          =  windowHandleConversionBufferLength        + 1 +
                              parentWindowHandleConversionBufferLength  + 1 +
                              37                                        +     // size of UUID with terminator
                              startingTimeConversionBufferLength        + 1;

   dataBuffer              = (char *) calloc (dataBufferSize, sizeof (*dataBuffer));
   dataBufferCopyPointer   = dataBuffer;

   memcpy (dataBufferCopyPointer, windowHandleConversionBuffer, windowHandleConversionBufferLength);

   dataBufferCopyPointer   += windowHandleConversionBufferLength + 1;

   memcpy (dataBufferCopyPointer, parentWindowHandleConversionBuffer, parentWindowHandleConversionBufferLength);

   dataBufferCopyPointer   += parentWindowHandleConversionBufferLength + 1;

   memcpy ( dataBufferCopyPointer,
            pChildWindowEnumerationInfo->enumerationId,
            37);

   dataBufferCopyPointer   += 37;

   memcpy ( dataBufferCopyPointer,
            startingTimeConversionBuffer,
            startingTimeConversionBufferLength);

   SendEventData (WRANGLER_CHILD_WINDOW, dataBuffer, dataBufferSize);

   free (dataBuffer);

   return TRUE;
}


char * ExtractCharString (Scheme_Object * schemeCharString) {

   mzchar * intermediateCharString        = NULL;
   int      intermediateCharStringLength  = 0;

   char * extractedCharString = NULL;


   MZ_GC_DECL_REG    (2);
   MZ_GC_VAR_IN_REG  (0, schemeCharString);
   MZ_GC_VAR_IN_REG  (1, intermediateCharString);
   MZ_GC_REG         ();

   intermediateCharStringLength  = SCHEME_CHAR_STRLEN_VAL   (schemeCharString) + 1;
   intermediateCharString        = SCHEME_CHAR_STR_VAL      (schemeCharString);

   extractedCharString = (char *) calloc (intermediateCharStringLength, sizeof (*intermediateCharString));

   scheme_utf8_encode_all (   intermediateCharString,
                              intermediateCharStringLength,
                              (unsigned char *) extractedCharString);

   MZ_GC_UNREG ();

   return extractedCharString;
}


void FreeExtractedCharString (char * extractedCharString) {

   if (NULL != extractedCharString) {

      free (extractedCharString);
   }
}


char * ExtractByteString (Scheme_Object * schemeByteString) {

   char *   intermediateByteString        = NULL;
   int      intermediateByteStringLength  = 0;

   char * extractedByteString = NULL;

   MZ_GC_DECL_REG    (2);
   MZ_GC_VAR_IN_REG  (0, schemeByteString);
   MZ_GC_VAR_IN_REG  (1, intermediateByteString);
   MZ_GC_REG         ();

   intermediateByteStringLength  = SCHEME_BYTE_STRLEN_VAL   (schemeByteString) + 1;
   intermediateByteString        = SCHEME_BYTE_STR_VAL      (schemeByteString);

   extractedByteString = (char *) calloc (intermediateByteStringLength, sizeof (*intermediateByteString));

   strcpy (extractedByteString, intermediateByteString);

   MZ_GC_UNREG ();

   return extractedByteString;
}


void FreeExtractedByteString (char * extractedByteString) {

   if (NULL != extractedByteString) {

      free (extractedByteString);
   }
}


char * ExtractSymbolString (Scheme_Object * schemeSymbolString) {

   char *   intermediateSymbolString         = NULL;
   int      intermediateSymbolStringLength   = 0;

   char * extractedSymbolString = NULL;

   MZ_GC_DECL_REG    (2);
   MZ_GC_VAR_IN_REG  (0, schemeSymbolString);
   MZ_GC_VAR_IN_REG  (1, intermediateSymbolString);
   MZ_GC_REG         ();

   intermediateSymbolStringLength   = SCHEME_SYM_LEN  (schemeSymbolString) + 1;
   intermediateSymbolString         = SCHEME_SYM_VAL  (schemeSymbolString);

   extractedSymbolString = (char *) calloc (intermediateSymbolStringLength, sizeof (*intermediateSymbolString));

   strcpy (extractedSymbolString, intermediateSymbolString);

   MZ_GC_UNREG ();

   return extractedSymbolString;
}


void FreeExtractedSymbolString (char * extractedSymbolString) {

   if (NULL != extractedSymbolString) {

      free (extractedSymbolString);
   }
}


char * ExtractKeywordString (Scheme_Object * schemeKeywordString) {

   char *   intermediateKeywordString        = NULL;
   int      intermediateKeywordStringLength  = 0;

   char * extractedKeywordString = NULL;

   MZ_GC_DECL_REG    (2);
   MZ_GC_VAR_IN_REG  (0, schemeKeywordString);
   MZ_GC_VAR_IN_REG  (1, intermediateKeywordString);
   MZ_GC_REG         ();

   intermediateKeywordStringLength  = SCHEME_SYM_LEN  (schemeKeywordString) + 1;
   intermediateKeywordString        = SCHEME_SYM_VAL  (schemeKeywordString);

   extractedKeywordString = (char *) calloc (intermediateKeywordStringLength, sizeof (*intermediateKeywordString));

   strcpy (extractedKeywordString, intermediateKeywordString);

   MZ_GC_UNREG ();

   return extractedKeywordString;
}


void FreeExtractedKeywordString (char * extractedKeywordString) {

   if (NULL != extractedKeywordString) {

      free (extractedKeywordString);
   }
}


LONG_PTR SchemeObjectToLongPtr (Scheme_Object * hwndAsSChemeString) {

   mzchar *          longPtrMZString      = NULL;
   unsigned int      longPtrStringLength  = 0;
   unsigned char *   longPtrString        = NULL;
   LONG_PTR          longPtr              = 0;


   MZ_GC_DECL_REG    (2);
   MZ_GC_VAR_IN_REG  (0, hwndAsSChemeString);
   MZ_GC_VAR_IN_REG  (1, longPtrMZString);
   MZ_GC_REG         ();

   longPtrStringLength  = SCHEME_CHAR_STRLEN_VAL   (hwndAsSChemeString) + 1;
   longPtrMZString      = SCHEME_CHAR_STR_VAL      (hwndAsSChemeString);

   longPtrString = (unsigned char *) calloc (longPtrStringLength, sizeof (*longPtrString));

   scheme_utf8_encode_all (   longPtrMZString,
                              longPtrStringLength,
                              longPtrString);

   longPtr = (LONG_PTR) _atoi64 ((char *) longPtrString);

   MZ_GC_UNREG ();

   free (longPtrString);

   return longPtr;
}


HWND SchemeObjectToHwnd (Scheme_Object * hwndAsSChemeNumber) {

   HWND windowHandle = NULL;


   MZ_GC_DECL_REG    (1);
   MZ_GC_VAR_IN_REG  (0, hwndAsSChemeNumber);
   MZ_GC_REG         ();

   windowHandle = (HWND) SCHEME_INT_VAL (hwndAsSChemeNumber);

   MZ_GC_UNREG ();

   return windowHandle;
}

int ClipsIoQueryFunction (void * theEnv, char * logicalName) {

   theEnv;
   logicalName;

   // accept all logical names since all of the I/O will be routed to the console
   return TRUE;
}


int ClipsIoPrintFunction (void * theEnv, char * logicalName, char * printString) {

   HANDLE   hConsole    = GetStdHandle (STD_OUTPUT_HANDLE);
   DWORD    dwCharCount = 0;


   theEnv;
   logicalName;

   WriteConsoleA (hConsole, printString, strlen (printString), &dwCharCount, NULL);

   return 0;
}


int ClipsIoExitFunction (void * theEnv, int exitCode) {

   theEnv;
   exitCode;

   // this return value also seems to be unused
   return 0;
}


void PopulateClipsDataObectFromSchemeObject (   void *            theEnv,
                                                DATA_OBJECT_PTR   dataObjectPtr,
                                                Scheme_Object *   schemeObject) {

   DWORD schemeType = SCHEME_TYPE (schemeObject);


   switch (schemeType) {
      
      case scheme_char_string_type:
         {
            char * returnedString = ExtractCharString (schemeObject);


            // map to STRING
            EnvSetpType    (theEnv, dataObjectPtr, STRING);
            EnvSetpValue   (theEnv, dataObjectPtr, EnvAddSymbol (theEnv, returnedString));

            FreeExtractedCharString (returnedString);
         }
         break;

      case scheme_byte_string_type:
         {
            char * returnedString = ExtractByteString (schemeObject);


            // map to STRING
            EnvSetpType    (theEnv, dataObjectPtr, STRING);
            EnvSetpValue   (theEnv, dataObjectPtr, EnvAddSymbol (theEnv, returnedString));

            FreeExtractedByteString (returnedString);
         }
         break;

      // documented but undefined for some reason
      //case scheme_path_type:

         // map to STRING
         // break;

      case scheme_symbol_type:
         {
            char * returnedString = ExtractSymbolString (schemeObject);


            // map to SYMBOL
            EnvSetpType    (theEnv, dataObjectPtr, SYMBOL);
            EnvSetpValue   (theEnv, dataObjectPtr, EnvAddSymbol (theEnv, returnedString));

            FreeExtractedSymbolString (returnedString);
         }
         break;

      case scheme_keyword_type:
         {
            char * returnedString = ExtractKeywordString (schemeObject);


            // map to SYMBOL
            EnvSetpType    (theEnv, dataObjectPtr, SYMBOL);
            EnvSetpValue   (theEnv, dataObjectPtr, EnvAddSymbol (theEnv, returnedString));

            FreeExtractedKeywordString (returnedString);
         }
         break;

      case scheme_true_type:

         // map to SYMBOL
         EnvSetpType    (theEnv, dataObjectPtr, SYMBOL);
         EnvSetpValue   (theEnv, dataObjectPtr, EnvTrueSymbol (theEnv));
         break;

      case scheme_false_type:

         // map to SYMBOL
         EnvSetpType    (theEnv, dataObjectPtr, SYMBOL);
         EnvSetpValue   (theEnv, dataObjectPtr, EnvFalseSymbol (theEnv));
         break;

      case scheme_double_type:
      case scheme_float_type:
      case scheme_rational_type:

         // map to FLOAT (CLIPS internally will store as double precision)
         EnvSetpType    (theEnv, dataObjectPtr, FLOAT);
         EnvSetpValue   (theEnv, dataObjectPtr, EnvAddDouble (theEnv, SCHEME_DBL_VAL (schemeObject)));
         break;

      case scheme_bignum_type:

         // map to FLOAT (CLIPS internally will store as double precision)
         EnvSetpType    (theEnv, dataObjectPtr, FLOAT);
         EnvSetpValue   (theEnv, dataObjectPtr, EnvAddDouble (theEnv, scheme_bignum_to_double (schemeObject)));
         break;

      case scheme_integer_type:

         // map to INTEGER
         EnvSetpType    (theEnv, dataObjectPtr, INTEGER);
         EnvSetpValue   (theEnv, dataObjectPtr, EnvAddLong (theEnv, SCHEME_INT_VAL (schemeObject)));
         break;

      case scheme_vector_type:

         // covert vector to list (which is has the type 'pair' and then...
         schemeObject = scheme_vector_to_list (schemeObject);
         // ...drop through to the pair case
      case scheme_pair_type:
      case scheme_mutable_pair_type:
         {
            int listLength = scheme_proper_list_length (schemeObject);

            if (-1 != listLength) {

               // map to MULTIFIELD
               EnvSetpType    (theEnv, dataObjectPtr, MULTIFIELD);
               EnvSetpValue   (theEnv, dataObjectPtr, SchemeListToMultiField (theEnv, schemeObject, listLength));
   
               EnvSetpDOBegin (theEnv, dataObjectPtr, 1);
               EnvSetpDOEnd   (theEnv, dataObjectPtr, listLength);
            }
            else {

               EnvSetpType    (theEnv, dataObjectPtr, SYMBOL);
               EnvSetpValue   (theEnv, dataObjectPtr, EnvFalseSymbol (theEnv));

               SendLogEntry (WRANGLER_ERROR, "Converting improper scheme lists to CLIPS multifields not supported.");
            }
         }
         break;
  
      case scheme_cpointer_type:

         // map to EXTERNAL_ADDRESS
         EnvSetpType    (theEnv, dataObjectPtr, EXTERNAL_ADDRESS);
         EnvSetpValue   (theEnv, dataObjectPtr, SCHEME_CPTR_VAL (schemeObject));
         break;

      case scheme_void_type:

         // map to RVOID (clips internal "void return" type.
         // set value to false (same as internal clips code; see prcdrfun.c)
         EnvSetpType    (theEnv, dataObjectPtr, RVOID);
         EnvSetpValue   (theEnv, dataObjectPtr, EnvFalseSymbol (theEnv));

         break;

      case scheme_structure_type:
      case scheme_struct_type_type:
      case scheme_struct_property_type:
      case scheme_hash_table_type:
      case scheme_bucket_table_type:
      case scheme_complex_type:
      // documented but undefined for some reason
      //case scheme_complex_izi_type:
      case scheme_box_type:
      case scheme_input_port_type:
      case scheme_output_port_type:
      case scheme_thread_type:
      case scheme_sema_type:
      case scheme_weak_box_type:
      case scheme_namespace_type:
      case scheme_prim_type:
      case scheme_closed_prim_type:
      // documented but undefined for some reason
      //case scheme_compiled_closure_type:
      case scheme_escaping_cont_type:
      case scheme_case_closure_type:
      case scheme_native_closure_type:
      default:
         {

            char           formatString               []    = "Unsupported scheme type (%s) to CLIPS type conversion.";
            char           schemeTypeConversionBuffer [33];
            unsigned int   logEntrySize                     = sizeof (formatString) + 33;
            char *         logEntry                         = NULL;


            // unhandled types map to false
            EnvSetpType    (theEnv, dataObjectPtr, SYMBOL);
            EnvSetpValue   (theEnv, dataObjectPtr, EnvFalseSymbol (theEnv));

            _itoa (schemeType, schemeTypeConversionBuffer, 10);

            logEntry = (char *) calloc (logEntrySize, sizeof (*logEntry));

            sprintf (logEntry, formatString, schemeTypeConversionBuffer);
   
            SendLogEntry (WRANGLER_ERROR, logEntry);

            free (logEntry);
         }
   }
}


// 'mapped' refers the fact that the CLIPS value is sprintf'd in such a way as to map
// meaningfully to a scheme literal string 
char * ClipsValueToMappedSchemeLiteralString (DATA_OBJECT_PTR dataObjectPtr) {

   unsigned int   mappedSchemeLiteralStringLength  = 0;
   char *         mappedSchemeLiteralString        = NULL;
   DWORD          clipsType                        = GetpType (dataObjectPtr);


   switch (clipsType) {
      
      case SYMBOL:
         {
            char * lexemeValue = DOPToString (dataObjectPtr);
            

            mappedSchemeLiteralStringLength += 1;                      // null terminator
            mappedSchemeLiteralStringLength += strlen (lexemeValue);   // value size

            mappedSchemeLiteralString = (char *) calloc (   mappedSchemeLiteralStringLength,
                                                            sizeof (*mappedSchemeLiteralString));

            sprintf (mappedSchemeLiteralString, "%s", lexemeValue);
         }
         break;

      case STRING:
      case INSTANCE_NAME:
         {
            char * lexemeValue = DOPToString (dataObjectPtr);
            

            mappedSchemeLiteralStringLength += 1;                      // null terminator
            mappedSchemeLiteralStringLength += 4;                      // two escaped double-quotes
            mappedSchemeLiteralStringLength += strlen (lexemeValue);   // value size

            mappedSchemeLiteralString = (char *) calloc (   mappedSchemeLiteralStringLength,
                                                            sizeof (*mappedSchemeLiteralString));

            sprintf (mappedSchemeLiteralString, "\"%s\"", lexemeValue);
         }
         break;

      case FLOAT:
         {
            double doubleValue = DOPToDouble (dataObjectPtr);


            mappedSchemeLiteralStringLength += 1;     // null terminator
            mappedSchemeLiteralStringLength += 128;   // should be far bigger than needed to hold
                                                      // a double in any std string representation
                                                      // and null terminator will truncate the excess
                                                      // when the value is strcat'd in the caller

            mappedSchemeLiteralString = (char *) calloc (   mappedSchemeLiteralStringLength,
                                                            sizeof (*mappedSchemeLiteralString));

            sprintf (mappedSchemeLiteralString, "%f", doubleValue);
         }
         break;

      case INTEGER:
         {
            int integerValue = DOPToInteger (dataObjectPtr);


            mappedSchemeLiteralStringLength += 1;  // null terminator
            mappedSchemeLiteralStringLength += 65; // large enough to hold 32 and 64 bit representations
                                                   // (currently, only 32-bit integers will be pased)
                                                   // and null terminator will truncate the excess
                                                   // when the value is strcat'd in the caller


            mappedSchemeLiteralString = (char *) calloc (   mappedSchemeLiteralStringLength,
                                                            sizeof (*mappedSchemeLiteralString));

            sprintf (mappedSchemeLiteralString, "%i", integerValue);
         }
         break;

      case MULTIFIELD:

         SendLogEntry (WRANGLER_ERROR, "Multifield has no mapped format string.");

         free (mappedSchemeLiteralString);
         mappedSchemeLiteralString = NULL;

         break;

      case INSTANCE_ADDRESS:
      case EXTERNAL_ADDRESS:
         {
            unsigned int addressValue = (unsigned int) DOPToPointer (dataObjectPtr);


            mappedSchemeLiteralStringLength += 1;  // null terminator
            mappedSchemeLiteralStringLength += 65; // large enough to hold 32 and 64 bit representations
                                                   // (currently, only 32-bit pointers will be pased)
                                                   // and null terminator will truncate the excess
                                                   // when the value is strcat'd in the caller


            mappedSchemeLiteralString = (char *) calloc (   mappedSchemeLiteralStringLength,
                                                            sizeof (*mappedSchemeLiteralString));

            sprintf (mappedSchemeLiteralString, "%u", addressValue);
         }
         break;

      default:
         {
            char           formatString               []    = "Unsupport clips type (%s); no mapped format string.";
            char           clipsTypeConversionBuffer  [33];
            unsigned int   logEntrySize                     = sizeof (formatString) + 33;
            char *         logEntry                         = NULL;


            _itoa (clipsType, clipsTypeConversionBuffer, 10);

            logEntry = (char *) calloc (logEntrySize, sizeof (*logEntry));

            sprintf (logEntry, formatString, clipsTypeConversionBuffer);

            SendLogEntry (WRANGLER_ERROR, logEntry);

            free (logEntry);
   
            free (mappedSchemeLiteralString);
            mappedSchemeLiteralString = NULL;
         }
   }

   return mappedSchemeLiteralString;
}


void FreeMappedSchemeLiteralString (char * mappedSchemeLiteralString) {

   if (NULL != mappedSchemeLiteralString) {
      
      free (mappedSchemeLiteralString);
   }
}


void * SchemeListToMultiField (void * theEnv, Scheme_Object * schemeList, int listLength) {

   Scheme_Object *   car         = scheme_car   (schemeList);
   Scheme_Object *   cdr         = scheme_cdr   (schemeList);
   Scheme_Type       carType     = SCHEME_TYPE  (car);

   int               i           = 0;

   DATA_OBJECT       clipsDataObject;

   void * returnMultifield = NULL;

      
   returnMultifield = EnvCreateMultifield (theEnv, listLength);

   for (i = 0; i < listLength; ++i) {

      carType = SCHEME_TYPE (car);
   
      if (scheme_pair_type != carType) {
      
         PopulateClipsDataObectFromSchemeObject (theEnv, &clipsDataObject, car);         

         EnvSetMFType   (theEnv, returnMultifield, (i+1), EnvGetType    (theEnv, clipsDataObject));
         EnvSetMFValue  (theEnv, returnMultifield, (i+1), EnvGetValue   (theEnv, clipsDataObject));

         car = scheme_car (cdr);
         cdr = scheme_cdr (cdr);
      }
      else {

         SendLogEntry (WRANGLER_ERROR, "Converting nested scheme lists to CLIPS multifields not supported.");

         break;
      }
   }
  
   return returnMultifield;
}


void RegisterProcessId (DWORD processId) {
   
   char              formatString      [] = "(register-process-id %i)";
   char *            schemeString         = NULL;
   int               schemeStringLength   = sizeof (formatString) + 33 + 1; // max dword string size + terminator
   Scheme_Config *   schemeConfig         = NULL;
   Scheme_Env *      schemeEnv            = NULL;           


   MZ_GC_DECL_REG    (2);
   MZ_GC_VAR_IN_REG  (0, schemeConfig);
   MZ_GC_VAR_IN_REG  (1, schemeEnv);
   MZ_GC_REG         ();

   schemeString = (char *) calloc (schemeStringLength, sizeof (*schemeString));
  
   sprintf (schemeString, formatString, processId);

   schemeConfig = scheme_current_config   ();
   schemeEnv    = scheme_get_env          (schemeConfig);     

   scheme_eval_string (schemeString, schemeEnv);

   MZ_GC_UNREG ();

   free (schemeString);
}
