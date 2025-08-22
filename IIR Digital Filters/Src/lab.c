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


int16_t mod(int16_t num, int16_t mod){
	tic();
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

	int16_t elapsed_cycles_mod = toc();
	return num;
}

int32_t position = 0;
float32_t x[7] = {0};
float32_t y[7] = {0};

void circular_buffer_shift_x(float32_t new_element)
{
    position = position-1;
    position = mod(position, 7);
    x[position] = new_element;
}

void circular_buffer_shift_y(float32_t new_element)
{
    position = position-1;
    position = mod(position, 7);
    y[position] = new_element;
}


void lab_init(int16_t* output_buffer)
{
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

/*
This function provides access to each individual sample that is incoming on the left channel.
The returned value will be sent to the left channel DAC.
Default behavior:
	1. Copy input to output without modification (passthrough)
	2. Estimate the number of cycles that have elapsed during the function call
*/
float32_t a[7] = {1.000000,-1.331603,1.736054,-1.467887,1.431451,-0.696109,0.383105};
float32_t b[7] = {0.069259,-0.005620,-0.188714,0.000000,0.188714,0.005620,-0.069259};

float32_t B[3][3] = {
		{1.000000,1.889818,1.000000},
		     {1.000000,-1.970969,1.000000},
		     {1.000000,0.000000,-1.000000}
};
float32_t A[3][3] = {
		{1.000000,-1.447046,0.862778},
		     {1.000000,0.622267,0.802149},
		     {1.000000,-0.506824,0.553559}
};
float32_t Y[3][3] = {0};
float32_t X[3][3] = {0};
float32_t G[3] = {0.340877,0.340877,0.596044};

int16_t process_left_sample(int16_t input_sample)
{
	tic();
	int16_t output_sample;

	//circular_buffer_shift_x(((float32_t) input_sample) * INPUT_SCALE_FACTOR);
	X[0][0] = (((float32_t) input_sample) * INPUT_SCALE_FACTOR) * G[0];

	float32_t feedforward = B[0][0] * X[0][0];
	float32_t feedback = 0;
	float32_t newin = 0;

	for(int16_t i = 1; i < 3; i++){
		feedforward += B[0][i]*X[0][i];
		feedback += A[0][i]*Y[0][i];
		newin = feedforward - feedback;
	}
	X[1][0] = newin * G[1];
	for(int16_t i = 0; i < 3; i++){
		feedforward += B[1][i]*X[1][i];
		feedback += A[1][i]*Y[1][i];
		newin = feedforward - feedback;
	}
	X[2][0] = newin * G[2];
	for(int16_t i = 0; i < 3; i++){
		feedforward += B[2][i]*X[2][i];
		feedback += A[2][i]*Y[2][i];
	}

	Y[2][0] = feedforward - feedback;

	output_sample = Y[2][0] * OUTPUT_SCALE_FACTOR;

	for(int16_t i = 0; i < 3; i++){
		for(int16_t j = 2; j > 0; j--){
			X[i][j] = X[i][j-1];
			Y[i][j] = Y[i][j-1];
		}
	}

	elapsed_cycles = toc();
	return output_sample;
}

int16_t process_left_sample_circularbuffer(int16_t input_sample)
{
	tic();
	int16_t output_sample;

	//circular_buffer_shift_x(((float32_t) input_sample) * INPUT_SCALE_FACTOR);
	x[position] = (((float32_t) input_sample) * INPUT_SCALE_FACTOR);

	float32_t feedforward = b[0] * x[position];
	float32_t feedback = 0;

	for(int16_t i = 1; i < 7; i++){
		feedforward += b[i]*x[mod(position + i, 7)];
		feedback += a[i]*y[mod(position + i, 7)];
	}

	y[position] = (feedforward - feedback);

	output_sample = y[position] * OUTPUT_SCALE_FACTOR;
	position -= 1;
	position = mod(position, 7);
	elapsed_cycles = toc();
	return output_sample;
}


int16_t process_left_sample_linearbuffer(int16_t input_sample)
{
	tic();
	int16_t output_sample;
	output_sample = input_sample;
	x[0] = ((float32_t) input_sample) * INPUT_SCALE_FACTOR;

	float32_t feedforward = b[0] * x[0];
	float32_t feedback = 0;


	for(int16_t i = 1; i < 7; i++){
		feedforward += b[i]*x[i];
		feedback += a[i]*y[i];
	}


	y[0] = feedforward - feedback;

	output_sample = y[0] * OUTPUT_SCALE_FACTOR;

	for(int16_t i = 6; i > 0; i--){
		x[i] = x[i-1];
		y[i] = y[i-1];
	}

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
