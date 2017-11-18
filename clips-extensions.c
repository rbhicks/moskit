// CLIPS extension functions

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <time.h>

#include "extension-support.h"
#include "clips.h"


// normall, we'd run this through the scheme layer (specifically: 'current-seconds'), but
// it returns a 'bignum' which is converted to a double via 'SingleReturnValueSchemeBridge'
// the rest of the processing uses a DWORD. It's cleaner just to provide this as a
// standalone native clips extension
unsigned long SecondsSinceEpoch (void * theEnv) {

   // std signature
   theEnv;
   
   return (unsigned long) time (NULL);
}


// generalized single return value scheme bridge via scheme_eval_string
void SingleReturnValueSchemeBridge (void * theEnv, DATA_OBJECT_PTR returnValuePtr) {

   int               lexemeSize           = 0;
   int               schemeStringLength   = 0;
   int               reallocSize          = 0;
   int               argCount             = EnvRtnArgCount (theEnv);
   int               i                    = 1;
   char *            lexeme               = NULL;
   char *            schemeString         = NULL;
   char *            tempArgumentString   = NULL;
   char *            newMemoryPtr         = NULL;
   char *            calledFunctionName   = NULL;
   BOOL              fromSchemeCall       = FALSE;
   BOOL              fromSchemeEvalString = FALSE;
   Scheme_Config *   schemeConfig         = NULL;
   Scheme_Env *      schemeEnv            = NULL;
   DATA_OBJECT       dataObject;

   Scheme_Object *   schemeReturnValue    = NULL;
   

   MZ_GC_DECL_REG    (3);
   MZ_GC_VAR_IN_REG  (0, schemeReturnValue);
   MZ_GC_VAR_IN_REG  (1, schemeConfig);
   MZ_GC_VAR_IN_REG  (2, schemeEnv);
   MZ_GC_REG         ();


   // This is an internal CLIPS call. It's not part of the API but it's the same way they get the
   // called function name is stored/retrieved
   calledFunctionName = ValueToString (ExpressionFunctionCallName (EvaluationData (theEnv)->CurrentExpression));

   if       (!strcmp (calledFunctionName, "scheme-call")) {

      fromSchemeCall = TRUE;
   }
   else if  (!strcmp (calledFunctionName, "scheme-eval-string")) {
   
      fromSchemeEvalString = TRUE;   
   }

   if (TRUE == fromSchemeEvalString) {

      if (-1 == EnvArgCountCheck (theEnv, "scheme-eval-string", EXACTLY, 1)) {
         
         EnvSetpType    (theEnv, returnValuePtr, SYMBOL);
         EnvSetpValue   (theEnv, returnValuePtr, EnvFalseSymbol (theEnv));
   
         return;
      }

      if (!EnvArgTypeCheck (theEnv, "scheme-eval-string", 1, STRING, returnValuePtr)) {
         
         EnvSetpType    (theEnv, returnValuePtr, SYMBOL);
         EnvSetpValue   (theEnv, returnValuePtr, EnvFalseSymbol (theEnv));
   
         return;
      }
   
      lexeme               = EnvRtnLexeme (theEnv, 1);
      lexemeSize           = strlen (lexeme);
      schemeStringLength   = lexemeSize + 1;
      schemeString         = (char *) calloc (schemeStringLength, sizeof (*schemeString));
   
      memcpy (schemeString, lexeme, lexemeSize);
   }
   else {

      if (FALSE == fromSchemeCall) {

         schemeStringLength += strlen (calledFunctionName);
      }

      schemeStringLength += 1;  // null terminator
      schemeStringLength += 1;  // opening paren
      schemeStringLength += 1;  // space after function name
      schemeStringLength += 1;  // closing paren
   
      schemeString = (char *) calloc (schemeStringLength, sizeof (*schemeString));
   
      strcat (schemeString, "(");

      if (FALSE == fromSchemeCall) {

         strcat (schemeString, calledFunctionName);
         strcat (schemeString, " ");
      }
   
      for (i; i <= argCount; ++i) {
   
         EnvRtnUnknown (theEnv, i, &dataObject);
   
         tempArgumentString   = ClipsValueToMappedSchemeLiteralString (&dataObject);
         reallocSize          = schemeStringLength + strlen (tempArgumentString) + 2; // 2 for the null and the space between args
   
         schemeString         = (char *) realloc (schemeString, reallocSize);
         newMemoryPtr         = schemeString + strlen (schemeString);
   
         memset (newMemoryPtr, 0, (reallocSize - schemeStringLength));
   
         strcat (schemeString, tempArgumentString);
         strcat (schemeString, " ");
   
         FreeMappedSchemeLiteralString (tempArgumentString);
   
         schemeStringLength = reallocSize;
      }
   
      strcat (schemeString, ")");
   }

   schemeConfig      = scheme_current_config ();
   schemeEnv         = scheme_get_env        (schemeConfig);
   schemeReturnValue = scheme_eval_string    (schemeString, schemeEnv);

   free (schemeString);

   PopulateClipsDataObectFromSchemeObject (theEnv,
                                           returnValuePtr,
                                           schemeReturnValue);

   MZ_GC_UNREG ();
}


// multiple return value scheme bridge via scheme_eval_string_multi

//
// specific defined extensions for certain* functions
// 
// * This just means that they've already been made and 
//   have well-defined semantics. The general bridges above
//   can be used for anything at anytime which is good for
//   implementing new things quickly. Ideally, after something
//   has been used by the general versions often enougt it'll
//   been moved into the 'specifically defined' section
//


