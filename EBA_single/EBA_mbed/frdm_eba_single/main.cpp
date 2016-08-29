#include "mbed.h"
#include "filter.h"

#define ROLE 0 // 0 - MASTER / 1 - SLAVE
#define SCALE_FACTOR 1.01

extern "C" {
  #include "controller.h"
  #include "rtwtypes.h"
}

DigitalOut      led(LED_BLUE);
DigitalOut      trigger(D5);
InterruptIn     echo(D2);
Timer           t;
Ticker          scheduler;

Serial  pc(USBTX, USBRX); // tx, rx
Serial  debug(D15, D14); // tx, rx

Filter f(5);

// 
// Copy from ert_main.c
//
static RT_MODEL_controller_T controller_M_;
static RT_MODEL_controller_T *const controller_M = &controller_M_;/* Real-time model */
static DW_controller_T controller_DW;/* Observable states */
static ExtU_controller_T controller_U;/* External inputs */
static ExtY_controller_T controller_Y;/* External outputs */

/*
 * OneStep function copied from ertmain.c
 */
void rt_OneStep(RT_MODEL_controller_T *const controller_M)
{
  static boolean_T OverrunFlag = false;
  /* Disable interrupts here */
  /* Check for overrun */
  if (OverrunFlag) {
    rtmSetErrorStatus(controller_M, "Overrun");
    return;
  }
  OverrunFlag = true;
  /* Save FPU context here (if necessary) */
  /* Re-enable timer or interrupt here */
  /* Set model inputs here */
  /* Step the model */
  controller_step(controller_M, &controller_U, &controller_Y);
  /* Get model outputs here */
  /* Indicate task complete */
  OverrunFlag = false;
  /* Disable interrupts here */
  /* Restore FPU context here (if necessary) */
  /* Enable interrupts here */
}
//
// End copy
//

void step() {
    // input is in ASCII encoding
    if (pc.readable()){
        int i = 0;
        char buf[10];
        while (pc.readable() && i<10){
            buf[i] = pc.getc();
            i++;
            if (buf[i-1] == '\r') break;
        }
        fflush(pc);
        buf[i] = '\0';
        controller_U.speed = atoi(buf);
    }
    
    // do a step of controller code
    rt_OneStep(controller_M);
   
    pc.printf("%d %d\n",controller_Y.brake, controller_U.distance);
    debug.printf("SPEED: %3d km/h BRAKE: %d DISTANCE %3d\n", controller_U.speed, controller_Y.brake, controller_U.distance);
}
    
void onInterruptEdgeUp(){
    t.start();    
}

void onInterruptEdgeDown(){
    t.stop();
    // set controller input as distance measured
    controller_U.distance = f.newValue(uint8_t(t.read_us()/58.0));
    t.reset();
}

int main()
{
    //
    // Copy from ert_main.c
    //    
    /* Pack model data into RTM */
    controller_M->ModelData.dwork = &controller_DW;
  
    /* Initialize model */
    controller_initialize(controller_M, &controller_U, &controller_Y);
    //
    // End copy
    //

    t.reset();
    echo.rise( &onInterruptEdgeUp );
    echo.fall( &onInterruptEdgeDown );
    controller_U.distance = 0;
    controller_U.speed = 0;
    controller_Y.brake = 0;
    controller_U.safe_factor = SCALE_FACTOR;
    trigger = 0;
      
    scheduler.attach( &step, 0.1 );
    debug.printf("Ready\n");
    while (true) {
        trigger = 1;
        wait_us( 10 );
        trigger = 0;
        led = controller_Y.led; 
    }
}