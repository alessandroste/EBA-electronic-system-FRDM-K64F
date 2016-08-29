/*  This code is executed on the Arduino board, which manages the coordinated work of the two ECUs.
 *  Arduino is responsible for the detection of boards errors, which are not completely able to detect a malfunction themself.
 *  The following checks are performed:
 *  - PRESENCE OF SIGNAL: if a port does not receive any signal in a defined number of cycles, then it means that the ECU
 *    is not working or that the communication cannot take place. After a certain amount of time the ECU is considered
 *    faulty.
 *  - COHERENCE OF SIGNAL: the signal coming from a port is analyzed for errors in the structure of information,
 *    in particular two checks are executed: the first three characters should represent the distance measured,
 *    hence they have to be digits, second, the last two characters should represent respectively the state and the role of
 *    ECU. If they are not consistent with what declared before or if the message has an unexpectd total 
 *    lenght then the ECU could be faulty. However, some disturbs can occur along the transmission line, hence a 
 *    reasonable number of errors in the stream has to occur in order to consider the ECU faulty.
 *  - COHERENCE OF MEASURE: the distances measured by the ECUs are compared, and if the difference is higher than
 *    a certain threshold, then a malfunction could have happened in one ECU. In this case there is not any other
 *    way to check which is the correctly working ECU. Hence we decided to assume that the master has a
 *    better hardware so the slave will be considered faulty after a certain amount of wrong measurements.
 *  - SELF CHECK OF ECU: the ECUs inform the manager about their state, because they can detect a malfunction 
 *    also on their own (for example absence of signal from body computer which should provide vehicle current
 *    speed). If the master ECU notifies the manager that it is not working correctly, then the manager promotes
 *    the slave by sending a promote signal, so that it can issue brake requests to the vehicle.
 *  To improve the system, a bypass communicaton between master and slave can be added, because in case of manager
 *  fault the system is not able to work at all. Anyway, in this study, the manager is considered always working properly.
 */

#include <SoftwareSerial.h>
#include <ctype.h>                    // used for isDigit function

const unsigned int WAITFORDATA = 70;  // delay required to receive data
const unsigned int MAXERRORS = 5;     // maximum number of errors before deactivating ecu
const unsigned int THRESHOLD = 20;    // maximum acceptable difference between distance measurements (cm)
const unsigned int MAXDISTER = 5;     // maximum number of steps with measured distance difference over limit
const unsigned int MAXWAIT = 10;      // max wait time in steps without signal to consider an ECU faulty
unsigned int i;
char buff[2];                         // buffer for saving variables about state and role
char buff_n[4];                       // buffer for saving speed in chars
bool fault_one = false;               // becomes TRUE when the ECU at port one is faulty
bool fault_two = false;               // becomes TRUE when the ECU at port two is faulty
char state_one;                       // variable for storing state of ECU at port one
char state_two;                       // variable for storing state of ECU at port two
bool f;                               // utility variable for fault check
unsigned int p_one_errors = 0;        // number of stream errors at port one
unsigned int p_two_errors = 0;        // number of stream errors at port two
unsigned int dist_errors = 0;         // number of errors of measurements of distance between the two ECUs
unsigned int dist_one;                // variable for storing distance measured by ECU at port one
unsigned int dist_two;                // variable for storing distance measured by ECU at port two
unsigned int wait_one = 0;            // number of steps without signal from port one
unsigned int wait_two = 0;            // number of steps without signal from port two
bool role_one;                        // variable for storing role of ECU at port one
bool role_two;                        // variable for storing role of ECU at port two

// The board has not got any available hardware serial port because the only one is used for debugging and 
// direct communicaton with body computer. Hence the solution is to use virtual serial ports which are
// implemented through virtual buffers and interrupts. The downside is that only one port at a time can
// receive values in its buffer.
// To overcome this problem a spin use of serial buffer is done: alternatively the two ports are activated
// with the command p_#.listen(), then a small delay is put to receive the signal, then the buffer is read.
// The delay has to be calibrated in function of serial sampling time of ECUs.

SoftwareSerial p_one(10, 11);          // software serial #1: RX = digital pin 10, TX = digital pin 11
SoftwareSerial p_two(6, 7);            // software serial #2: RX = digital pin 6, TX = digital pin 7

void setup(){
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  // Start each software serial port
  p_one.begin(9600);
  p_two.begin(9600);
}

void loop(){
  // Listening for signals from ECU at port one
  i = 0;
  f = false;
  p_one.listen();
  delay(WAITFORDATA);
  if (p_one.available()){
    // A signal is received from ECU at port one
    while(p_one.available() && i < 5){
      if (i < 3){
        // First three characters should be digits
        buff_n[i] = p_one.read();
        // If character is not digit there is an error in the stream
        if (!isDigit(buff_n[i])) f = true;
      }
      else {
        // Other two alphabetic characters for state and role
        buff[i-3] = p_one.read();
        // First is state and should be E (error) or N (normal)
        if ((i == 3) && !((buff[i-3] == 'E') || (buff[i-3] == 'N'))) f = true;
        // Second is role and should be S (slave) or M (master)
        if ((i == 4) && !((buff[i-3] == 'S') || (buff[i-3] == 'M'))) f = true;
      }
      i++;
      delay(1);
    }
    // If the message lenght is not the one expected or there is a structure error
    // in the stream then the count of errors at port one is incremented
    if ((i != 5) || f ) { 
      p_one_errors++;
    }
    else {
      // If the message is good the count of errors is reset
      p_one_errors = 0;
      // The conversion of distance from characters to integer takes place
      // through the function atoi which accepts a string so the last character
      // has to be a null character
      buff_n[3] = '\0';
      dist_one = atoi(buff_n);
      // If the ECU is already in fault (due to previous communication from manager
      // or failed self check) then it is considered
      state_one = buff[0];
      if (buff[0] == 'E') fault_one = true;
      // The role of ECU at port one is saved
      if (buff[1] == 'M')
        role_one = 0;
      else
        role_one = 1;
    }
    // A message has arrived, then the counter of steps without message has to be
    // reset
    wait_one = 0;
  } else {
    // No message received, then the counter of steps without message has to be
    // incremented
    wait_one++;  
  }

  // Same procedure for port two
  i = 0;
  f = false;
  p_two.listen();
  delay(WAITFORDATA);
  if (p_two.available()){
    //Serial.print("PORT 2|");
    while(p_two.available() && i < 5){
      if (i < 3) {
        buff_n[i] = p_two.read();
        if (!isDigit(buff_n[i])){
          f = true;
        }
        //Serial.write(buff_n[i]);
      }
      else {
        buff[i-3] = p_two.read();
        if ((i == 3) && !((buff[i-3] == 'E') || (buff[i-3] == 'N'))) f = true;
        if ((i == 4) && !((buff[i-3] == 'S') || (buff[i-3] == 'M'))) f = true;
        //Serial.write(buff[i-3]);
      }
      i++;
      delay(1);
    }
    if ((i != 5) || f ) { 
      p_two_errors++;
    }
    else {
      p_two_errors = 0;
      buff_n[3] = '\0';
      dist_two = atoi(buff_n);
      state_two = buff[0];
      if (buff[0] == 'E') fault_two = true;
      if (buff[1] == 'M')
        role_two = 0;
      else
        role_two = 1;
    }
    wait_two = 0;
  } else {
    wait_two++;  
  }

  // Now all the checks take place
  // If the distances are too much different the counter of measurement
  // errors is incremented, otherwise is reset
  if (THRESHOLD < abs(dist_two - dist_one)){
    dist_errors++;
  } else {
    dist_errors = 0;
  }

  // If the measurements differ for too long time the slave board if marked
  // as faulty if the master is not faulty
  if (dist_errors > MAXDISTER) {
    if ((role_one == 0) && !fault_one) fault_two = true;
    if ((role_two == 0) && !fault_two) fault_one = true;
  }

  // If the stream from an ECU is not coherent or is not present at all
  // for some time, the the ECU is marked as faulty
  if (p_one_errors > MAXERRORS || wait_one > MAXWAIT) fault_one = true;
  if (p_two_errors > MAXERRORS || wait_two > MAXWAIT) fault_two = true;

  // Debugging purpose
  delay(100);
  Serial.print("PORT 1|");
  Serial.print(" DIST:");
  Serial.print(dist_one);
  Serial.print(" REC_STAT:");
  Serial.print(state_one);
  Serial.print(" STAT:");
  Serial.print(fault_one);
  Serial.print(" ROLE:");
  Serial.print(role_one);
  Serial.write('\n');    
  Serial.print("PORT 2|");
  Serial.print(" DIST:");
  Serial.print(dist_two);
  Serial.print(" REC_STAT:");
  Serial.print(state_two);
  Serial.print(" STAT:");
  Serial.print(fault_two);
  Serial.print(" ROLE:");
  Serial.print(role_two);
  Serial.write('\n');  

  // Messages to the ECUs are sent on the base of their state:
  // - E stands for error: if an ECU receives E it put itself in error
  //   state if is not already in that state
  // - P stands for promote: if an ECU receives P it put its role in
  //   master state if is not alreay in master state (i.e. able to issue
  //   brake requests)
  if (fault_one && !fault_two){
    p_one.print("E");
    if (role_two == 1)
      p_two.print("P");
  }
  if (!fault_one && fault_two){
    if (role_one == 1)
      p_one.print("P");
    p_two.print("E");
  }
  if (fault_one && fault_two){
    Serial.println("No ECUs available. System is deactivated.");
    // An infinite delay means that the component is deactivated
    // Error message is transmitted continuously to be sure that the boards
    // receive it
    while(true){
      p_one.print("E");
      delay(1);
      p_two.print("E");
      delay(1);
    }
    // Moreover a visual indicator, like a LED can be lighted in this
    // case by the manager through a DigitalWrite command and a suitable circuit.
  }

}
