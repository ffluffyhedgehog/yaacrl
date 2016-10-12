/*
 * Copyright 2016 yaacrl contributors
*/

#ifndef SPECGRAM_H_
#define SPECGRAM_H_

typedef struct {
  float** sg;  // Specgram itself
  int windows;  // X axis - quantity of windows
  int freq;  // Y axis - quantity of freq bins
}  Specgram;

/*
 * Calculates signal specgram using fft.
 *
 * Returns a specgram.
 */
Specgram gen_specgram(float * from,  // Samples
              int signal_length,     // Quantity of samples given
              int window_size,       // Quantity of samples per specgram window.
                                     // 2048 or 4096 recommended.
                                     // Must be a power of 2.
              float overlap);        // How windows overlap each other.
                                     // Must be less than 1. 0.5 recommended

/*
 * Builds a Hann window function in window.
 * * * * * * * * * * * * * * * * * * * * *
 * No need in initializing window as well
 */
void hanning(int window_size, float * window);

/*
 * Transforms complex falue into float, also turning it to decibell scale.
 */

inline float log_transform(kiss_fft_cpx val);

#endif  // SPECGRAM_H_
