#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <psapi.h>
#include <time.h>

#include <common-utility.h>

#include "clips.h"
#include "clips-extensions.h"
#include "extension-support.h"

extern BOOL          LaunchInstall                          (char * exeName);
extern BOOL CALLBACK EnumerateInstallParentWindowsCallback  (HWND hWnd, LPARAM lParam);
extern BOOL CALLBACK EnumerateInstallChildWindowsCallback   (HWND hWnd, LPARAM lParam);
extern HWND          SchemeObjectToHwnd                     (Scheme_Object * hwndAsSChemeString);
extern LONG_PTR      SchemeObjectToLongPtr                  (Scheme_Object * hwndAsSChemeString);
extern int           ClipsIoQueryFunction                   (void * theEnv, char * logicalName);
extern int           ClipsIoPrintFunction                   (void * theEnv, char * logicalName, char * printString);
extern int           ClipsIoExitFunction                    (void * theEnv, int exitCode);



// ////////////////////
// /////////////////////////////////////////////
// clips api extensions
// /////////////////////////////////////////////
// ////////////////////



BOOL WINAPI ConsoleHandlerRoutine (DWORD dwCtrlType) {

   dwCtrlType;

   // we want the console to just be for display and not do anything (responding to
   // clicking 'X' is handled below), so respond TRUE (handled) for all KB events.
   return TRUE;
}


Scheme_Object * clips_create_environment (int argc, Scheme_Object ** argv) {

   void * env = CreateEnvironment ();


   // standard signature
   argc;
   argv;

   // reroute the clips I/O for this environment to the console
   if (0 == EnvAddRouter ( env,
                           "scheme-router",
                           20,
                           ClipsIoQueryFunction,
                           ClipsIoPrintFunction,
                           NULL,
                           NULL,
                           ClipsIoExitFunction)) {

      char           errorString       [] = "Unable to reroute CLIPS UI";
      char           formatString      [] = "(display \"%s\")";
      unsigned int   schemeStringLength   = sizeof (formatString) + sizeof (errorString) + 1;
      char *         schemeString         = NULL;


      // yes, the size is a two chars too big because of the format string but Zeroing it out takes care of this
      schemeString = (char *) calloc (schemeStringLength, sizeof (*schemeString));

      sprintf (schemeString, formatString, errorString);

      SendLogEntry         (WRANGLER_ERROR, errorString);
      scheme_eval_string   (schemeString, scheme_get_env (scheme_current_config ()));

      free (schemeString);
   }

   AllocConsole            ();
   SetConsoleCtrlHandler   (ConsoleHandlerRoutine, TRUE);

   // disable the 'X' so the console can't be closed
   EnableMenuItem (GetSystemMenu (GetConsoleWindow (), FALSE), SC_CLOSE, MF_BYCOMMAND | MF_GRAYED );

   return scheme_make_external_cptr (env, scheme_make_byte_string ("clips-env"));
}


Scheme_Object * clips_destroy_environment (int argc, Scheme_Object ** argv) {

   Scheme_Object * retVal = scheme_true;


   // standard signature
   argc;

   if (FALSE == DestroyEnvironment (SCHEME_CPTR_VAL (argv [0]))) {

	  retVal = scheme_false;
   }

   return retVal;
}


Scheme_Object * clips_load (int argc, Scheme_Object ** argv) {

   void *            env            = NULL;
   mzchar *          mzcharFileName = NULL;
   char *            fileName       = NULL;
   unsigned int      fileNameLength = 0;
   int               loadRetVal     = 0;
   Scheme_Object *   retVal         = NULL;


   MZ_GC_DECL_REG    (3);
   MZ_GC_VAR_IN_REG  (0, argv [0]);
   MZ_GC_VAR_IN_REG  (1, argv [1]);
   MZ_GC_VAR_IN_REG  (2, mzcharFileName);
   MZ_GC_REG         ();

   // standard signature
   argc;

   env            = SCHEME_CPTR_VAL          (argv [0]);
   fileNameLength = SCHEME_CHAR_STRLEN_VAL   (argv [1]) + 1;
   mzcharFileName = SCHEME_CHAR_STR_VAL      (argv [1]);

   fileName = (char *) calloc (fileNameLength, sizeof (*fileName));

   scheme_utf8_encode_all (mzcharFileName, fileNameLength, (unsigned char *) fileName);
   
   loadRetVal  = EnvLoad (env, fileName);
   retVal      = scheme_make_integer_value (loadRetVal);

   free (fileName);

   MZ_GC_UNREG ();

   return retVal;
}


Scheme_Object * clips_clear (int argc, Scheme_Object ** argv) {

   void * env = NULL;


   MZ_GC_DECL_REG    (1);
   MZ_GC_VAR_IN_REG  (0, argv [0]);
   MZ_GC_REG         ();

   // standard signature
   argc;

   env = SCHEME_CPTR_VAL (argv [0]);

   MZ_GC_UNREG ();

   EnvClear (env);

   return scheme_void;
}


Scheme_Object * clips_reset (int argc, Scheme_Object ** argv) {

   void * env = NULL;


   MZ_GC_DECL_REG    (1);
   MZ_GC_VAR_IN_REG  (0, argv [0]);
   MZ_GC_REG         ();

   // standard signature
   argc;

   env = SCHEME_CPTR_VAL (argv [0]);

   MZ_GC_UNREG ();

   EnvReset (env);

   return scheme_void;
}


Scheme_Object * clips_run (int argc, Scheme_Object ** argv) {

   void *            env      = NULL;
   long int          runLimit = -1;
   long int          runCount = 0;
   Scheme_Object *   retVal   = NULL;


   MZ_GC_DECL_REG    (2);
   MZ_GC_VAR_IN_REG  (0, argv [0]);
   MZ_GC_VAR_IN_REG  (1, argv [1]);
   MZ_GC_REG         ();

   // standard signature
   argc;

   env      = SCHEME_CPTR_VAL (argv [0]);
   runLimit = SCHEME_INT_VAL  (argv [1]);

   MZ_GC_UNREG ();

   runCount = EnvRun (env, runLimit);
   retVal   = scheme_make_integer_value (runCount);

   return retVal;
}


Scheme_Object * clips_assert_string (int argc, Scheme_Object ** argv) {

   void *            env                  = NULL;
   mzchar *          mzcharAssertString   = NULL;
   char *            assertString         = NULL;
   unsigned int      assertStringLength   = 0;


   MZ_GC_DECL_REG    (3);
   MZ_GC_VAR_IN_REG  (0, argv [0]);
   MZ_GC_VAR_IN_REG  (1, argv [1]);
   MZ_GC_VAR_IN_REG  (2, mzcharAssertString);
   MZ_GC_REG         ();

   // standard signature
   argc;

   env                  = SCHEME_CPTR_VAL          (argv [0]);
   assertStringLength   = SCHEME_CHAR_STRLEN_VAL   (argv [1]) + 1;
   mzcharAssertString   = SCHEME_CHAR_STR_VAL      (argv [1]);

   assertString = (char *) calloc (assertStringLength, sizeof (*assertString));

   scheme_utf8_encode_all (mzcharAssertString, assertStringLength, (unsigned char *) assertString);
   
   EnvAssertString (env, assertString);

   MZ_GC_UNREG ();

   free (assertString);

   return scheme_void;
}


Scheme_Object * clips_facts (int argc, Scheme_Object ** argv) {

   void *         env                     = NULL;
   mzchar *       mzcharLogicalNameString = NULL;
   char *         logicalNameString       = NULL;
   unsigned int   logicalNameStringLength = 0;
   void *         module                  = NULL;
   long int       start                   = 0;
   long int       end                     = 0;
   long int       max                     = 0;


   MZ_GC_DECL_REG    (7);
   MZ_GC_VAR_IN_REG  (0, argv [0]);
   MZ_GC_VAR_IN_REG  (1, argv [1]);
   MZ_GC_VAR_IN_REG  (2, argv [2]);
   MZ_GC_VAR_IN_REG  (3, argv [3]);
   MZ_GC_VAR_IN_REG  (4, argv [4]);
   MZ_GC_VAR_IN_REG  (5, argv [5]);
   MZ_GC_VAR_IN_REG  (6, mzcharLogicalNameString);
   MZ_GC_REG         ();

   // standard signature
   argc;

   env                     = SCHEME_CPTR_VAL          (argv [0]);
   logicalNameStringLength = SCHEME_CHAR_STRLEN_VAL   (argv [1]) + 1;
   mzcharLogicalNameString = SCHEME_CHAR_STR_VAL      (argv [1]);
   module                  = SCHEME_CPTR_VAL          (argv [2]);
   start                   = SCHEME_INT_VAL           (argv [3]);
   end                     = SCHEME_INT_VAL           (argv [4]);
   max                     = SCHEME_INT_VAL           (argv [5]);

   logicalNameString = (char *) calloc (logicalNameStringLength, sizeof (*logicalNameString));

   scheme_utf8_encode_all (   mzcharLogicalNameString,
                              logicalNameStringLength,
                              (unsigned char *) logicalNameString);
   
   EnvFacts (env, logicalNameString, module, start, end, max);

   MZ_GC_UNREG ();

   free (logicalNameString);

   return scheme_void;
}


Scheme_Object * clips_set_strategy (int argc, Scheme_Object ** argv) {

   void *   env               = NULL;
   int      strategy          = 0;
	int      setStrategyRetVal = DEPTH_STRATEGY;

   Scheme_Object * retVal = NULL;


   MZ_GC_DECL_REG    (2);
   MZ_GC_VAR_IN_REG  (0, argv [0]);
   MZ_GC_VAR_IN_REG  (1, argv [1]);
   MZ_GC_REG         ();

   // standard signature
   argc;

   env               = SCHEME_CPTR_VAL             (argv [0]);
   strategy          = SCHEME_INT_VAL              (argv [1]);
   setStrategyRetVal = EnvSetStrategy              (env, strategy);
   retVal            = scheme_make_integer_value   (setStrategyRetVal);

   MZ_GC_UNREG ();

	return retVal;
}


Scheme_Object * clips_watch (int argc, Scheme_Object ** argv) {

   void *         env                     = NULL;
   mzchar *       mzcharWatchItemString   = NULL;      
   char *         watchItemString         = 0;
   unsigned int   watchItemStringLength   = 0;
	int            watchRetVal             = 0;

   Scheme_Object * retVal = NULL;


   MZ_GC_DECL_REG    (2);
   MZ_GC_VAR_IN_REG  (0, argv [0]);
   MZ_GC_VAR_IN_REG  (1, argv [1]);
   MZ_GC_REG         ();

   // standard signature
   argc;

   env                     = SCHEME_CPTR_VAL          (argv [0]);
   watchItemStringLength   = SCHEME_CHAR_STRLEN_VAL   (argv [1]) + 1;
   mzcharWatchItemString   = SCHEME_CHAR_STR_VAL      (argv [1]);

   watchItemString = (char *) calloc (watchItemStringLength, sizeof (*watchItemString));

   scheme_utf8_encode_all (   mzcharWatchItemString,
                              watchItemStringLength,
                              (unsigned char *) watchItemString);

   watchRetVal = EnvWatch                    (env, watchItemString);
   retVal      = scheme_make_integer_value   (watchRetVal);

   MZ_GC_UNREG ();

   free (mzcharWatchItemString);

	return retVal;
}


Scheme_Object * clips_unwatch (int argc, Scheme_Object ** argv) {

   void *         env                     = NULL;
   mzchar *       mzcharWatchItemString   = NULL;      
   char *         watchItemString         = 0;
   unsigned int   watchItemStringLength   = 0;
	int            watchRetVal             = 0;

   Scheme_Object * retVal = NULL;


   MZ_GC_DECL_REG    (2);
   MZ_GC_VAR_IN_REG  (0, argv [0]);
   MZ_GC_VAR_IN_REG  (1, argv [1]);
   MZ_GC_REG         ();

   // standard signature
   argc;

   env                     = SCHEME_CPTR_VAL             (argv [0]);
   watchItemStringLength   = SCHEME_CHAR_STRLEN_VAL      (argv [1]) + 1;
   mzcharWatchItemString   = SCHEME_CHAR_STR_VAL         (argv [1]);

   watchItemString = (char *) calloc (watchItemStringLength, sizeof (*watchItemString));

   scheme_utf8_encode_all (   mzcharWatchItemString,
                              watchItemStringLength,
                              (unsigned char *) watchItemString);

   watchRetVal = EnvUnwatch                  (env, watchItemString);
   retVal      = scheme_make_integer_value   (watchRetVal);

   MZ_GC_UNREG ();

   free (mzcharWatchItemString);

	return retVal;
}


Scheme_Object * clips_add_scheme_function (int argc, Scheme_Object ** argv) {

   void *         env                        = NULL;
   mzchar *       mzcharFunctionNameString   = NULL;      
   char *         functionNameString         = 0;
   unsigned int   functionNameStringLength   = 0;
   int            defineFunctionRetVal       = 0;   
	
   Scheme_Object * retVal = NULL;


   MZ_GC_DECL_REG    (2);
   MZ_GC_VAR_IN_REG  (0, argv [0]);
   MZ_GC_VAR_IN_REG  (1, argv [1]);
   MZ_GC_REG         ();

   // standard signature
   argc;

   env                        = SCHEME_CPTR_VAL          (argv [0]);
   functionNameStringLength   = SCHEME_CHAR_STRLEN_VAL   (argv [1]) + 1;
   mzcharFunctionNameString   = SCHEME_CHAR_STR_VAL      (argv [1]);

   functionNameString = (char *) calloc (functionNameStringLength, sizeof (*functionNameString));

   scheme_utf8_encode_all (   mzcharFunctionNameString,
                              functionNameStringLength,
                              (unsigned char *) functionNameString);

   defineFunctionRetVal = EnvDefineFunction (   env,
                                                functionNameString,
                                                'u',
                                                PTIEF SingleReturnValueSchemeBridge,
                                                "SingleReturnValueSchemeBridge");

   retVal = scheme_make_integer_value (defineFunctionRetVal);

   MZ_GC_UNREG ();

   free (functionNameString);

	return retVal;
}


// ////////////////////
// /////////////////////////////////////////////
// win32 api extensions
// /////////////////////////////////////////////
// ////////////////////



Scheme_Object * win32_get_class_name_a (int argc, Scheme_Object ** argv) {

   HWND  hWnd                    = SchemeObjectToHwnd (argv [0]);
   char  windowClassName [1024];   // use 1024 since it's static and should be big enough for any window class name
   
   Scheme_Object * retVal = NULL;


   // standard signature
   argc;   

   memset (windowClassName, 0, sizeof (windowClassName));

   GetClassNameA (hWnd, windowClassName, sizeof (windowClassName));

   if (0 != strlen (windowClassName)) {

      retVal = scheme_make_utf8_string (windowClassName);
   }
   else {

      retVal = scheme_false;
   }

   return retVal;
}


Scheme_Object * win32_get_parent (int argc, Scheme_Object ** argv) {

   HWND     hWnd                                      = SchemeObjectToHwnd (argv [0]);
   LONG_PTR parentHwnd                                = (LONG_PTR) GetParent (hWnd);
   char     parentWindowHandleConversionBuffer  [65];


   // standard signature
   argc;

   memset (parentWindowHandleConversionBuffer, 0, sizeof (parentWindowHandleConversionBuffer));

   _i64toa (parentHwnd, parentWindowHandleConversionBuffer, 10);

   return scheme_make_utf8_string (parentWindowHandleConversionBuffer);
}


Scheme_Object * win32_get_window_thread_process_id (int argc, Scheme_Object ** argv) {

   HWND  hWnd        = SchemeObjectToHwnd (argv [0]);
   DWORD processId   = 0;

   Scheme_Object * retVal = NULL;


   // standard signature
   argc;


   GetWindowThreadProcessId (hWnd, &processId);


   if (0 != processId) {
      
      retVal = scheme_make_integer (processId);
   }
   else {

      retVal = scheme_false;
   }

   return retVal;
}


Scheme_Object * win32_get_window_long_ptr (int argc, Scheme_Object ** argv) {

   HWND     hWnd           = SchemeObjectToHwnd (argv [0]);
   LONG_PTR windowLongPtr  = GetWindowLongPtr   (hWnd, GWL_STYLE);

   Scheme_Object * schemeWindowLongPtr = NULL;


   // standard signature
   argc;

   if (0 == windowLongPtr) {

      schemeWindowLongPtr = scheme_false;
   }
   else {

      schemeWindowLongPtr = scheme_make_integer_value (windowLongPtr);
   }

   return schemeWindowLongPtr;
}


Scheme_Object * win32_get_window_text_length (int argc, Scheme_Object ** argv) {

   HWND hWnd = SchemeObjectToHwnd (argv [0]);

   Scheme_Object * schemeWindowTextLength = NULL;


   // standard signature
   argc;

   schemeWindowTextLength = scheme_make_integer_value (SendMessage ( hWnd,
                                                                     WM_GETTEXTLENGTH,
                                                                     0,
                                                                     0));

   return schemeWindowTextLength;
}


Scheme_Object * win32_get_window_text_a (int argc, Scheme_Object ** argv) {
   
   HWND     hWnd              = SchemeObjectToHwnd (argv [0]);
   DWORD    windowTextLength  = 0;
   char *   windowText        = NULL;

   Scheme_Object * schemeText = NULL;


   // standard signature
   argc;

   windowTextLength  = GetWindowTextLength (hWnd) + 1;
   windowText        = (char *) calloc (windowTextLength, sizeof (*windowText));

   GetWindowTextA (hWnd, windowText, windowTextLength);

   schemeText = scheme_make_utf8_string (windowText);

   free (windowText);

   return schemeText;
}


Scheme_Object * win32_get_window_text_w (int argc, Scheme_Object ** argv) {

   HWND           hWnd                 = SchemeObjectToHwnd (argv [0]);
   DWORD          windowTextCharCount  = 0;
   wchar_t *      windowWideText       = NULL;
   mzchar *       windowMzCharText     = NULL;
   unsigned long  windowWideTextSize   = 0;
   unsigned long  windowMzCharTextSize = 0;
   long           ucsEncodingLength    = 0;

   Scheme_Object * schemeText = NULL;
   

   // standard signature
   argc;

   windowTextCharCount  = GetWindowTextLength (hWnd) + 1;

   windowWideTextSize   = windowTextCharCount * sizeof (*windowWideText);
   windowMzCharTextSize = windowTextCharCount * sizeof (*windowMzCharText);

   windowWideText    = (wchar_t *)  calloc (windowTextCharCount, sizeof (*windowWideText));
   windowMzCharText  = (mzchar *)   calloc (windowTextCharCount, sizeof (*windowMzCharText));

   GetWindowTextW (hWnd, windowWideText, windowWideTextSize);

   scheme_utf16_to_ucs4 (  windowWideText,
                           0,
                           windowTextCharCount,
                           windowMzCharText,
                           windowMzCharTextSize,
                           &ucsEncodingLength,
                           1);

   schemeText = scheme_make_char_string (windowMzCharText);

   free (windowWideText);
   free (windowMzCharText);
   
   return schemeText;
}


Scheme_Object * win32_retrieve_window_text (int argc, Scheme_Object ** argv) {
   
   HWND           hWnd                 = SchemeObjectToHwnd (argv [0]);
   DWORD          windowTextCharCount  = 65536 + 1;
   wchar_t *      windowWideText       = NULL;
   mzchar *       windowMzCharText     = NULL;
   unsigned long  windowWideTextSize   = 0;
   unsigned long  windowMzCharTextSize = 0;
   long           ucsEncodingLength    = 0;

   Scheme_Object * schemeText = NULL;
   

   // standard signature
   argc;

   // windowTextCharCount is set to 65536 + 1 to account for the maximum number of
   // characters, plus terinator, that can be in a control. This is since sending a
   // WM_GETTEXTLENGTH message first doesn't work. It's possible for the control to
   // change it's text after getting the size. This often happens in installers that
   // rapidly show file decompression in a static control. We can't suspend the UI
   // thread since it's needed to respond to the messages. In short, there's no way
   // to syncronize this; so, we have to be defensive by having max buffer size and 
   // then calculate the string length before the call to 'scheme_utf16_to_ucs4'.

   windowWideTextSize   = windowTextCharCount * sizeof (*windowWideText);
   windowMzCharTextSize = windowTextCharCount * sizeof (*windowMzCharText);

   windowWideText    = (wchar_t *)  calloc (windowTextCharCount, sizeof (*windowWideText));
   windowMzCharText  = (mzchar *)   calloc (windowTextCharCount, sizeof (*windowMzCharText));

   windowTextCharCount = SendMessage ( hWnd,
                                       WM_GETTEXT,
                                       (WPARAM) windowTextCharCount,
                                       (LPARAM) windowWideText);

   scheme_utf16_to_ucs4 (  windowWideText,
                           0,
                           lstrlen (windowWideText),
                           windowMzCharText,
                           windowMzCharTextSize,
                           &ucsEncodingLength,
                           0);

   schemeText = scheme_make_char_string (windowMzCharText);

   free (windowWideText);
   free (windowMzCharText);
   
   return schemeText;
}
   

Scheme_Object * win32_get_window_size_and_location (int argc, Scheme_Object ** argv) {

   HWND  hWnd                 = SchemeObjectToHwnd (argv [0]);
   RECT  topLevelWindowRect;

   Scheme_Object * schemeWindowSizeValues       = NULL;
   Scheme_Object * schemeObjects          [4];


   // standard signature
   argc;

   if (0 == GetWindowRect (hWnd, &topLevelWindowRect)) {
   
      schemeObjects [0] = scheme_false;
      schemeObjects [1] = scheme_false;
      schemeObjects [2] = scheme_false;
      schemeObjects [3] = scheme_false;

      schemeWindowSizeValues = scheme_values (4, schemeObjects);
   }
   else {

      schemeObjects [0] = scheme_make_integer (topLevelWindowRect.bottom - topLevelWindowRect.top);   // height
      schemeObjects [1] = scheme_make_integer (topLevelWindowRect.right  - topLevelWindowRect.left);  // width
      schemeObjects [2] = scheme_make_integer (topLevelWindowRect.left);                              // x_upper_left_corner
      schemeObjects [3] = scheme_make_integer (topLevelWindowRect.top);                               // y_upper_left_corner

      schemeWindowSizeValues = scheme_values (4, schemeObjects);
   }

   return schemeWindowSizeValues;
}


Scheme_Object * win32_enumerate_parent_windows (int argc, Scheme_Object ** argv) {

   DWORD                            installProcessId              = 0;
   mzchar *                         mzcharEnumerationIdString     = NULL;
   unsigned long                    startingTime                  = 0;
   PARENT_WINDOW_ENUMERATION_INFO   parentWindowEnumerationInfo;


   MZ_GC_DECL_REG    (4);
   MZ_GC_VAR_IN_REG  (0, argv [0]);
   MZ_GC_VAR_IN_REG  (1, argv [1]);
   MZ_GC_VAR_IN_REG  (2, argv [2]);
   MZ_GC_VAR_IN_REG  (3, mzcharEnumerationIdString);
   MZ_GC_REG         ();


   // standard signature
   argc;

   installProcessId           = SCHEME_INT_VAL           (argv [0]);
   mzcharEnumerationIdString  = SCHEME_CHAR_STR_VAL      (argv [1]);

   scheme_get_unsigned_int_val (argv [2], &startingTime);

   parentWindowEnumerationInfo.installProcessId = installProcessId;

   // 'parentWindowEnumerationInfo.enumerationId' is a UUID
   // the length, including terminator is always 37
   scheme_utf8_encode_all (   mzcharEnumerationIdString,
                              37,
                              (unsigned char *) parentWindowEnumerationInfo.enumerationId);

   parentWindowEnumerationInfo.startingTime     = startingTime;

   EnumWindows (  EnumerateInstallParentWindowsCallback,
                  (LPARAM) &parentWindowEnumerationInfo);

   MZ_GC_UNREG ();

   return scheme_void;
}


Scheme_Object * win32_enumerate_child_windows (int argc, Scheme_Object ** argv) {

   HWND                          parentHwnd                    = SchemeObjectToHwnd (argv [0]);
   mzchar *                      mzcharEnumerationIdString     = NULL;
   unsigned int                  startingTime                  = 0;
   CHILD_WINDOW_ENUMERATION_INFO childWindowEnumerationInfo;


   MZ_GC_DECL_REG    (3);
   MZ_GC_VAR_IN_REG  (0, argv [1]);
   MZ_GC_VAR_IN_REG  (1, argv [2]);
   MZ_GC_VAR_IN_REG  (2, mzcharEnumerationIdString);
   MZ_GC_REG         ();


   // standard signature
   argc;

   mzcharEnumerationIdString  = SCHEME_CHAR_STR_VAL      (argv [1]);
   startingTime               = SCHEME_INT_VAL           (argv [2]);

   childWindowEnumerationInfo.parentHwnd     = parentHwnd;

   // 'childWindowEnumerationInfo.enumerationId' is a UUID
   // the length, including terminator is always 37
   scheme_utf8_encode_all (   mzcharEnumerationIdString,
                              37,
                              (unsigned char *) childWindowEnumerationInfo.enumerationId);

   childWindowEnumerationInfo.startingTime  = startingTime;

   EnumChildWindows (   parentHwnd,
                        EnumerateInstallChildWindowsCallback,
                        (LPARAM) &childWindowEnumerationInfo);

   MZ_GC_UNREG ();

   return scheme_void;
}


Scheme_Object * win32_check_for_large_ascii_window_text (int argc, Scheme_Object ** argv) {

   HWND  hWnd           = SchemeObjectToHwnd (argv [0]);
   BOOL  asciiAvailable = FALSE;

   Scheme_Object * retVal = scheme_false;


   // standard signature
   argc;

   SendMessage    (hWnd, EM_SETSEL, 0, -1);
   SendMessage    (hWnd, WM_COPY, 0, 0);
   OpenClipboard  (NULL);

   asciiAvailable = IsClipboardFormatAvailable (CF_TEXT);

   if (asciiAvailable) {

      retVal = scheme_true;
   }

   EmptyClipboard ();
   CloseClipboard ();

   return retVal;
}


Scheme_Object * win32_check_for_large_unicode_window_text (int argc, Scheme_Object ** argv) {

   HWND  hWnd              = SchemeObjectToHwnd (argv [0]);
   BOOL  unicodeAvailable  = FALSE;

   Scheme_Object * retVal = scheme_false;


   // standard signature
   argc;

   SendMessage    (hWnd, EM_SETSEL, 0, -1);
   SendMessage    (hWnd, WM_COPY, 0, 0);
   OpenClipboard  (NULL);

   unicodeAvailable = IsClipboardFormatAvailable (CF_UNICODETEXT);

   if (unicodeAvailable) {

      retVal = scheme_true;
   }

   EmptyClipboard ();
   CloseClipboard ();

   return retVal;
}


Scheme_Object * win32_retrieve_large_ascii_window_text (int argc, Scheme_Object ** argv) {

   HANDLE hText = NULL;

   Scheme_Object * retVal = scheme_false;


   // standard signature
   argc;

   OpenClipboard (NULL);

   hText = GetClipboardData (CF_TEXT);

   if (NULL != hText) {

      char * pLargeText = (char *) GlobalLock (hText);


      if (NULL != pLargeText) {

         LONG_PTR       windowHandle               = (LONG_PTR) SchemeObjectToHwnd (argv [0]);
         char *         schemeString               = NULL;
         char           hwndConversionBuffer [65];
         char           schemeFormatString   []    = "(add-large-text %s %s)";
         unsigned int   schemeStringSize           =  sizeof (schemeFormatString)   +
                                                      sizeof (hwndConversionBuffer) +
                                                      strlen (pLargeText) + 1;


         schemeString = (char *) malloc (schemeStringSize);

         sprintf (   schemeString,
                     schemeFormatString,
                     _i64toa (windowHandle, hwndConversionBuffer, 10),
                     pLargeText);

         scheme_eval_string (schemeString, scheme_get_env (scheme_current_config ()));

         free (schemeString);

         GlobalUnlock (hText);

         retVal = scheme_true;
      }
   }

   EmptyClipboard ();
   CloseClipboard ();

   return retVal;
}


Scheme_Object * win32_retrieve_large_unicode_window_text (int argc, Scheme_Object ** argv) {

   HANDLE hText = NULL;

   Scheme_Object * retVal = scheme_false;


   // standard signature
   argc;

   OpenClipboard (NULL);

   hText = GetClipboardData (CF_UNICODETEXT);

   if (NULL != hText) {

      wchar_t * pLargeWideText = (wchar_t *) GlobalLock (hText);


      if (NULL != pLargeWideText ) {

         LONG_PTR       windowHandle               = (LONG_PTR) SchemeObjectToHwnd (argv [0]);
         char *         schemeString               = NULL;
         char *         mbcsConversionBuffer       = NULL;
         char           hwndConversionBuffer [65];
         char           schemeFormatString   []    = "(add-large-text %s %s)";
         unsigned int   mbcsConversionBufferLength =  wcslen (pLargeWideText) + 1;
         unsigned int   schemeStringLength         =  sizeof (schemeFormatString)   +
                                                      sizeof (hwndConversionBuffer) +
                                                      mbcsConversionBufferLength;

         schemeString         = (char *) calloc (schemeStringLength,          sizeof (*schemeString));
         mbcsConversionBuffer = (char *) calloc (mbcsConversionBufferLength,  sizeof (*mbcsConversionBuffer));

         memset (hwndConversionBuffer, 0, sizeof (hwndConversionBuffer));

         wcstombs (mbcsConversionBuffer, pLargeWideText, mbcsConversionBufferLength);

         sprintf (   schemeString,
                     schemeFormatString,
                     _i64toa (windowHandle, hwndConversionBuffer, 10),
                     mbcsConversionBuffer);

         scheme_eval_string (schemeString, scheme_get_env (scheme_current_config ()));

         free           (mbcsConversionBuffer);
         free           (schemeString);
         GlobalUnlock   (hText);

         retVal = scheme_true;
      }
   }

   EmptyClipboard ();
   CloseClipboard ();

   return retVal;
}


Scheme_Object * win32_get_top_window (int argc, Scheme_Object ** argv) {

   HWND     parenthWnd                             = SchemeObjectToHwnd (argv [0]);
   LONG_PTR topHwnd                                = (LONG_PTR) GetTopWindow (parenthWnd);
   char     topWindowHandleConversionBuffer  [65];


   // standard signature
   argc;

   memset (topWindowHandleConversionBuffer, 0, sizeof (topWindowHandleConversionBuffer));

   _i64toa (topHwnd, topWindowHandleConversionBuffer, 10);

   return scheme_make_utf8_string (topWindowHandleConversionBuffer);
}


Scheme_Object * win32_click_button (int argc, Scheme_Object ** argv) {
	
	HWND hWnd = SchemeObjectToHwnd (argv [0]);


   // standard signature
   argc;

	PostMessage (hWnd, BM_CLICK, 0, 0);

   return scheme_void;
}


Scheme_Object * win32_post_message (int argc, Scheme_Object ** argv) {
	
	HWND  hWnd     = SchemeObjectToHwnd (argv [0]);
   int   message  = 0;


   MZ_GC_DECL_REG    (1);
   MZ_GC_VAR_IN_REG  (0, argv [1]);
   MZ_GC_REG         ();

   // standard signature
   argc;

   message = SCHEME_INT_VAL (argv [1]);

   MZ_GC_UNREG ();

	PostMessage (hWnd, message, 0, 0);

   return scheme_void;
}


Scheme_Object * win32_set_active_window (int argc, Scheme_Object ** argv) {
	
	HWND hWnd = SchemeObjectToHwnd (argv [0]);


   // standard signature
   argc;

	SetActiveWindow (hWnd);

   return scheme_void;
}




//------------------
//------marked------
//------------------
// ////////////////////
// /////////////////////////////////////////////
// c-runtime scheme extensions
// /////////////////////////////////////////////
// ////////////////////
Scheme_Object * crt_time (int argc, Scheme_Object ** argv) {

   // standard signature
   argc;
   argv;


   return scheme_make_integer_value ((unsigned int) time (NULL));
}
// ////////////////////
// /////////////////////////////////////////////
// c-runtime scheme extensions
// /////////////////////////////////////////////
// ////////////////////
//------------------
//------marked------
//------------------



// ////////////////////
// /////////////////////////////////////////////
// wrangler scheme extensions
// /////////////////////////////////////////////
// ////////////////////



Scheme_Object * wrangler_initialize_common_utilities (int argc, Scheme_Object ** argv) {
   
   // standard signature
   argc;
   argv;

   if (FALSE == InitializeCommonUtilities ()) {

         MessageBoxA (NULL, "Failed to initialize.", "Common Utility Error", MB_OK|MB_TOPMOST);
   }

   return scheme_void;
}


Scheme_Object * wrangler_launch_install (int argc, Scheme_Object ** argv) {

   char * exeName = NULL;

   Scheme_Object * retVal = scheme_false;

   MZ_GC_DECL_REG    (1);
   MZ_GC_VAR_IN_REG  (0, argv [0]);
   MZ_GC_REG         ();

   // standard signature
   argc;

   if (!SCHEME_CHAR_STRINGP (argv [0])) {

      SendLogEntry (WRANGLER_ERROR, "first parameter is not a string (launch_install)");

      goto EXIT;
   }

   exeName = ExtractCharString (argv [0]);

   if (LaunchInstall (exeName)) {

      retVal = scheme_true;
   }

   MZ_GC_UNREG ();

   FreeExtractedCharString (exeName);

   EXIT:

   return retVal;
}


#define PROCESS_SLOT_COUNT 16384
Scheme_Object * wrangler_process_exists (int argc, Scheme_Object ** argv) {
	
   DWORD processes      [PROCESS_SLOT_COUNT];
   DWORD processesSize                       = sizeof (processes);
   DWORD bytesReturned                       = 0;
   DWORD targetProcess                       = 0;
   
   Scheme_Object * processExists = scheme_false;


   MZ_GC_DECL_REG    (1);
   MZ_GC_VAR_IN_REG  (0, argv [0]);
   MZ_GC_REG         ();

   // standard signature
   argc;

   targetProcess = SCHEME_INT_VAL (argv [0]);

   MZ_GC_UNREG ();

   memset (processes, 0, processesSize);

   if (EnumProcesses (processes, processesSize, &bytesReturned)) {
   
      int i = 0;


      for (i = 0; i < PROCESS_SLOT_COUNT; ++i) {

         if (targetProcess == processes [i]) {

            processExists = scheme_true;

            break;
         }
      }
   }

   return processExists;
}


Scheme_Object * wrangler_wait_for_process_two_minutes (int argc, Scheme_Object ** argv) {
	
   DWORD    targetProcess  = 0;
   DWORD    waitResult     = 0;
   HANDLE   processHandle  = NULL;
   
   Scheme_Object * processExited = scheme_true;


   MZ_GC_DECL_REG    (1);
   MZ_GC_VAR_IN_REG  (0, argv [0]);
   MZ_GC_REG         ();

   // standard signature
   argc;

   targetProcess = SCHEME_INT_VAL (argv [0]);

   MZ_GC_UNREG ();

   processHandle = OpenProcess (SYNCHRONIZE, FALSE, targetProcess);

   if (NULL == processHandle) {
      
      DWORD lastError = 0;


      if (0x57 != lastError) {
         
         processExited = scheme_false;
      }
   }
   else {

      waitResult = WaitForSingleObject (processHandle, 120000);
   
      if (WAIT_TIMEOUT == waitResult) {
   
         processExited = scheme_false;
      }
   }

   return processExited;
}



// ////////////////////
// /////////////////////////////////////////////
// scheme extension exports
// /////////////////////////////////////////////
// ////////////////////



Scheme_Object * scheme_reload (Scheme_Env * global_env);

Scheme_Object * scheme_initialize (Scheme_Env * global_env) {

   // actions for initialize are contained in reload
   return scheme_reload (global_env);
}


Scheme_Object * scheme_reload (Scheme_Env * global_env) {

   Scheme_Env *      moduleEnv         = NULL;
   Scheme_Object *   extensionFunction = NULL;


   MZ_GC_DECL_REG    (2);
   MZ_GC_VAR_IN_REG  (0, global_env);
   MZ_GC_VAR_IN_REG  (1, moduleEnv);
   MZ_GC_REG         ();


   moduleEnv = scheme_primitive_module (scheme_intern_symbol ("wrangler"), global_env);

   extensionFunction = scheme_make_prim_w_arity (clips_create_environment, "clips-create-environment", 0, 0);
   scheme_add_global ("clips-create-environment", extensionFunction, moduleEnv);
   extensionFunction = scheme_make_prim_w_arity (clips_destroy_environment, "clips-destroy-environment", 1, 1);
   scheme_add_global ("clips-destroy-environment", extensionFunction, moduleEnv);
   extensionFunction = scheme_make_prim_w_arity (clips_load, "clips-load", 2, 2);
   scheme_add_global ("clips-load", extensionFunction, moduleEnv);
   extensionFunction = scheme_make_prim_w_arity (clips_clear, "clips-clear", 1, 1);
   scheme_add_global ("clips-clear", extensionFunction, moduleEnv);
   extensionFunction = scheme_make_prim_w_arity (clips_reset, "clips-reset", 1, 1);
   scheme_add_global ("clips-reset", extensionFunction, moduleEnv);
   extensionFunction = scheme_make_prim_w_arity (clips_run, "clips-run", 2, 2);
   scheme_add_global ("clips-run", extensionFunction, moduleEnv);
   extensionFunction = scheme_make_prim_w_arity (clips_assert_string, "clips-assert-string", 2, 2);
   scheme_add_global ("clips-assert-string", extensionFunction, moduleEnv);
   extensionFunction = scheme_make_prim_w_arity (clips_facts, "clips-facts", 6, 6);
   scheme_add_global ("clips-facts", extensionFunction, moduleEnv);
   extensionFunction = scheme_make_prim_w_arity (clips_set_strategy, "clips-set-strategy", 2, 2);
   scheme_add_global ("clips-set-strategy", extensionFunction, moduleEnv);
   extensionFunction = scheme_make_prim_w_arity (clips_watch, "clips-watch", 2, 2);
   scheme_add_global ("clips-watch", extensionFunction, moduleEnv);
   extensionFunction = scheme_make_prim_w_arity (clips_unwatch, "clips-unwatch", 2, 2);
   scheme_add_global ("clips-unwatch", extensionFunction, moduleEnv);
   extensionFunction = scheme_make_prim_w_arity (clips_add_scheme_function, "clips-add-scheme-function", 2, 2);
   scheme_add_global ("clips-add-scheme-function", extensionFunction, moduleEnv);
   extensionFunction = scheme_make_prim_w_arity (win32_get_class_name_a, "win32-get-class-name-a", 1, 1);
   scheme_add_global ("win32-get-class-name-a", extensionFunction, moduleEnv);
   extensionFunction = scheme_make_prim_w_arity (win32_get_parent, "win32-get-parent", 1, 1);
   scheme_add_global ("win32-get-parent", extensionFunction, moduleEnv);
   extensionFunction = scheme_make_prim_w_arity (win32_get_window_thread_process_id, "win32-get-window-thread-process-id", 1, 1);
   scheme_add_global ("win32-get-window-thread-process-id", extensionFunction, moduleEnv);
   extensionFunction = scheme_make_prim_w_arity (win32_get_window_long_ptr, "win32-get-window-long-ptr", 1, 1);
   scheme_add_global ("win32-get-window-long-ptr", extensionFunction, moduleEnv);
   extensionFunction = scheme_make_prim_w_arity (win32_get_window_text_length, "win32-get-window-text-length", 1, 1);
   scheme_add_global ("win32-get-window-text-length", extensionFunction, moduleEnv);
   extensionFunction = scheme_make_prim_w_arity (win32_get_window_text_a, "win32-get-window-text-a", 1, 1);
   scheme_add_global ("win32-get-window-text-a", extensionFunction, moduleEnv);
   extensionFunction = scheme_make_prim_w_arity (win32_get_window_text_w, "win32-get-window-text-w", 1, 1);
   scheme_add_global ("win32-get-window-text-w", extensionFunction, moduleEnv);
   extensionFunction = scheme_make_prim_w_arity (win32_retrieve_window_text, "win32-retrieve-window-text", 1, 1);
   scheme_add_global ("win32-retrieve-window-text", extensionFunction, moduleEnv);
   extensionFunction = scheme_make_prim_w_arity (win32_get_window_size_and_location, "win32-get-window-size-and-location", 1, 1);
   scheme_add_global ("win32-get-window-size-and-location", extensionFunction, moduleEnv);
   extensionFunction = scheme_make_prim_w_arity (win32_enumerate_parent_windows, "win32-enumerate-parent-windows", 3, 3);
   scheme_add_global ("win32-enumerate-parent-windows", extensionFunction, moduleEnv);
   extensionFunction = scheme_make_prim_w_arity (win32_enumerate_child_windows, "win32-enumerate-child-windows", 3, 3);
   scheme_add_global ("win32-enumerate-child-windows", extensionFunction, moduleEnv);
   extensionFunction = scheme_make_prim_w_arity (win32_check_for_large_ascii_window_text, "win32-check-for-large-ascii-window-text", 1, 1);
   scheme_add_global ("win32-check-for-large-ascii-window-text", extensionFunction, moduleEnv);
   extensionFunction = scheme_make_prim_w_arity (win32_check_for_large_unicode_window_text, "win32-check-for-large-unicode-window-text", 1, 1);
   scheme_add_global ("win32-check-for-large-unicode-window-text", extensionFunction, moduleEnv);
   extensionFunction = scheme_make_prim_w_arity (win32_retrieve_large_ascii_window_text, "win32-retrieve-large-ascii-window-text", 1, 1);
   scheme_add_global ("win32-retrieve-large-ascii-window-text", extensionFunction, moduleEnv);
   extensionFunction = scheme_make_prim_w_arity (win32_retrieve_large_unicode_window_text, "win32-retrieve-large-unicode-window-text", 1, 1);
   scheme_add_global ("win32-retrieve-large-unicode-window-text", extensionFunction, moduleEnv);
   extensionFunction = scheme_make_prim_w_arity (win32_get_top_window, "win32-get-top-window", 1, 1);
   scheme_add_global ("win32-get-top-window", extensionFunction, moduleEnv);
   extensionFunction = scheme_make_prim_w_arity (win32_post_message, "win32-post-message", 2, 2);
   scheme_add_global ("win32-post-message", extensionFunction, moduleEnv);
   extensionFunction = scheme_make_prim_w_arity (win32_set_active_window, "win32-set-active-window", 1, 1);
   scheme_add_global ("win32-set-active-window", extensionFunction, moduleEnv);

   //------------------
   //------marked------
   //------------------
   extensionFunction = scheme_make_prim_w_arity (crt_time, "crt-time", 0, 0);
   scheme_add_global ("crt-time", extensionFunction, moduleEnv);
   //------------------
   //------marked------
   //------------------

   extensionFunction = scheme_make_prim_w_arity (wrangler_launch_install, "wrangler-launch-install", 1, 1);
   scheme_add_global ("wrangler-launch-install", extensionFunction, moduleEnv);
   extensionFunction = scheme_make_prim_w_arity (wrangler_initialize_common_utilities, "wrangler-initialize-common-utilities", 0, 0);
   scheme_add_global ("wrangler-initialize-common-utilities", extensionFunction, moduleEnv);
   extensionFunction = scheme_make_prim_w_arity (wrangler_process_exists, "wrangler-process-exists?", 1, 1);
   scheme_add_global ("wrangler-process-exists?", extensionFunction, moduleEnv);
   extensionFunction = scheme_make_prim_w_arity (wrangler_wait_for_process_two_minutes, "wrangler-wait-for-process-two-minutes", 1, 1);
   scheme_add_global ("wrangler-wait-for-process-two-minutes", extensionFunction, moduleEnv);

   scheme_finish_primitive_module (moduleEnv);

   MZ_GC_UNREG ();

   return scheme_void;
}


Scheme_Object * scheme_module_name (void) {

   return scheme_intern_symbol ("wrangler");
}
