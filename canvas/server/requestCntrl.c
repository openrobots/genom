/*	$LAAS$ */

$commentbegin$
/* 
 * Copyright (c) 1993-2003 LAAS/CNRS
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
 *  $module$Cntrl$request$ - Control request
 *
 */

#if ($controlFuncFlag$)
  extern int $cControlFunc$ ();
#endif 

static void $module$Cntrl$request$ (SERV_ID servId, int rqstId)

{
  int i;
  $tabCompatibilityDeclare$
  int *compatibilityTab = $module$$request$Compatibility;
  int bilan=OK;
#if ($inputFlag$ && $controlFuncFlag$)
  $inputDeclare$
#endif

  /*--------------------------------------------------------------
   * Control init done and exec tasks status 
   */
  if (!controlExecTaskStatus(servId, rqstId))
    return;

  /*--------------------------------------------------------------
   * Interruption of the incompatible activities
   */
  for (i = 0; i < MAX_ACTIVITIES; i++) 
    if (ACTIVITY_STATUS(i) != ETHER && ! compatibilityTab[ACTIVITY_RQST_TYPE(i)]) 
      $module$AbortActivity (servId, i);

  /*--------------------------------------------------------------
   * Call control func
   */
#if ($controlFuncFlag$ /* Control func */)
  {
    BOOL status;

#if ($inputFlag$ /* Input */)
    /* Get inputs */
    if (csServRqstParamsGet (servId, rqstId, (void *) $inputNamePtr$, 
			     $inputSize$, (FUNCPTR) NULL) != OK)
      $module$ReplyAndSuspend (servId, rqstId, TRUE);
    
    /* Call control func */
    status = $cControlFunc$ ($inputNamePtr$, &bilan);
#else
    status = $cControlFunc$ (&bilan);
#endif
    if (status != OK) {
      if (bilan == OK)
	bilan =  S_$module$CntrlTask_CONTROL_CODEL_ERROR;
      if (csServReplySend (servId, rqstId, FINAL_REPLY, bilan, 
			   (void *) NULL, 0, (FUNCPTR) NULL) != OK)
	$module$CntrlTaskSuspend (TRUE);
      return;
    }
  }
#endif

 /*-------------------------------------------------------------
  * Record inputs
  */
#if ($inputFlag$ /* Input */) 
#if ($controlFuncFlag$ /* Control func */)
  memcpy ((void *) $inputRefPtr$, (void *) $inputNamePtr$, $inputSize$);
#else
  if (csServRqstParamsGet (servId, rqstId, (void *) $inputRefPtr$,
			   $inputSize$, (FUNCPTR) NULL) != OK)
    $module$ReplyAndSuspend (servId, rqstId, TRUE);
#endif
#endif

 /*-------------------------------------------------------------
  * Send final reply
  */
  if (csServReplySend (servId, rqstId, FINAL_REPLY, bilan, 
		       (void *) $outputRefPtr$, $outputSize$,
		       (FUNCPTR) NULL) != OK)
    $module$CntrlTaskSuspend (TRUE);
}

