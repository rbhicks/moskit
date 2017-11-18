#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <psapi.h>

#include <common-utility.h>


int main (int argc, char * argv []) {

   DWORD                         processId         = (DWORD) (atoi (argv [1]));
   HANDLE                        hProcess;
   WRANGLER_INSTALL_EVENT_TYPE   installEventType  = WRANGLER_EXIT_PROCESS;


   if (FALSE == InitializeCommonUtilities ()) {

      MessageBoxA (NULL, "Failed to initialize.", "Common Utility Error", MB_OK|MB_TOPMOST);
   }
   else {
   
      hProcess = OpenProcess (PROCESS_ALL_ACCESS, FALSE, processId);
   
      if (NULL != hProcess) {
   
         char     processIdConversionBuffer  [33];
         wchar_t  watchedProcessCommandLine  [MAX_PATH + 1];
   
   
         if (GetModuleFileNameEx (  hProcess,
                                    NULL,
                                    watchedProcessCommandLine,
                                    sizeof (watchedProcessCommandLine))) {

            wprintf (L"Watching: %s\n%d", watchedProcessCommandLine, processId);
         }
         else {
   
            printf ( "Watching: %d\n(GetModuleFileNameEx failed with: %d)",
                     processId,
                     GetLastError ());
         }

         WaitForSingleObject  (hProcess, 600000);
         CloseHandle          (hProcess);

         _itoa (processId, processIdConversionBuffer, 10);

         SendEventData (   installEventType,
                           processIdConversionBuffer,
                           strlen (processIdConversionBuffer));
      }
      else {
   
         printf ( "Watching %d\n(OpenProcess failed with: %d\n",
                  processId,
                  GetLastError ());
      }
   
      DeInitializeCommonUtilities ();
   }

   return 0;
}
