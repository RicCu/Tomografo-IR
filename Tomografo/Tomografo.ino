/*************************************************************************/

/***********************CONSTANT DEFINITIONS******************************/
#define NUM_DETECTORS 3
#define NUM_SAMPLES_PER_DETECTOR 10
#define NUM_STEPS_PER_SLICE 48 // 180 grados
#define NUM_STEPS_PER_SLICE_Z 3 //30
#define TOTAL_NUM_XY_STEPS 48 // 360
#define CLOCKWISE_DIRECTION true
#define BASE_DELAY 50
#define OK_TOKEN 200
#define DONE_TOKEN 205

/********************************Detectors**********************************/
static const uint8_t DETECTOR_PINS[NUM_DETECTORS] = {A0, A1, A2};

/********************************STEPPER**********************************/
static const uint8_t STEPPER_XY_PINS[4] = {8, 9, 10, 11};
static const uint8_t STEPPER_Z_PINS[4] = {2, 3, 4, 5};


static const int STEP_SEQUENCE [ 8 ][ 4 ] =
   {  {1, 0, 0, 0},
      {1, 1, 0, 0},
      {0, 1, 0, 0},
      {0, 1, 1, 0},
      {0, 0, 1, 0},
      {0, 0, 1, 1},
      {0, 0, 0, 1},
      {1, 0, 0, 1}
   };

 static const int STEP_SEQUENCE_Z [ 4 ][ 4 ] =
   {  {1, 0, 0, 0},
      {0, 0, 1, 0},
      {0, 1, 0, 0},
      {0, 0, 0, 1}
   };

/********************************COMMUNICATIONS**********************************/
String WaitForData() {
  String strbuffer = "";   // for incoming serial data
  String MSG = "";
  while (Serial.available() == 0){
    delay(1);
  }
  while (Serial.available() > 0) {
   //read the incoming byte:
   strbuffer = Serial.readString();
   MSG += strbuffer;
   //Serial.print(incomingByte);
   //incomingByte = "";
  }
  return MSG;
}

void sendMatrix(float matrix[NUM_STEPS_PER_SLICE][NUM_DETECTORS], int rows, int cols){
  Serial.print("s/");
  for (int r=0; r<rows; r++){
    for (int c=0; c<cols; c++){
      Serial.print(matrix[r][c], 4);
      Serial.print('/');
    }
  }
  Serial.print("e/");
}

void sendOK(){
  Serial.print(OK_TOKEN);
  Serial.print('/');
}

void sendDONE(){
  Serial.print(DONE_TOKEN);
  Serial.print('/');
}


/*************************************************************************/
/***********************FUNCTION DEFINITIONS******************************/
/*************************************************************************/

/********************************STEPPER**********************************/
void setupStepper(uint8_t* stepper_pins){
  pinMode(stepper_pins[0], OUTPUT);
  pinMode(stepper_pins[1], OUTPUT);
  pinMode(stepper_pins[2], OUTPUT);
  pinMode(stepper_pins[3], OUTPUT);
}

void setupStepperZ(uint8_t* stepper_pinsZ){
  pinMode(stepper_pinsZ[0], OUTPUT);
  pinMode(stepper_pinsZ[1], OUTPUT);
  pinMode(stepper_pinsZ[2], OUTPUT);
  pinMode(stepper_pinsZ[3], OUTPUT);
}

int stepXY(uint8_t* stepper_pins, unsigned int current_step, bool clockwise){
  int step_stage = current_step % 8;
  digitalWrite( stepper_pins[0], STEP_SEQUENCE[step_stage][ 0] );
  digitalWrite( stepper_pins[1], STEP_SEQUENCE[step_stage][ 1] );
  digitalWrite( stepper_pins[2], STEP_SEQUENCE[step_stage][ 2] );
  digitalWrite( stepper_pins[3], STEP_SEQUENCE[step_stage][ 3] );
  digitalWrite( stepper_pins[0], STEP_SEQUENCE[step_stage][ 0] );
  digitalWrite( stepper_pins[1], STEP_SEQUENCE[step_stage][ 1] );
  digitalWrite( stepper_pins[2], STEP_SEQUENCE[step_stage][ 2] );
  digitalWrite( stepper_pins[3], STEP_SEQUENCE[step_stage][ 3] );
  delay(100);
  if (clockwise) {
    current_step += 1;
  }
  else {
    current_step -= 1;
  }
  return current_step;
}

int stepZ(uint8_t* stepper_pinsZ, unsigned int current_step_z, bool clockwise){
  int step_stage_z = current_step_z % 4;
  digitalWrite( stepper_pinsZ[0], STEP_SEQUENCE_Z[step_stage_z][ 0] );
  digitalWrite( stepper_pinsZ[1], STEP_SEQUENCE_Z[step_stage_z][ 1] );
  digitalWrite( stepper_pinsZ[2], STEP_SEQUENCE_Z[step_stage_z][ 2] );
  digitalWrite( stepper_pinsZ[3], STEP_SEQUENCE_Z[step_stage_z][ 3] );
  delay(100);
  if (clockwise) {
    current_step_z += 1;
  }
  else {
    current_step_z -= 1;
  }
  return current_step_z;
}

void resetXY(uint8_t* stepper_pins, int current_step_position, bool turn_clockwise){
  
  while (current_step_position < TOTAL_NUM_XY_STEPS) {
    current_step_position = stepXY(STEPPER_XY_PINS, current_step_position, CLOCKWISE_DIRECTION);
    delay(BASE_DELAY);
  } 
}

/******************************Detectors**********************************/
float queryDetector(uint8_t pin, int num_samples) {
  /* Return a float average over num_samples mmeasurements
   * at Analog Pin pin;
   */
  float avg_sample = 0.0;
  for (int i=0; i<num_samples; i++) {
    avg_sample += (float) analogRead(pin);
  }
  return avg_sample / num_samples;
}

void queryDetectors(float* measurements, uint8_t* detector_pins, int num_detectors,
                    int num_samples) {
  /* Obtain float avg. mesurements over num_samples per each detector at detector_pins
   * 
   */
  for (int i=0; i < num_detectors; i++) {
    measurements[i] = queryDetector(detector_pins[i], num_samples);
  }
}

/*************************************************************************/
/***************************MAIN SEQUENCE*********************************/
/*************************************************************************/
void collectSlice(float sinogram[NUM_STEPS_PER_SLICE][NUM_DETECTORS]){
  unsigned int current_xy_step = 0;
  unsigned int current_z_step = 0;

  while (current_xy_step < NUM_STEPS_PER_SLICE){
    current_xy_step = stepXY(STEPPER_XY_PINS, current_xy_step, CLOCKWISE_DIRECTION);
    queryDetectors(sinogram[current_xy_step], DETECTOR_PINS, NUM_DETECTORS,
                   NUM_SAMPLES_PER_DETECTOR);
                   delay(100);
  }
  //resetXY(STEPPER_XY_PINS, current_xy_step, CLOCKWISE_DIRECTION);
}

void collectVolume(){
  float sinogram[NUM_STEPS_PER_SLICE][NUM_DETECTORS];
  unsigned int current_z_step = 0;
  String msg;
  delay(1000);
  while (current_z_step < NUM_STEPS_PER_SLICE_Z){
    //delay(5000);
    //msg = WaitForData();
    //if (msg == "n"){
      current_z_step = stepZ(STEPPER_Z_PINS, current_z_step, CLOCKWISE_DIRECTION);
      collectSlice(sinogram);
      //sendOK();
      sendMatrix(sinogram, NUM_STEPS_PER_SLICE, NUM_DETECTORS);
      delay(100);
    //}
  }
  //sendDONE();
}

void setup() {
 //PINS DE STEPPER W
  setupStepper(STEPPER_XY_PINS);
  setupStepperZ(STEPPER_Z_PINS);

  
  //Inicializacion serial
  Serial.begin(9600);
  
}

// Llama a guardarDatos -- genera secuencia de pines
void loop() {
  //float sinogram[NUM_STEPS_PER_SLICE][NUM_DETECTORS];
  String msg;
  msg = WaitForData();
  if (msg == "m"){
    collectVolume();
    //sendMatrix(sinogram, NUM_STEPS_PER_SLICE, NUM_DETECTORS);
  }
  else{
    //Serial.print(msg);
  }
  delay(500);
  
  
}
 
/*Función LECTURA: 
 */ 
float leerDatos (int pinNumber, int numMeasures){

  float voltage[numMeasures];
  float sum = 0;
  
  for (int i = 0; i < numMeasures; i++){
        int SensorValue = analogRead(pinNumber);
        voltage[i] = SensorValue * (5.0 / 1023);
    }

    for (int j = 0 ; j < numMeasures; j++){
      sum += voltage [j] ;
      }

      float average = sum/numMeasures;
        
  return average;
  
  }
