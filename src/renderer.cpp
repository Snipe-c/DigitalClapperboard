#include <MD_MAX72xx.h>
#include <stdlib.h>

/*
This is the render program. After Arduino convention, have all program a setup and loop function.
The render function is heavily optimised, so that the amount of flickering is reduced to a minimim.
It takes in a char* pointing to a sufficient amount of heap memory, where the colomns are represented as bytes.
It uses the set point func. because it is generally faster than setting a whole col. at once.

The inv_mat func. prepares the simple readable buffers for the render func.

Because of the check_btn func. can the program know if the clapperboard is closed and can display accordingly.

Here I have the example text JAPAN when closed and PHOT.Fi.01 when open. It is the remainder of a usecase I had for the d. clapperboard. 

Components used:

- 4x64 LED Matrix Display
- Arduino UNO (R3)
- Wires
- 2x 10 K Ohm resistors
- 1 nF Capacitor
- Clapperboard from Amamzon
- Double sticky tape

*/

#define HARDWARE_TYPE MD_MAX72XX::ICSTATION_HW
//
#define MAX_8X8_MAT 4 
#define COLUMNS 8 // per display
#define ALL_COL 32 // 4*8 Col
//
// Digital Output Pins
#define CLK_PIN 13 
#define DATA_PIN 11
#define CS_PIN 10

#define BTN 2 // Digital Input Pin

MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_8X8_MAT);

char port_tas = BTN;
bool stat = false;

void check_btn(){
  if (digitalRead(BTN) == HIGH){ // HIGH when reading 5V
    stat = true;
  }
  else{
    stat = false;
  }
}

// JAPAN
char mat_Text_j [MAX_8X8_MAT][COLUMNS] = {  
// A byte can be interpreted as beginning the bottom and setting one bit at a time going up.
// The next byte is the next col. from left to right.
  {0b00110010, 0b00100010, 0b00111110, 0b00000000, 0b00111110, 0b00001010, 0b00111110, 0b00000000},
  {0b00111110, 0b00001010, 0b00001110, 0b00000000, 0b00111110, 0b00001010, 0b00111110, 0b00000000},
  {0b00111110, 0b00001100, 0b00011000, 0b00111110, 0b00000000, 0b00000000, 0b00000000, 0b00000000},
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
};

// PHOT.Fi.01
char mat_Text_ph [MAX_8X8_MAT][COLUMNS] = {
  {0b00111110, 0b00001010, 0b00001110, 0b00000000, 0b00111110, 0b00001000, 0b00111110, 0b00000000},
  {0b00011100, 0b00100010, 0b00011100, 0b00000000, 0b00000010, 0b00111110, 0b00000010, 0b00100000},
  {0b00000000, 0b00111110, 0b00001010, 0b00000010, 0b00000000, 0b00111010, 0b00000000, 0b00100000},
  {0b00000000, 0b00111110, 0b00100010, 0b00111110, 0b00000000, 0b00111110, 0b00000000, 0b00000000}
};

char* mat_Text_j_inv = (char*)malloc(sizeof(char) * ALL_COL); // Setting up memory for the parameter for render()
char* mat_Text_ph_inv = (char*)malloc(sizeof(char) * ALL_COL);

void inv_mat(char arr[MAX_8X8_MAT][COLUMNS], char* res){
  for (char column = 0; column < ALL_COL; column++){
    char dis = column >> 3;
    *res = arr[dis][COLUMNS - 1 - (column - (dis << 3))]; 
    res++;
  }
}

void render(char* mat_Text) {
  mx.update(MD_MAX72XX::OFF);
  for (char row = 0; row < 8; row++){
    for (char column = 0; column < ALL_COL; column++){
      mx.setPoint(row, column, (mat_Text[column] >> row) & 0x01); 
    }
  }
  mx.update(MD_MAX72XX::ON);
  mx.update(MD_MAX72XX::OFF);
}

void setup() {
  mx.begin();
  inv_mat(mat_Text_j, mat_Text_j_inv); // inverting the original buffer and storing result at given adress // has to be on heap to garantee pointer is valid
  inv_mat(mat_Text_ph, mat_Text_ph_inv); 
  pinMode(BTN, INPUT);
//  mx.control(MD_MAX72XX::INTENSITY, 15); // Normally, the display is set to half brightness. Range from 0x0 to 0xF 
}

void loop() {
  check_btn();
  if (stat == true){ // if closed
    render(mat_Text_j_inv);
  }
  if (stat == false){
    render(mat_Text_ph_inv);
  }
}
