#include "mbed.h"
#include "filter.h"
#include <cctype>

#define ROLE 0                          // initial role of ECU 0 - MASTER / 1 - SLAVE
#define SCALE_FACTOR 1.01               // scale factor is used to avoid intermittence in brake request
#define TIMEOUT 5000                    // maximum wait time for signal from body computer

extern "C" {
#include "controller.h"
#include "rtwtypes.h"
}

DigitalOut      led_brake(LED_BLUE);    // led used for braking notification
DigitalOut      led_error(LED_RED);     // led used for error notification
DigitalOut      trigger(D5);            // signal provided to distance sensor
InterruptIn     echo(D2);               // signal received from distance sensor
Timer           t;                      // timer for measuring distance sensor response
Timer           bc_t;                   // timer for detection of net ifc fault
Ticker          scheduler;              // scheduler for step
Ticker          scheduler_net;          // scheduler for network check
uint8_t role;                           // current role of the ECU

Serial pc(USBTX, USBRX);                // connection to the plant
//Serial debug(D15, D14);               // for debugging purposes, not used (PTE24, PTE25)
Serial manager(PTD3, PTD2);             // connection to the manager

Filter f(5);                            // simple moving mean filter

enum stat {                             // state of ECU
    NORMAL,                             // defined as enum
    ERROR
} state;

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

void read_manager()
{
    // read from managers possible malfunctions detected
    // - if malfunction set state to ERROR
    // - if promotion received and ECU currently is not able to 
    //   issue brake request then it becomes able to do so
    if (manager.readable())
    {
        char rec = manager.getc();
        fflush(manager);
        if(rec == 'E'){
            state = ERROR;
        }
        if((rec == 'P') && role == 1){
            role = 0;
        }
    }
}

void read_bc()
{
    // read speed from body computer (ASCII encoding)
    // - if malfunction set state to ERROR
    if (pc.readable())
    {
        bool malfunc = false;
        int i = 0;
        char buf[10];
        while (pc.readable() && i<10) {
            buf[i] = pc.getc();
            if (buf[i] == '\r') break;
            else if (!isdigit(buf[i])) malfunc = true;
            i++;
        }
        fflush(pc);
        if (!malfunc) {
            // the communication is working as expected then
            // the value is stored and the timer of network fault
            // check is reset
            buf[i] = '\0';
            controller_U.speed = atoi(buf);
            bc_t.reset();
        }
    }
}

void step()
{
    // perform reading from manager
    read_manager();

    // perform reading form body computer
    read_bc();

    // do a step of controller code
    rt_OneStep(controller_M);

    // output brake request to body computer
    // only if master otherwise brake request is 0
    int brake = 0;
    if ((role == 0) && (state == NORMAL)) {
        brake = controller_Y.brake;
    }
    pc.printf("%d %d\n", brake, controller_U.distance);

    // output heartbeat to manager to provide measured distance
    // status and role
    char s = 'N';
    if (state != NORMAL) {
        s = 'E';
    }
    char r = 'M';
    if (role != 0) {
        r = 'S';
    }
    manager.printf("%03d%c%c", controller_U.distance, s, r);

}

void onInterruptEdgeUp()
{
    t.start();
}

void onInterruptEdgeDown()
{
    t.stop();
    // set controller input as distance measured (passing through filter)
    controller_U.distance = f.newValue(uint8_t (t.read_us()/58.0));
    t.reset();
}

void netcheck()
{
    bc_t.stop();
    if (bc_t.read_ms() > TIMEOUT) {
        // network error
        state = ERROR;
    }
    bc_t.start();
}

int main()
{
    /* Pack model data into RTM */
    controller_M->ModelData.dwork = &controller_DW;

    /* Initialize model */
    controller_initialize(controller_M, &controller_U, &controller_Y);

    t.reset();

    echo.rise( &onInterruptEdgeUp );
    echo.fall( &onInterruptEdgeDown );

    controller_U.distance = 0;
    controller_U.speed = 0;
    controller_Y.brake = 0;
    controller_U.safe_factor = SCALE_FACTOR;

    state = NORMAL;
    role = ROLE;

    led_error = 1;
    trigger = 0;

    bc_t.reset();
    bc_t.start();

    // attach scheduled tasks to respective schedulers
    scheduler.attach( &step, 0.1 );
    scheduler_net.attach( &netcheck, 0.5);

    while (true) {
        trigger = 1;
        wait_us( 10 );
        trigger = 0;
        if (state == NORMAL){
            led_brake = controller_Y.led;
        }
        else {
            led_brake.write(1);
            led_error.write(0);
        }
    }
}