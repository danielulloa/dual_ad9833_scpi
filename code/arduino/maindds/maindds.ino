#include "src/SCPIParser/SCPIParser.h"
#include <MD_AD9833.h>
#include <SPI.h>

struct scpi_parser_context ctx;

scpi_error_t default_status(struct scpi_parser_context* context, struct scpi_token* command);
scpi_error_t identify(struct scpi_parser_context* context, struct scpi_token* command);
//scpi_error_t get_frequency(struct scpi_parser_context* context, struct scpi_token* command);
scpi_error_t set_frequency(struct scpi_parser_context* context, struct scpi_token* command);
scpi_error_t set_amplitude(struct scpi_parser_context* context, struct scpi_token* command);
scpi_error_t set_offset(struct scpi_parser_context* context, struct scpi_token* command);
scpi_error_t set_phase(struct scpi_parser_context* context, struct scpi_token* command);
scpi_error_t set_function(struct scpi_parser_context* context, struct scpi_token* command);
scpi_error_t set_frequency2(struct scpi_parser_context* context, struct scpi_token* command);
scpi_error_t set_amplitude2(struct scpi_parser_context* context, struct scpi_token* command);
scpi_error_t set_offset2(struct scpi_parser_context* context, struct scpi_token* command);
scpi_error_t set_phase2(struct scpi_parser_context* context, struct scpi_token* command);
scpi_error_t set_function2(struct scpi_parser_context* context, struct scpi_token* command);
scpi_error_t set_sweepstart(struct scpi_parser_context* context, struct scpi_token* command);
scpi_error_t set_sweepstop(struct scpi_parser_context* context, struct scpi_token* command);
scpi_error_t set_sweeptime(struct scpi_parser_context* context, struct scpi_token* command);
scpi_error_t set_sweep(struct scpi_parser_context* context, struct scpi_token* command);
scpi_error_t set_sweep2(struct scpi_parser_context* context, struct scpi_token* command);


// Conexiones de los IC AD9833
#define DATA  11	// SPI Data pin
#define CLK   13	// SPI Clock pin
#define FSYNC 8	// Chip enable IC1
#define FSYNC2 12  // Chip enable IC2

// Instanciacion de los componentes
MD_AD9833	AD(DATA, CLK, FSYNC);  
MD_AD9833 AD2(DATA, CLK, FSYNC2);

int ldr1 = 10;
int ldr2 = 9;
int offset1 = 6;
int offset2 = 5;

float startHz = 1000, stopHz = 5000, incHz = 1, sweepTimeSec = 60.0;

/*
 * We need to manually call serialEventRun since we're not returning through the loop()
 * function while inside the test functions. If a character is in the receive buffer,
 * exit the test function. We also blink the I'm Alive LED to give a visual indication
 * that the program is not hung up.
while ( true ) {
  for ( int i = 0 ; i <= 10; i += 1 ) {
    YIELD_ON_CHAR
    Serial.println(i);
    delay(100);
    }
}
 */
#define YIELD_ON_CHAR     if ( serialEventRun ) serialEventRun(); \
                          if ( Serial.available() ) return;


void setup() {

  pinMode(ldr1, OUTPUT);
  pinMode(ldr2, OUTPUT);
  pinMode(offset1, OUTPUT);
  pinMode(offset2, OUTPUT);

  TCCR0B = (TCCR0B & 0b11111000) | 0x01; //62.5 [kHz]
  TCCR1B = (TCCR1B & 0b11111000) | 0x01; //31.37255 [kHz]


  struct scpi_command* awg1;
  struct scpi_command* awg2;

  /* First, initialise the parser. */
  scpi_init(&ctx);

  /*
   * After initialising the parser, we set up the command tree.  Ours is
   *
   *  *IDN?         -> identify
   *  :AWG
   *    :FREQuency  -> set_frequency
   *    :FREQuency? -> get_frequency
   */
  scpi_register_command(ctx.command_tree, SCPI_CL_SAMELEVEL, "*IDN?", 5, "*IDN?", 5, identify);
  scpi_register_command(ctx.command_tree,SCPI_CL_SAMELEVEL, "*RST", 4, "*RST", 4, default_status);

  awg1 = scpi_register_command(ctx.command_tree, SCPI_CL_CHILD, "AWG1", 4, "AWG1", 4, NULL);
  scpi_register_command(awg1, SCPI_CL_CHILD, "FREQUENCY", 9, "FREQ", 4, set_frequency);
  //scpi_register_command(awg1, SCPI_CL_CHILD, "FREQUENCY?", 10, "FREQ?", 5, get_frequency);
  scpi_register_command(awg1, SCPI_CL_CHILD, "AMPLITUDE", 9, "AMP", 3, set_amplitude);
  //scpi_register_command(awg1, SCPI_CL_CHILD, "AMPLITUDE?", 10, "AMP?", 4, get_amplitude);
  scpi_register_command(awg1, SCPI_CL_CHILD, "FUNCTION", 8, "FUNC", 4, set_function);
  //scpi_register_command(awg1, SCPI_CL_CHILD, "FUNCTION?", 9, "FUNC?", 5, get_function);
  scpi_register_command(awg1, SCPI_CL_CHILD, "PHASE", 5, "PHA", 3, set_phase);
  //scpi_register_command(awg1, SCPI_CL_CHILD, "PHASE?", 6, "PHA?", 4, get_phase);
  scpi_register_command(awg1, SCPI_CL_CHILD, "OFFSET", 6, "OFFS", 4, set_offset);
  //scpi_register_command(awg1, SCPI_CL_CHILD, "OFFSET?", 7, "OFFS?", 5, get_offset);
  scpi_register_command(awg1, SCPI_CL_CHILD, "SWEEPSTART", 10, "SWESTART", 8, set_sweepstart);
  scpi_register_command(awg1, SCPI_CL_CHILD, "SWEEPSTOP", 9, "SWESTOP", 7, set_sweepstop);
  scpi_register_command(awg1, SCPI_CL_CHILD, "SWEEPTIME", 9, "SWETIME", 7, set_sweeptime);
  scpi_register_command(awg1, SCPI_CL_CHILD, "SWEEP", 5, "SWE", 3, set_sweep);

  awg2 = scpi_register_command(ctx.command_tree, SCPI_CL_CHILD, "AWG2", 4, "AWG2", 4, NULL);
  scpi_register_command(awg2, SCPI_CL_CHILD, "FREQUENCY", 9, "FREQ", 4, set_frequency2);
  //scpi_register_command(awg2, SCPI_CL_CHILD, "FREQUENCY?", 10, "FREQ?", 5, get_frequency2);
  scpi_register_command(awg2, SCPI_CL_CHILD, "AMPLITUDE", 9, "AMP", 3, set_amplitude2);
  //scpi_register_command(awg2, SCPI_CL_CHILD, "AMPLITUDE?", 10, "AMP?", 4, get_amplitude2);
  scpi_register_command(awg2, SCPI_CL_CHILD, "FUNCTION", 8, "FUNC", 4, set_function2);
  //scpi_register_command(awg2, SCPI_CL_CHILD, "FUNCTION?", 9, "FUNC?", 5, get_function2);
  scpi_register_command(awg2, SCPI_CL_CHILD, "PHASE", 5, "PHA", 3, set_phase2);
  //scpi_register_command(awg2, SCPI_CL_CHILD, "PHASE?", 6, "PHA?", 4, get_phase2);
  scpi_register_command(awg2, SCPI_CL_CHILD, "OFFSET", 6, "OFFS", 4, set_offset2);
  //scpi_register_command(awg2, SCPI_CL_CHILD, "OFFSET?", 7, "OFFS?", 5, get_offset2);
  scpi_register_command(awg2, SCPI_CL_CHILD, "SWEEPSTART", 10, "SWESTART", 8, set_sweepstart);
  scpi_register_command(awg2, SCPI_CL_CHILD, "SWEEPSTOP", 9, "SWESTOP", 7, set_sweepstop);
  scpi_register_command(awg2, SCPI_CL_CHILD, "SWEEPTIME", 9, "SWETIME", 7, set_sweeptime);
  scpi_register_command(awg2, SCPI_CL_CHILD, "SWEEP", 5, "SWE", 3, set_sweep2);

  AD.begin();
  AD2.begin();
  AD.setFrequency(MD_AD9833::CHAN_0, 1000);
  AD.setFrequency(MD_AD9833::CHAN_1, 1000);
  AD2.setFrequency(MD_AD9833::CHAN_0, 1000);
  AD2.setFrequency(MD_AD9833::CHAN_1, 1000);
  AD.setPhase(MD_AD9833::CHAN_0, 900);
  AD2.setPhase(MD_AD9833::CHAN_0, 1800);
  AD.setPhase(MD_AD9833::CHAN_1, 900);  
  AD2.setPhase(MD_AD9833::CHAN_1, 1800);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  char line_buffer[256];
  unsigned char read_length;

  while(1)
  {
    /* Read in a line and execute it. */
    read_length = Serial.readBytesUntil('\n', line_buffer, 256);
    if(read_length > 0)
    {
      scpi_execute_command(&ctx, line_buffer, read_length);
    }
  }
}


scpi_error_t default_status(struct scpi_parser_context* context, struct scpi_token* command)
{
  scpi_free_tokens(command);
  AD.begin();
  AD.setFrequency(MD_AD9833::CHAN_0, 1000);
  AD.setPhase(MD_AD9833::CHAN_0, 900);
  AD2.begin();
  AD2.setFrequency(MD_AD9833::CHAN_0, 1000);
  AD2.setPhase(MD_AD9833::CHAN_0, 1800);
  startHz = 1000, stopHz = 5000, incHz = 1, sweepTimeSec = 5.0;
  return SCPI_SUCCESS;
}

/*
 * Answer to *IDN?
 * "Manufacturer, Model, Serial Number, Version number"
 */
scpi_error_t identify(struct scpi_parser_context* context, struct scpi_token* command)
{
  
  scpi_free_tokens(command);
  Serial.println("ULLOA,DUAL DDS 2020,P0001,V1.0");
  return SCPI_SUCCESS;
}

/**
 * Set the frequency1.
 */
scpi_error_t set_frequency(struct scpi_parser_context* context, struct scpi_token* command)
{
  struct scpi_token* args;
  struct scpi_numeric output_numeric;
  unsigned char output_value;

  args = command;

  while(args != NULL && args->type == 0)
  {
    args = args->next;
  }

  output_numeric = scpi_parse_numeric(args->value, args->length, 1e3, 0, 1e6);
  if(output_numeric.length == 0 ||
    (output_numeric.length == 2 && output_numeric.unit[0] == 'H' && output_numeric.unit[1] == 'z'))
  {
    AD.setFrequency(MD_AD9833::CHAN_0, (unsigned long)constrain(output_numeric.value, 0, 1e6));
  }
  else
  {
    scpi_error error;
    error.id = -200;
    error.description = "Command error;Invalid unit";
    error.length = 26;
    
    scpi_queue_error(&ctx, error);
    scpi_free_tokens(command);
    return SCPI_SUCCESS;
  }

  scpi_free_tokens(command);

  return SCPI_SUCCESS;
}
/**
 * Set the frequency2.
 */
scpi_error_t set_frequency2(struct scpi_parser_context* context, struct scpi_token* command)
{
  struct scpi_token* args;
  struct scpi_numeric output_numeric;
  unsigned char output_value;

  args = command;

  while(args != NULL && args->type == 0)
  {
    args = args->next;
  }

  output_numeric = scpi_parse_numeric(args->value, args->length, 1e3, 0, 1e6);
  if(output_numeric.length == 0 ||
    (output_numeric.length == 2 && output_numeric.unit[0] == 'H' && output_numeric.unit[1] == 'z'))
  {
    AD2.setFrequency(MD_AD9833::CHAN_0, (unsigned long)constrain(output_numeric.value, 0, 1e6));
  }
  else
  {
    scpi_error error;
    error.id = -200;
    error.description = "Command error;Invalid unit";
    error.length = 26;
    
    scpi_queue_error(&ctx, error);
    scpi_free_tokens(command);
    return SCPI_SUCCESS;
  }

  scpi_free_tokens(command);

  return SCPI_SUCCESS;
}
/**
 * Set the amplitude1.
 */
scpi_error_t set_amplitude(struct scpi_parser_context* context, struct scpi_token* command)
{
  struct scpi_token* args;
  struct scpi_numeric output_numeric;
  unsigned char output_value;

  args = command;

  while(args != NULL && args->type == 0)
  {
    args = args->next;
  }

  output_numeric = scpi_parse_numeric(args->value, args->length, 50, 0, 255);
  if(output_numeric.length == 0 )
  {
    analogWrite(ldr1, (unsigned long)constrain(output_numeric.value, 0, 255));
  }
  else
  {
    scpi_error error;
    error.id = -200;
    error.description = "Command error;Invalid unit";
    error.length = 26;
    
    scpi_queue_error(&ctx, error);
    scpi_free_tokens(command);
    return SCPI_SUCCESS;
  }

  scpi_free_tokens(command);

  return SCPI_SUCCESS;
}
/**
 * Set the amplitude2.
 */
scpi_error_t set_amplitude2(struct scpi_parser_context* context, struct scpi_token* command)
{
  struct scpi_token* args;
  struct scpi_numeric output_numeric;
  unsigned char output_value;

  args = command;

  while(args != NULL && args->type == 0)
  {
    args = args->next;
  }

  output_numeric = scpi_parse_numeric(args->value, args->length, 50, 0, 255);
  if(output_numeric.length == 0 )
  {
    analogWrite(ldr2, (unsigned long)constrain(output_numeric.value, 0, 255));
  }
  else
  {
    scpi_error error;
    error.id = -200;
    error.description = "Command error;Invalid unit";
    error.length = 26;
    
    scpi_queue_error(&ctx, error);
    scpi_free_tokens(command);
    return SCPI_SUCCESS;
  }

  scpi_free_tokens(command);

  return SCPI_SUCCESS;
}
/***************************
 * 
 ****   Set offset1. *******
 *
 ***************************/
scpi_error_t set_offset(struct scpi_parser_context* context, struct scpi_token* command)
{
  struct scpi_token* args;
  struct scpi_numeric output_numeric;
  unsigned char output_value;

  args = command;

  while(args != NULL && args->type == 0)
  {
    args = args->next;
  }

  output_numeric = scpi_parse_numeric(args->value, args->length, 50, 0, 255);
  if(output_numeric.length == 0 )
  {
    analogWrite(offset1, (unsigned long)constrain(output_numeric.value, 0, 255));
  }
  else
  {
    scpi_error error;
    error.id = -200;
    error.description = "Command error;Invalid unit";
    error.length = 26;
    
    scpi_queue_error(&ctx, error);
    scpi_free_tokens(command);
    return SCPI_SUCCESS;
  }

  scpi_free_tokens(command);

  return SCPI_SUCCESS;
}

/**
 * Set offset2.
 */
scpi_error_t set_offset2(struct scpi_parser_context* context, struct scpi_token* command)
{
  struct scpi_token* args;
  struct scpi_numeric output_numeric;
  unsigned char output_value;

  args = command;

  while(args != NULL && args->type == 0)
  {
    args = args->next;
  }

  output_numeric = scpi_parse_numeric(args->value, args->length, 50, 0, 255);
  if(output_numeric.length == 0 )
  {
    analogWrite(offset2, (unsigned long)constrain(output_numeric.value, 0, 255));
  }
  else
  {
    scpi_error error;
    error.id = -200;
    error.description = "Command error;Invalid unit";
    error.length = 26;
    
    scpi_queue_error(&ctx, error);
    scpi_free_tokens(command);
    return SCPI_SUCCESS;
  }

  scpi_free_tokens(command);

  return SCPI_SUCCESS;
}

/**
 * Set phase1.
 */
scpi_error_t set_phase(struct scpi_parser_context* context, struct scpi_token* command)
{
  struct scpi_token* args;
  struct scpi_numeric output_numeric;
  unsigned char output_value;

  args = command;

  while(args != NULL && args->type == 0)
  {
    args = args->next;
  }

  output_numeric = scpi_parse_numeric(args->value, args->length, 0, 0, 3600);
  if(output_numeric.length == 0 )
  {
    AD.setPhase(MD_AD9833::CHAN_0, (unsigned long)constrain(output_numeric.value, 0, 3600));
  }
  else
  {
    scpi_error error;
    error.id = -200;
    error.description = "Command error;Invalid unit";
    error.length = 26;
    
    scpi_queue_error(&ctx, error);
    scpi_free_tokens(command);
    return SCPI_SUCCESS;
  }

  scpi_free_tokens(command);

  return SCPI_SUCCESS;
}

/**
 * Set phase2.
 */
scpi_error_t set_phase2(struct scpi_parser_context* context, struct scpi_token* command)
{
  struct scpi_token* args;
  struct scpi_numeric output_numeric;
  unsigned char output_value;

  args = command;

  while(args != NULL && args->type == 0)
  {
    args = args->next;
  }

  output_numeric = scpi_parse_numeric(args->value, args->length, 0, 0, 3600);
  if(output_numeric.length == 0 )
  {
    AD2.setPhase(MD_AD9833::CHAN_0, (unsigned long)constrain(output_numeric.value, 0, 3600));
  }
  else
  {
    scpi_error error;
    error.id = -200;
    error.description = "Command error;Invalid unit";
    error.length = 26;
    
    scpi_queue_error(&ctx, error);
    scpi_free_tokens(command);
    return SCPI_SUCCESS;
  }

  scpi_free_tokens(command);

  return SCPI_SUCCESS;
}

/**
 * Set function1.
 */
scpi_error_t set_function(struct scpi_parser_context* context, struct scpi_token* command) {  
  struct scpi_token* args;    
  
  args = command;  
  while(args != NULL && args->type == 0) {  
    args = args->next;  
  }  
  
  
  if (args->length == 1) {  
    if (args->value[0] == 'S') {  
      AD.setMode(MD_AD9833::MODE_SINE);  
    }  
    if (args->value[0] == 'T') {  
      AD.setMode(MD_AD9833::MODE_TRIANGLE);  
    }
    if (args->value[0] == 'C') {  
      AD.setMode(MD_AD9833::MODE_SQUARE1);  
    }
    if (args->value[0] == 'O') {  
      AD.setMode(MD_AD9833::MODE_OFF);  
    }  
  }
  else
  {
    scpi_error error;
    error.id = -200;
    error.description = "Command error;Invalid unit";
    error.length = 26;
    
    scpi_queue_error(&ctx, error);
    scpi_free_tokens(command);
    return SCPI_SUCCESS;
  }  
 
  
  scpi_free_tokens(command);
  return SCPI_SUCCESS;  
  
  
}  

/**
 * Set function2.
 */
scpi_error_t set_function2(struct scpi_parser_context* context, struct scpi_token* command) {  
  struct scpi_token* args;    
  
  args = command;  
  while(args != NULL && args->type == 0) {  
    args = args->next;  
  }  
  
  
  if (args->length == 1) {  
    if (args->value[0] == 'S') {  
      AD2.setMode(MD_AD9833::MODE_SINE);  
    }  
    if (args->value[0] == 'T') {  
      AD2.setMode(MD_AD9833::MODE_TRIANGLE);  
    }
    if (args->value[0] == 'C') {  
      AD2.setMode(MD_AD9833::MODE_SQUARE1);  
    }
    if (args->value[0] == 'O') {  
      AD2.setMode(MD_AD9833::MODE_OFF);  
    }  
  }
  else
  {
    scpi_error error;
    error.id = -200;
    error.description = "Command error;Invalid unit";
    error.length = 26;
    
    scpi_queue_error(&ctx, error);
    scpi_free_tokens(command);
    return SCPI_SUCCESS;
  }  
 
  
  scpi_free_tokens(command);
  return SCPI_SUCCESS;  
  
  
}

/**
 * Set sweepstart.
 */
scpi_error_t set_sweepstart(struct scpi_parser_context* context, struct scpi_token* command)
{
  struct scpi_token* args;
  struct scpi_numeric output_numeric;
  unsigned char output_value;

  args = command;

  while(args != NULL && args->type == 0)
  {
    args = args->next;
  }

  output_numeric = scpi_parse_numeric(args->value, args->length, 1000, 0, 9.9e6);
  if(output_numeric.length == 0 )
  {
    startHz = (unsigned long)constrain(output_numeric.value, 0, 9.9e6);
  }
  else
  {
    scpi_error error;
    error.id = -200;
    error.description = "Command error;Invalid unit";
    error.length = 26;
    
    scpi_queue_error(&ctx, error);
    scpi_free_tokens(command);
    return SCPI_SUCCESS;
  }

  scpi_free_tokens(command);

  return SCPI_SUCCESS;
}

/**
 * Set sweepstop.
 */
scpi_error_t set_sweepstop(struct scpi_parser_context* context, struct scpi_token* command)
{
  struct scpi_token* args;
  struct scpi_numeric output_numeric;
  unsigned char output_value;

  args = command;

  while(args != NULL && args->type == 0)
  {
    args = args->next;
  }

  output_numeric = scpi_parse_numeric(args->value, args->length, 5000, 0, 10e6);
  if(output_numeric.length == 0 )
  {
    stopHz = (unsigned long)constrain(output_numeric.value, 0, 10e6);
  }
  else
  {
    scpi_error error;
    error.id = -200;
    error.description = "Command error;Invalid unit";
    error.length = 26;
    
    scpi_queue_error(&ctx, error);
    scpi_free_tokens(command);
    return SCPI_SUCCESS;
  }

  scpi_free_tokens(command);

  return SCPI_SUCCESS;
}

/**
 * Set sweeptime.
 */
scpi_error_t set_sweeptime(struct scpi_parser_context* context, struct scpi_token* command)
{
  struct scpi_token* args;
  struct scpi_numeric output_numeric;
  unsigned char output_value;

  args = command;

  while(args != NULL && args->type == 0)
  {
    args = args->next;
  }

  output_numeric = scpi_parse_numeric(args->value, args->length, 5, 0.1, 5000);
  if(output_numeric.length == 0 )
  {
    sweepTimeSec = (unsigned long)constrain(output_numeric.value, 0.1, 5000);
  }
  else
  {
    scpi_error error;
    error.id = -200;
    error.description = "Command error;Invalid unit";
    error.length = 26;
    
    scpi_queue_error(&ctx, error);
    scpi_free_tokens(command);
    return SCPI_SUCCESS;
  }

  scpi_free_tokens(command);

  return SCPI_SUCCESS;
}

/**
 * Set sweep1.
 */
scpi_error_t set_sweep(struct scpi_parser_context* context, struct scpi_token* command)
{
  
  scpi_free_tokens(command);
      // Calculate the delay between each increment.
    uint16_t numMsecPerStep = (sweepTimeSec * 1000.0) / ((uint16_t)((stopHz - startHz) / incHz) + 1);
    if ( numMsecPerStep == 0 ) numMsecPerStep = 1;

    // Apply a signal to the output. If phaseReg is not supplied, then
    // a phase of 0.0 is applied to the same register as freqReg
    AD.setFrequency(MD_AD9833::CHAN_0, startHz);

    while ( true ) {
      
        AD.setFrequency(MD_AD9833::CHAN_0, startHz);

        for ( float i = startHz ; i <= stopHz; i += incHz ) {
            YIELD_ON_CHAR
            AD.setFrequency(MD_AD9833::CHAN_0, i);
            delay(numMsecPerStep); 
        }
    }
  return SCPI_SUCCESS;
}

/**
 * Set sweep2.
 */
scpi_error_t set_sweep2(struct scpi_parser_context* context, struct scpi_token* command)
{
  
  scpi_free_tokens(command);
      // Calculate the delay between each increment.
    uint16_t numMsecPerStep = (sweepTimeSec * 1000.0) / ((uint16_t)((stopHz - startHz) / incHz) + 1);
    if ( numMsecPerStep == 0 ) numMsecPerStep = 1;

    // Apply a signal to the output. If phaseReg is not supplied, then
    // a phase of 0.0 is applied to the same register as freqReg
    AD2.setFrequency(MD_AD9833::CHAN_0, startHz);

    while ( true ) {
      
        AD2.setFrequency(MD_AD9833::CHAN_0, startHz);

        for ( float i = startHz ; i <= stopHz; i += incHz ) {
            YIELD_ON_CHAR
            AD2.setFrequency(MD_AD9833::CHAN_0, i);
            delay(numMsecPerStep); 
        }
    }
  return SCPI_SUCCESS;
}
