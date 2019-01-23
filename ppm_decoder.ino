#define PPM_CHANNELS_COUNT 8
#define PPM_PIN 2 // pin with interrupt (2 and 3 for UNO)
#define TIME_BETWEEN_PPM_BLOCKS 2500 // interval for separating PPM blocks (in microseconds)

#define INTERRUPTS_SET_SIZE (PPM_CHANNELS_COUNT + 1) * 2 + 1
#define CHANNEL_WINDOW 1000 // channel "duration" in microseconds

unsigned long int current_peak_time = 0;
unsigned long int previous_peak_time = 0;
unsigned long int time_between_peaks = 0;

int current_interrupts_set[INTERRUPTS_SET_SIZE] = {};
int completed_interrupts_set[INTERRUPTS_SET_SIZE] = {};
int ppm_block[PPM_CHANNELS_COUNT] = {};

short peak_counter = 0;

void setup() {
  Serial.begin(9600);
  pinMode(PPM_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PPM_PIN), read_ppm_port, FALLING);
}

void loop() {
  read_ppm_block();

  Serial.print(ppm_block[0]);Serial.print("\t");
  Serial.print(ppm_block[1]);Serial.print("\t");
  Serial.print(ppm_block[2]);Serial.print("\t");
  Serial.print(ppm_block[3]);Serial.print("\t");
  Serial.print(ppm_block[4]);Serial.print("\t");
  Serial.print(ppm_block[5]);Serial.print("\t");
  Serial.print(ppm_block[6]);Serial.print("\t");
  Serial.print(ppm_block[7]);Serial.print("\n");

  delay(100);
}


void read_ppm_port()  {
  current_peak_time = micros();
  time_between_peaks = current_peak_time - previous_peak_time;
  previous_peak_time = current_peak_time;

  // store INTERRUPTS_SET_SIZE values in array
  current_interrupts_set[peak_counter] = time_between_peaks;
  peak_counter++;

  // save set of interrupts after INTERRUPTS_SET_SIZE readings
  if (peak_counter == INTERRUPTS_SET_SIZE) {
    for(short i = 0; i < INTERRUPTS_SET_SIZE; i++) {
      completed_interrupts_set[i] = current_interrupts_set[i];
    }
    
    peak_counter = 0;
  }
}

void read_ppm_block(){
  short start_index = 0;
  // detect separation space TIME_BETWEEN_PPM_BLOCKS microseconds inside completed PPM block
  for(short i = INTERRUPTS_SET_SIZE; i >= 0; i--) {
    if(completed_interrupts_set[i] > TIME_BETWEEN_PPM_BLOCKS) {
      start_index = i + 1;
    }
  }

  // assign PPM_CHANNELS_COUNT channel values after separation space
  for(short i = 0; i < PPM_CHANNELS_COUNT; i++) {
    ppm_block[i] = (completed_interrupts_set[start_index + i] - CHANNEL_WINDOW);
  }
}
