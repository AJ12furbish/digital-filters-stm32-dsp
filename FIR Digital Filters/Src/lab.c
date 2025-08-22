/*
This file contains the functions you will modify in order to complete the labs.
These functions allow you to perform DSP on a per-frame or per-sample basis.
When processing on a per-sample basis, there are separate functions for left and right channels.
When processing on a per-frame basis, the left and right channels are interleaved.
The sample rate and frame size can be modified in lab.h.
You can also configure which of the four functions are active in lab.h
*/

#include "main.h"
#include "lab.h"

//These functions allow estimation of the number of elapsed clock cycles
extern void tic(void);
extern uint32_t toc(void);

//variables used for the spectrum visualization
extern arm_rfft_fast_instance_f32 fft_inst;
extern float32_t fft_in[FRAME_SIZE/4];
extern float32_t fft_out[FRAME_SIZE/4];
extern float32_t fft_mag[FRAME_SIZE/8];

//declare variables local to this file
uint32_t elapsed_cycles;

/*
This function will be called once before beginning the main program loop.
This is the best place to build a lookup table.
*/
float32_t b[31] = {0.000487,0.005702,0.000058,0.005160,0.020904,-0.018583,-0.078442,-0.013990,0.095721,0.050080,-0.016382,0.049791,-0.020496,-0.251282,-0.127792,0.301475,0.301475,-0.127792,-0.251282,-0.020496,0.049791,-0.016382,0.050080,0.095721,-0.013990,-0.078442,-0.018583,0.020904,0.005160,0.000058,0.005702,0.000487};
float32_t x[31] = {0};
float32_t y = 0;
int32_t position = 0;
float32_t filter_in[FRAME_SIZE/4] = {0};
float32_t filter_out[FRAME_SIZE/4] = {0};
float32_t state[31+(FRAME_SIZE/4)-1] = {0};
arm_fir_instance_f32 filter_instance;
void lab_init(int16_t* output_buffer)
{
	arm_fir_init_f32(&filter_instance, 31, b, state, FRAME_SIZE/4);
	return;
}

/*
This function will be called each time a complete frame of data is recorded.
Modify this function as needed.
Default behavior:
	1. Deinterleave the left and right channels
	2. Combine the two channels (by addition) into one signal
	3. Save the result to the fft_in buffer which will be used for the display
	4. The original audio buffer is left unchanged (passthrough)
*/
void process_input_buffer(int16_t* input_buffer)
{
	int16_t left_sample;
	int16_t right_sample;
	for (uint32_t i_sample = 0; i_sample < FRAME_SIZE/2; i_sample+=1)
	{
		left_sample = input_buffer[i_sample];
		i_sample +=1;
		right_sample = input_buffer[i_sample];
		fft_in[i_sample/2] =  (((float32_t) left_sample) + ((float32_t) right_sample))/2;
	}
	arm_rfft_fast_f32(&fft_inst, fft_in, fft_out, 0);
	arm_cmplx_mag_f32(fft_out, fft_mag, FRAME_SIZE/8);
	return;
}

void process_input_buffer2(int16_t* input_buffer)
{

	int16_t left_sample;
	int16_t right_sample;

	for (uint32_t i_sample = 0; i_sample < FRAME_SIZE/2; i_sample+=2)
	{
	    filter_in[i_sample/2] = ((float32_t)input_buffer[i_sample])*INPUT_SCALE_FACTOR;
	}

	arm_fir_f32(&filter_instance, filter_in, filter_out, FRAME_SIZE/4);

	for (uint32_t i_sample = 0; i_sample < FRAME_SIZE/2; i_sample+=1)
	{
	     input_buffer[i_sample] = OUTPUT_SCALE_FACTOR*filter_out[i_sample/2];
	     i_sample+=1;
	     input_buffer[i_sample] = 0;
	}

	return;
}

/*
This function provides access to each individual sample that is incoming on the left channel.
The returned value will be sent to the left channel DAC.
Default behavior:
	1. Copy input to output without modification (passthrough)
	2. Estimate the number of cycles that have elapsed during the function call
*/


int16_t mod(int16_t num, int16_t mod){
	if(num < 0)
		return num + mod;

	if(num == mod)
		return 0;

	if(num > mod){
		while(num > mod){
			num -= mod;
		}
		return num;
	}

	return num;
}

void circular_buffer_shift(float32_t new_element)
{
    position = position-1;
    position = mod(position, 31);
    x[position] = new_element;
}

int16_t process_left_sample(int16_t input_sample)
{
	tic();
	int16_t output_sample;
	circular_buffer_shift(((float32_t) input_sample) * INPUT_SCALE_FACTOR);

	y = 0;

	for(int16_t i = 0; i < 31; i++){
		y += b[i]*x[(position + i)%31];
	}

	output_sample = y * OUTPUT_SCALE_FACTOR;
	elapsed_cycles = toc();
	return output_sample;
}

// Took 23 Cycles
int16_t process_left_sample_circularbuffer(int16_t input_sample)
{
	tic();
	int16_t output_sample;
	circular_buffer_shift(((float32_t) input_sample) * INPUT_SCALE_FACTOR);

	y = 0;

	for(int16_t i = 0; i < 31; i++){
		y += b[i]*x[(position + i)%31];
	}

	output_sample = y * OUTPUT_SCALE_FACTOR;
	elapsed_cycles = toc();
	return output_sample;
}

// Took 29 Cycles
int16_t process_left_sample_linearbuffer(int16_t input_sample)
{
	tic();
	int16_t output_sample;
	x[0] = ((float32_t) input_sample) * INPUT_SCALE_FACTOR;
	y = 0;
	for(int16_t i = 0; i < 31; i++){
		y += b[i]*x[i];
	}

	for(int16_t i = 30; i > 0; i--){
		x[i] = x[i-1];
	}

	output_sample = y * OUTPUT_SCALE_FACTOR;
	elapsed_cycles = toc();
	return output_sample;
}

/*
This function provides access to each individual sample that is incoming on the left channel.
The returned value will be sent to the right channel DAC.
Default behavior:
	1. Copy input to output without modification (passthrough)
	2. Estimate the number of cycles that have elapsed during the function call
*/
int16_t process_right_sample(int16_t input_sample)
{
	tic();
	int16_t output_sample;
	output_sample = input_sample;
	elapsed_cycles = toc();
	return output_sample;
}

/*
This function provides another opportunity to access the frame of data
The default behavior is to leave the buffer unchanged (passthrough)
The buffer you see here will have any changes that occurred to the signal due to:
	1. the process_input_buffer function
	2. the process_left_sample and process_right_sample functions
*/
void process_output_buffer(int16_t* output_buffer)
{
	return;
}
