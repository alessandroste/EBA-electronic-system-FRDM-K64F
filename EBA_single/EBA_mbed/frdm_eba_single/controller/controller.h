/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: controller.h
 *
 * Code generated for Simulink model 'controller'.
 *
 * Model version                  : 1.6
 * Simulink Coder version         : 8.10 (R2016a) 10-Feb-2016
 * C/C++ source code generated on : Wed Jul 20 16:07:00 2016
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#ifndef RTW_HEADER_controller_h_
#define RTW_HEADER_controller_h_
#include <string.h>
#ifndef controller_COMMON_INCLUDES_
# define controller_COMMON_INCLUDES_
#include "rtwtypes.h"
#endif                                 /* controller_COMMON_INCLUDES_ */

/* Macros for accessing real-time model data structure */
#ifndef rtmGetErrorStatus
# define rtmGetErrorStatus(rtm)        ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
# define rtmSetErrorStatus(rtm, val)   ((rtm)->errorStatus = (val))
#endif

/* Forward declaration for rtModel */
typedef struct tag_RTM_controller_T RT_MODEL_controller_T;

/* Block states (auto storage) for system '<Root>' */
typedef struct {
  uint32_T temporalCounter_i1;         /* '<S1>/Chart' */
  uint8_T is_active_c3_controller;     /* '<S1>/Chart' */
  uint8_T is_c3_controller;            /* '<S1>/Chart' */
  uint8_T is_UNSAFE_DISTANCE;          /* '<S1>/Chart' */
} DW_controller_T;

/* External inputs (root inport signals with auto storage) */
typedef struct {
  uint8_T distance;                     /* '<Root>/distance' */
  uint8_T speed;                        /* '<Root>/speed' */
  real_T safe_factor;                  /* '<Root>/safe_factor' */
} ExtU_controller_T;

/* External outputs (root outports fed by signals with auto storage) */
typedef struct {
  uint8_T brake;                       /* '<Root>/brake' */
  uint8_T led;                         /* '<Root>/led' */
  uint8_T state;                       /* '<Root>/state' */
} ExtY_controller_T;

/* Real-time Model Data Structure */
struct tag_RTM_controller_T {
  const char_T * volatile errorStatus;

  /*
   * ModelData:
   * The following substructure contains information regarding
   * the data used in the model.
   */
  struct {
    DW_controller_T *dwork;
  } ModelData;
};

/* Model entry point functions */
extern void controller_initialize(RT_MODEL_controller_T *const controller_M,
  ExtU_controller_T *controller_U, ExtY_controller_T *controller_Y);
extern void controller_step(RT_MODEL_controller_T *const controller_M,
  ExtU_controller_T *controller_U, ExtY_controller_T *controller_Y);
extern void controller_terminate(RT_MODEL_controller_T *const controller_M);

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Note that this particular code originates from a subsystem build,
 * and has its own system numbers different from the parent model.
 * Refer to the system hierarchy for this subsystem below, and use the
 * MATLAB hilite_system command to trace the generated code back
 * to the parent model.  For example,
 *
 * hilite_system('controller_only/controller')    - opens subsystem controller_only/controller
 * hilite_system('controller_only/controller/Kp') - opens and selects block Kp
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'controller_only'
 * '<S1>'   : 'controller_only/controller'
 * '<S2>'   : 'controller_only/controller/Chart'
 */
#endif                                 /* RTW_HEADER_controller_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
