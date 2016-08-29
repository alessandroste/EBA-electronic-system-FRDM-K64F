/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: controller.c
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

#include "controller.h"

/* Named constants for Chart: '<S1>/Chart' */
#define controller_IN_INIT_STATE       ((uint8_T)1U)
#define controller_IN_LED_OFF          ((uint8_T)1U)
#define controller_IN_LED_ON           ((uint8_T)2U)
#define controller_IN_NO_ACTIVE_CHILD  ((uint8_T)0U)
#define controller_IN_SAFE_DISTANCE    ((uint8_T)2U)
#define controller_IN_UNSAFE_DISTANCE  ((uint8_T)3U)

/*===========*
 * Constants *
 *===========*/
#define RT_PI                          3.14159265358979323846
#define RT_PIF                         3.1415927F
#define RT_LN_10                       2.30258509299404568402
#define RT_LN_10F                      2.3025851F
#define RT_LOG10E                      0.43429448190325182765
#define RT_LOG10EF                     0.43429449F
#define RT_E                           2.7182818284590452354
#define RT_EF                          2.7182817F

/*
 * UNUSED_PARAMETER(x)
 *   Used to specify that a function parameter (argument) is required but not
 *   accessed by the function body.
 */
#ifndef UNUSED_PARAMETER
# if defined(__LCC__)
#   define UNUSED_PARAMETER(x)                                   /* do nothing */
# else

/*
 * This is the semi-ANSI standard way of indicating that an
 * unused function parameter is required.
 */
#   define UNUSED_PARAMETER(x)         (void) (x)
# endif
#endif

/* Model step function */
void controller_step(RT_MODEL_controller_T *const controller_M,
                     ExtU_controller_T *controller_U, ExtY_controller_T
                     *controller_Y)
{
  DW_controller_T *controller_DW = ((DW_controller_T *)
    controller_M->ModelData.dwork);

  /* Chart: '<S1>/Chart' incorporates:
   *  Inport: '<Root>/distance'
   *  Inport: '<Root>/safe_factor'
   *  Inport: '<Root>/speed'
   */
  if (controller_DW->temporalCounter_i1 < MAX_uint32_T) {
    controller_DW->temporalCounter_i1++;
  }

  /* Gateway: controller/Chart */
  /* During: controller/Chart */
  if (controller_DW->is_active_c3_controller == 0U) {
    /* Entry: controller/Chart */
    controller_DW->is_active_c3_controller = 1U;

    /* Entry Internal: controller/Chart */
    /* Transition: '<S2>:9' */
    controller_DW->is_c3_controller = controller_IN_INIT_STATE;

    /* Outport: '<Root>/brake' */
    /* Entry 'INIT_STATE': '<S2>:8' */
    /* '<S2>:8:1' brake = uint8(0), */
    controller_Y->brake = 0U;

    /* Outport: '<Root>/led' */
    /* '<S2>:8:1' led = uint8(0), */
    controller_Y->led = 0U;

    /* Outport: '<Root>/state' */
    /* '<S2>:8:2' state = uint8(0); */
    controller_Y->state = 0U;
  } else {
    switch (controller_DW->is_c3_controller) {
     case controller_IN_INIT_STATE:
      /* During 'INIT_STATE': '<S2>:8' */
      /* '<S2>:12:1' sf_internal_predicateOutput = ... */
      /* '<S2>:12:1' D > (V^2)/100; */
      if (controller_U->distance > controller_U->speed * controller_U->speed /
          100.0) {
        /* Transition: '<S2>:12' */
        controller_DW->is_c3_controller = controller_IN_SAFE_DISTANCE;

        /* Outport: '<Root>/brake' */
        /* Entry 'SAFE_DISTANCE': '<S2>:10' */
        /* '<S2>:10:1' brake = uint8(0), */
        controller_Y->brake = 0U;

        /* Outport: '<Root>/led' */
        /* '<S2>:10:1' led = uint8(0); */
        controller_Y->led = 0U;
      } else {
        /* '<S2>:13:1' sf_internal_predicateOutput = ... */
        /* '<S2>:13:1' D <= (V^2)/100; */
        if (controller_U->distance <= controller_U->speed * controller_U->speed /
            100.0) {
          /* Transition: '<S2>:13' */
          controller_DW->is_c3_controller = controller_IN_UNSAFE_DISTANCE;

          /* Outport: '<Root>/brake' */
          /* Entry 'UNSAFE_DISTANCE': '<S2>:11' */
          /* '<S2>:11:1' brake = uint8(1); */
          controller_Y->brake = 1U;

          /* Entry Internal 'UNSAFE_DISTANCE': '<S2>:11' */
          /* Transition: '<S2>:15' */
          controller_DW->is_UNSAFE_DISTANCE = controller_IN_LED_ON;
          controller_DW->temporalCounter_i1 = 0U;

          /* Outport: '<Root>/led' */
          /* Entry 'LED_ON': '<S2>:14' */
          /* '<S2>:14:1' led = uint8(1); */
          controller_Y->led = 1U;
        }
      }
      break;

     case controller_IN_SAFE_DISTANCE:
      /* During 'SAFE_DISTANCE': '<S2>:10' */
      /* '<S2>:19:1' sf_internal_predicateOutput = ... */
      /* '<S2>:19:1' D <= (V^2)/100; */
      if (controller_U->distance <= controller_U->speed * controller_U->speed /
          100.0) {
        /* Transition: '<S2>:19' */
        controller_DW->is_c3_controller = controller_IN_UNSAFE_DISTANCE;

        /* Outport: '<Root>/brake' */
        /* Entry 'UNSAFE_DISTANCE': '<S2>:11' */
        /* '<S2>:11:1' brake = uint8(1); */
        controller_Y->brake = 1U;

        /* Entry Internal 'UNSAFE_DISTANCE': '<S2>:11' */
        /* Transition: '<S2>:15' */
        controller_DW->is_UNSAFE_DISTANCE = controller_IN_LED_ON;
        controller_DW->temporalCounter_i1 = 0U;

        /* Outport: '<Root>/led' */
        /* Entry 'LED_ON': '<S2>:14' */
        /* '<S2>:14:1' led = uint8(1); */
        controller_Y->led = 1U;
      }
      break;

     default:
      /* During 'UNSAFE_DISTANCE': '<S2>:11' */
      /* '<S2>:20:1' sf_internal_predicateOutput = ... */
      /* '<S2>:20:1' D > (V^2)/100 * safe_factor; */
      if (controller_U->distance > controller_U->speed * controller_U->speed /
          100.0 * controller_U->safe_factor) {
        /* Transition: '<S2>:20' */
        /* Exit Internal 'UNSAFE_DISTANCE': '<S2>:11' */
        controller_DW->is_UNSAFE_DISTANCE = controller_IN_NO_ACTIVE_CHILD;
        controller_DW->is_c3_controller = controller_IN_SAFE_DISTANCE;

        /* Outport: '<Root>/brake' */
        /* Entry 'SAFE_DISTANCE': '<S2>:10' */
        /* '<S2>:10:1' brake = uint8(0), */
        controller_Y->brake = 0U;

        /* Outport: '<Root>/led' */
        /* '<S2>:10:1' led = uint8(0); */
        controller_Y->led = 0U;
      } else if (controller_DW->is_UNSAFE_DISTANCE == controller_IN_LED_OFF) {
        /* During 'LED_OFF': '<S2>:16' */
        /* '<S2>:17:1' sf_internal_predicateOutput = ... */
        /* '<S2>:17:1' after(1/(1+V^2/100-D), msec); */
        if (controller_DW->temporalCounter_i1 * 100U >= 1.0 /
            ((controller_U->speed * controller_U->speed / 100.0 + 1.0) -
             controller_U->distance)) {
          /* Transition: '<S2>:17' */
          controller_DW->is_UNSAFE_DISTANCE = controller_IN_LED_ON;
          controller_DW->temporalCounter_i1 = 0U;

          /* Outport: '<Root>/led' */
          /* Entry 'LED_ON': '<S2>:14' */
          /* '<S2>:14:1' led = uint8(1); */
          controller_Y->led = 1U;
        }
      } else {
        /* During 'LED_ON': '<S2>:14' */
        /* '<S2>:18:1' sf_internal_predicateOutput = ... */
        /* '<S2>:18:1' after(1/(1+V^2/100-D), msec); */
        if (controller_DW->temporalCounter_i1 * 100U >= 1.0 /
            ((controller_U->speed * controller_U->speed / 100.0 + 1.0) -
             controller_U->distance)) {
          /* Transition: '<S2>:18' */
          controller_DW->is_UNSAFE_DISTANCE = controller_IN_LED_OFF;
          controller_DW->temporalCounter_i1 = 0U;

          /* Outport: '<Root>/led' */
          /* Entry 'LED_OFF': '<S2>:16' */
          /* '<S2>:16:1' led = uint8(0); */
          controller_Y->led = 0U;
        }
      }
      break;
    }
  }

  /* End of Chart: '<S1>/Chart' */
}

/* Model initialize function */
void controller_initialize(RT_MODEL_controller_T *const controller_M,
  ExtU_controller_T *controller_U, ExtY_controller_T *controller_Y)
{
  DW_controller_T *controller_DW = ((DW_controller_T *)
    controller_M->ModelData.dwork);

  /* Registration code */

  /* states (dwork) */
  (void) memset((void *)controller_DW, 0,
                sizeof(DW_controller_T));

  /* external inputs */
  (void) memset((void *)controller_U, 0,
                sizeof(ExtU_controller_T));

  /* external outputs */
  (void) memset((void *)controller_Y, 0,
                sizeof(ExtY_controller_T));

  /* SystemInitialize for Chart: '<S1>/Chart' */
  controller_DW->is_UNSAFE_DISTANCE = controller_IN_NO_ACTIVE_CHILD;
  controller_DW->temporalCounter_i1 = 0U;
  controller_DW->is_active_c3_controller = 0U;
  controller_DW->is_c3_controller = controller_IN_NO_ACTIVE_CHILD;
}

/* Model terminate function */
void controller_terminate(RT_MODEL_controller_T *const controller_M)
{
  /* (no terminate code required) */
  UNUSED_PARAMETER(controller_M);
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
