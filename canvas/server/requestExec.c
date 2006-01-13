$commentbegin$
/*	$LAAS$ */
/* 
 * Copyright (c) 1993-2005 LAAS/CNRS
 * All rights reserved.
 *
 * Redistribution and use  in source  and binary  forms,  with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *   1. Redistributions of  source  code must retain the  above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice,  this list of  conditions and the following disclaimer in
 *      the  documentation  and/or  other   materials provided  with  the
 *      distribution.
 *
 * THIS  SOFTWARE IS PROVIDED BY  THE  COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND  ANY  EXPRESS OR IMPLIED  WARRANTIES,  INCLUDING,  BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES  OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR  PURPOSE ARE DISCLAIMED. IN  NO EVENT SHALL THE COPYRIGHT
 * HOLDERS OR      CONTRIBUTORS  BE LIABLE FOR   ANY    DIRECT, INDIRECT,
 * INCIDENTAL,  SPECIAL,  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF  SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN  CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE   OF THIS SOFTWARE, EVEN   IF ADVISED OF   THE POSSIBILITY OF SUCH
 * DAMAGE.
 */
$commentend$

/*****************************************************************************
 *
 *  $module$Cntrl$request$ - Execution request
 */

#if ($controlFuncFlag$ /* control func */)
  extern int $cControlFunc$ ();
#endif 

static void $module$Cntrl$request$ (SERV_ID servId, int rqstId)

{
  $tabCompatibilityDeclare$
  int activity;
  int i;
  int *compatibilityTab = $module$$request$Compatibility;
#if ($inputFlag$ && $controlFuncFlag$ /* Inputs + control func */)
  $inputDeclare$
#endif

  /*--------------------------------------------------------------
   * Control init done and exec tasks status 
   */
#if ($initRqst$ /* not init rqst */)
  if (!controlExecTaskStatusAndInitRqst(servId, rqstId))
    return;  
#endif
 
  /*--------------------------------------------------------------
   * Activity allocation
   */
  activity = allocActivity(rqstId, $requestNum$, $execTaskNum$, $reentrantFlag$, servId);
  if (activity == -1) return;

  /*--------------------------------------------------------------
   * Call control func
   */
#if ($controlFuncFlag$ /* control func */)
  {
    STATUS status;
    int bilan;

#if ($inputFlag$  /* Input */)
    /* Get inputs */
    if (csServRqstParamsGet (servId, rqstId, (void *) $inputNamePtr$, 
			     $inputSize$, (FUNCPTR) NULL) != OK)
      $module$ReplyAndSuspend (servId, rqstId, TRUE);
    status = $cControlFunc$ ($inputNamePtr$, &bilan);
#else
    status = $cControlFunc$ (&bilan);
#endif

    /* Control not OK: stop there */
    if (status != OK) {
      freeActivity(activity);
      if (bilan == OK)
	bilan = S_$module$_stdGenoM_CONTROL_CODEL_ERROR;
      if (csServReplySend (servId, rqstId, FINAL_REPLY, bilan, 
			   (void *) NULL, 0, (FUNCPTR) NULL) != OK)
	$module$CntrlTaskSuspend (TRUE);
      return;
    }
  }
#endif

 /*-------------------------------------------------------------
  * Record inputs
  *       XXXXXXXX NON C'EST TROP TOT -> INTERROMPRE LES ACTIVITES AVANT !
  */
#if ($inputFlag$ /* Input */)

  /* Get inputs address */
#if ($reentrantFlag$ /* multiple request  */)
  ACTIVITY_INPUT_ID(activity) = &($module$DataStrId->$request$Input)[activity];
#else
  ACTIVITY_INPUT_ID(activity) = $inputRefPtr$;
#endif                
  ACTIVITY_INPUT_SIZE(activity) = $inputSize$;

  /* Record inputs */
#if ($controlFuncFlag$ /* control func */)
  memcpy ((void *) ACTIVITY_INPUT_ID(activity), (void *) $inputNamePtr$, 
	  ACTIVITY_INPUT_SIZE(activity));
#else
  if (csServRqstParamsGet (servId, rqstId, ACTIVITY_INPUT_ID(activity), 
			   $inputSize$, (FUNCPTR) NULL) != OK)
    $module$ReplyAndSuspend (servId, rqstId, TRUE);
#endif
#endif          
  
 /*-------------------------------------------------------------
  * Get outputs address
  */
#if ($outputFlag$ /* Output */)
#if ($reentrantFlag$ /* Multiple request */)
  ACTIVITY_OUTPUT_ID(activity) = 
    &($module$DataStrId->$request$Output)[activity];
#else
  ACTIVITY_OUTPUT_ID(activity) = $outputRefPtr$;
#endif            
  ACTIVITY_OUTPUT_SIZE(activity) = $outputSize$;
#endif          

  moduleEventCntrl.eventType = STATE_START_EVENT;
  moduleEventCntrl.activityNum = activity;
  moduleEventCntrl.activityState = INIT;
  moduleEventCntrl.rqstType = $requestNum$;
  moduleEventCntrl.taskNum = $execTaskNum$;
  sendModuleEvent(&moduleEventCntrl);

  /*-------------------------------------------------------------
   * Interruption of the incompatible activities
   */
  for (i = 0; i < MAX_ACTIVITIES; i++) 
    if (i != activity && ACTIVITY_STATUS(i) != ETHER 
	&& ! compatibilityTab[ACTIVITY_RQST_TYPE(i)]) 
      
      /* Record remaining incompatible activities */
      if (! $module$AbortActivity (servId, i)) {
	ACTIVITY_TAB_INCOMP(activity)[ACTIVITY_NB_INCOMP(activity)] = i;
	ACTIVITY_NB_INCOMP(activity)++;
      }  
}

