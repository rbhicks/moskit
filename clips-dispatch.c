#include "clips.h"
#include "clips-dispatch.h"


extern int g_finished;


void * Clips_CreateEnvironment () {

   return CreateEnvironment ();
}


bool Clips_DestroyEnvironment (void * env) {

   bool retVal = true;


   if (FALSE == DestroyEnvironment (env)) {

	  retVal = false;
   }

   return retVal;
}


int Clips_Load (void * env, char * fileName) {

   return EnvLoad (env, fileName);
}


void Clips_Clear (void * env) {

   EnvClear (env);
}


void Clips_Reset (void * env) {

   EnvReset (env);
}


long int Clips_Run (void * env, long int runLimit) {

   return EnvRun (env, runLimit);
}


void Clips_AssertString (void * env, char * assertString) {

   EnvAssertString (env, assertString);
}


void Clips_Facts (	void *	env,
							char * 	logicalName,
							void * 	theModule,
							long 		start,
							long 		end,
							long 		max) {

	EnvFacts (env, logicalName, theModule, start, end, max);
}


int Clips_SetStrategy (void * env, STRATEGY strategy) {

	STRATEGY retVal = DEPTH_STRATEGY;


	switch (strategy) {

		case DEPTH:
			retVal = EnvSetStrategy (env, DEPTH_STRATEGY);
		case BREADTH:
			retVal = EnvSetStrategy (env, BREADTH_STRATEGY);
		case LEX:
			retVal = EnvSetStrategy (env, LEX_STRATEGY);
		case MEA:
			retVal = EnvSetStrategy (env, MEA_STRATEGY);
		case COMPLEXITY:
			retVal = EnvSetStrategy (env, COMPLEXITY_STRATEGY);
		case SIMPLICITY:
			retVal = EnvSetStrategy (env, SIMPLICITY_STRATEGY);
		case RANDOM:
			retVal = EnvSetStrategy (env, RANDOM_STRATEGY);
		default:
			retVal = EnvSetStrategy (env, DEPTH_STRATEGY);
	}

	return retVal;
}


bool Clips_IsFinished () {

   return g_finished;
}
