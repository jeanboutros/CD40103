/*
This project demonstrates interfacing the CD40103BEE4 8-bit binary 8-stage down counter with Arduino.

The CD40103 is a versatile IC that can be used for various counting and timing applications.
This tutorial guides you through connecting the IC to Arduino, setting up pins, and using
the `cycle_clk` function to simulate a clock signal and count clock cycles.

We'll explore the functionality of key CD40103 pins:
  - CLK (Clock): Enables decrementing on each positive transition.
  - INV_CO_ZD (Carry Out / Zero Detect): Goes LOW for one clock cycle when the count reaches zero.
  - INV_SYN_PE (Synchronous Preset Enable): Clocks data from JAM inputs on the next positive clock transition.

Connections (refer to CD40103 datasheet for more details):
  - CLK (CD40103 Pin 1) -> Arduino Pin (e.g., Pin 2)
  - VDD (CD40103 Pin 16) -> +5V Power Supply
  - GND (CD40103 Pin 8) -> Ground
  - INV_CO_ZD (CD40103 Pin 14) -> Arduino Pin (e.g., Pin 4) (Input)
  - INV_SYN_PE (CD40103 Pin 15) -> Arduino Pin (e.g., Pin 6) (Output)
  - JAM Inputs (J7-J0) (CD40103 Pins 13-4): Connect these pins to Arduino pins or tie them to HIGH/LOW
      to preset the initial counter value.

Datasheet: https://www.ti.com/lit/ds/symlink/cd40103b.pdf 
Mouser link: https://mou.sr/4aeHEmU

Author: Jean Boutros

*/


/* IC PIN 1 - CLOCK
The CLOCK pin enables the counter to decrememnt on each positive transition
of the clock. This pin should be connected to a clock such as a crystal or
any oscillator. In this example instead of using an oscillator, I am imitating 
a clock by using a function that sends a signal that is hign, than low, with 50%
duty cycle. The fucntion name is cycle_clk() and if no arguments are passed,
this function triggers one clock cycle. I chose this method to be able to run a 
controlled simulation and count the clock cycles in each case.
 */
const int CLOCK_PIN = 2; // IC PIN 1



/* IC PIN 2 - CLEAR

WARNING: THIS IS AN OUTPUT PIN

When the input on pin 2 is set to LOW the counter is asynchronously (instantly) cleared
to its maximum count which is 255 for this IC, regardless of the state of any other input.

Connect this pin to VDD (+5V or +3.3V) to keep the input HIGH all the time.
Tip: If you want to drive this pin using arduino, connect it to one of the available GPIO pins 
and set the pinMode to OUTPUT and the value to HIGH.
Example:
// for default configuration
const int INV_CLR = X; // IC PIN 2

void setup() {
  ...
  pinMode(INV_CLR, OUTPUT); 
  digitalWrite(INV_CLR, HIGH);
  ...
}

// to reset the counter you can do
void reset() {
  digitalWrite(INV_CLR, LOW);
  delayMicroseconds(XXX);
  digitalWrite(INV_CLR, HIGH);
}

 */



/* IC PIN 3 - CARRY-IN/COUNTER ENABLE 

WARNING: THIS IS AN OUTPUT PIN

It is a carry-input pin also known as counter enable pin. 
When this input pin is HIGH, the counter is inhibited so it stops counting.

Connect this pin to ground (GND 0V).

Tip: If you want to drive this pin using arduino, connect it to one of the available GPIO pins 
and set the pinMode to OUTPUT and the value to LOW.

Example:
// for default configuration
const int INV_CI_CE = X; // IC Pin 3

void setup() {
  ...
  pinMode(INV_CI_CE, OUTPUT); 
  digitalWrite(INV_CI_CE, LOW);
  ...
}

// to pause the counter
void pause_counter() {
  digitalWrite(INV_CI_CE, HIGH);
}
// to resume the counter
void resume_counter() {
  digitalWrite(INV_CI_CE, LOW);
}

*/


/* IC PIN 4 - Jam input 0 or J0

WARNING: THIS IS AN OUTPUT PIN

Represent 1 bit from an 8-bit word that makes up the number
between 0 and 255 that can be input into the counter for the counter
to count down from that number.
The 8-bit word is as follows:
J7-J6-J5-J4-J3-J2-J1-J0

if J0 is 1 and all the others are 0 we get: 00000001 ie. decimal 1

Set this pin to HIGH. We will set pins J7 to J0 all to HIGH so that we
get 255 into the counter when initialised and then we count down.
Feel free to change the pin confighurtations for the JAM inputs or 
to drive them using the arduino IO.

Example:
const int jamPins[8] = { 8, 9, 10, 11, 12, 13, xx, xx };  // Replace with actual pin numbers

void preset_counter_sync() {
  counter = 0;  // Reset counter before presetting

  // **Preset the counter to a specific value using JAM inputs:**

  // - Set the desired binary value on the JAM pins (J7-J0) using Arduino's digitalWrite function.
  //   - A HIGH value on a JAM pin represents a 1 in the binary value.
  //   - A LOW value on a JAM pin represents a 0 in the binary value.

  // For example, to preset the counter to decimal 12 (binary 00001100):
  digitalWrite(jamPins[7], LOW);
  digitalWrite(jamPins[6], LOW);
  digitalWrite(jamPins[5], LOW);
  digitalWrite(jamPins[4], LOW);
  digitalWrite(jamPins[3], LOW);
  digitalWrite(jamPins[2], HIGH); // Set bit 2 (J2) to 1
  digitalWrite(jamPins[1], HIGH); // Set bit 1 (J1) to 1
  digitalWrite(jamPins[0], LOW);

  // - Pulse the Synchronous Preset Enable (INV_SYN_PE) pin LOW to latch the JAM input values
  //   into the counter on the next positive clock transition.
  digitalWrite(presetPin, LOW);
  cycle_clk();
  digitalWrite(presetPin, HIGH);

  Serial.println("Counter preset");
}

*/



/* IC PIN 9 - Asynchronous Preset Enable 

WARNING: THIS IS AN OUTPUT PIN

When input on this pin is LOW, data at the JAM inputs is asynchronously forced
into the counter withou waiting for the next clock cycle.

Connect this pin to VCC (+5V or +3.3V).

// const int INV_ASYN_PE = 7; // IC PIN 9
*/



/* IC PIN 14 - Carry out / Zero detect 

WARNING: THIS IS AN INPUT PIN

The output on this pin goes low for one clock cycle when the count
reaches zero.

*/
const int INV_CO_ZD = 4; // IC Pin 14



/* IC PIN 15 - Synchronous Preset Enable 

WARNING: THIS IS AN OUTPUT PIN

When input on this pin is LOW, data at the JAM inputs is clocked
into the counter on the next positive clock trasition.
*/
const int INV_SYN_PE = 6; // IC PIN 15



// PIN Configuration
const int LED_PIN = 13;    // the on-board pin

// Default Clock Pulse Length in Microseconds
const int CLK_DELAY = 1000;

// Genral counter, in this case used to count the clock cycles
int counter = 0;

void cycle_clk(int numCycles = 1) {
  // Simulates a clock signal by sending a HIGH-LOW pulse with 50% duty cycle
  // for the specified number of cycles (default 1). Updates the counter accordingly.
  for (int i = 0; i < numCycles; i++) {
    digitalWrite(CLOCK_PIN, HIGH);
    delayMicroseconds(CLK_DELAY);
    digitalWrite(CLOCK_PIN, LOW);
  }
  counter += numCycles;  // Update counter based on number of cycles
}


void preset_counter_sync() {
  // Resets the counter to the values set on the JAM inputs (J7-J0) on the next
  // positive clock transition.
  // Example:
  // To Reset the counter to a specific value (e.g., 10 decimal = 1010 in binary):
  // Connect J1 (CD40103 Pin 5) to HIGH and other JAM inputs (J7-J0) to LOW.
  // This will set the counter to 10 on the next positive clock transition.
  counter = 0;
  // preset on next positive clock transition
  digitalWrite(INV_SYN_PE, LOW);
  cycle_clk();
  digitalWrite(INV_SYN_PE, HIGH);
  Serial.println("Counter preset to the values from the JAM inputs"); 
}

void setup() {
  // initialize the LED as an output:
  pinMode(LED_PIN, OUTPUT);       // Set the LED pin (defined as LED_PIN) to be an output pin.

  pinMode(INV_CO_ZD, INPUT);      // Set the Carry Out / Zero Detect pin (defined as INV_CO_ZD) to be an input pin.
  
  pinMode(INV_SYN_PE, OUTPUT);    // Set the Synchronous Preset Enable pin (defined as INV_SYN_PE) to be an output pin.
                                  // We will use this pin to control when the counter is pre-loaded with a value.
  digitalWrite(INV_SYN_PE, HIGH); // Set the Synchronous Preset Enable pin to HIGH initially.
                                  // This ensures the counter doesn't pre-load yet.


  pinMode(CLOCK_PIN, OUTPUT);     // Set the Clock pin (defined as CLOCK_PIN) to be an output pin.
                                  // We will use this pin to simulate a clock signal for the counter.
  
  cycle_clk(); // Run one clock cycle to initiate the counter.

  preset_counter_sync(); // Preset the counter to the value from data on J7-J0 on the next clock cycle.

  // initialize serial communication:
  Serial.begin(9600);
}

int local_invCoZdState = -1; // Current state of IC PIN 14 - Carry out / Zero detect

int local_count = 0;
void loop() {
  // Get the state of IC PIN 14 - Carry out / Zero detect
  local_invCoZdState = digitalRead(INV_CO_ZD);
  
  digitalWrite(LED_PIN, !local_invCoZdState);
  
  if ( local_invCoZdState == LOW ) {
    Serial.print("Clock cycle: "); Serial.print(counter); Serial.print(" -- invCoZdState: "); Serial.println(local_invCoZdState);
    
    // Preset the counter again.
    // This step is not necessary if the counters is set the it's maximun ie 255 or 11111111
    // Also the CO/ZD pin can be connected to the CLEAR pin to automatically reset the counter or
    // it can be connected to the SPE pin (pin 15) to automatically preset the counter to the values
    // of the JAM inputs.
    preset_counter_sync();
    Serial.println("Counter reset"); 
  }
  // Run one clock cycle
  cycle_clk();
}
