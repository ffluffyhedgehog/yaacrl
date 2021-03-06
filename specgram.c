/*
 * Copyright 2016 yaacrl contributors
*/

#include <stdlib.h>
#include <math.h>
// #include <stdio.h>

#include "kiss_fft/kiss_fft.h"
#include "./specgram.h"


/*
 * Calculates signal specgram using fft.
 *
 * Returns a specgram.
 *
 * Used some code from:
 * http://ofdsp.blogspot.com/2011/08/short-time-fourier-transform-with-fftw3.html
 * Actually, reworked it for kiss_fft
 */
Specgram gen_specgram(float * from,  // Samples
              int signal_length,     // Quantity of samples given
              int window_size,       // Quantity of samples per specgram window.
                                     // 2048 or 4096 recommended.
                                     // Must be a power of 2.
              float overlap) {       // How windows overlap each other.
                                     // Must be less than 1. 0.5 recommended
  float *window;
  window = windowing(window_size);
  int chunk_position = 0;
  int read_index, write_index, chunks_processed = 0, num_chunks = 1;
  for (int i = window_size; i < signal_length;) {
    i += window_size;
    i -= window_size*overlap;
    ++num_chunks;
  }

  Specgram to;
  to.freq = window_size/2 + 1;
  to.windows = num_chunks;

  to.sg = (float**) malloc(num_chunks * sizeof(float*));
  for (int i = 0; i < num_chunks; i++) {
    to.sg[i] = (float*) malloc (to.freq * sizeof(float));
  }

  kiss_fft_cfg cfg = kiss_fft_alloc(window_size, 0, NULL, NULL);
  kiss_fft_cpx * cpx_from, *cpx_to;

  // Should we stop reading in chunks?
  int stop = 0;

  // Process each chunk of the signal
  cpx_to = (kiss_fft_cpx*) malloc(window_size * sizeof(kiss_fft_cpx));
  cpx_from = (kiss_fft_cpx*) malloc(window_size * sizeof(kiss_fft_cpx));

  while ( chunk_position < signal_length && !stop ) {
    // Copy the chunk into our buffer
    for (int i = 0; i < window_size; i++) {
      read_index = chunk_position + i;
      if (read_index < signal_length) {
        // Do windowing
        cpx_from[i].r = from[read_index] * window[i];
        cpx_from[i].i = 0.0;
      } else {
        // we have read beyond the signal, so zero-pad it!
        cpx_from[i].r = 0.0;
        cpx_from[i].i = 0.0;
        stop = 1;
      }
    }
    // Perform the FFT on our chunk
    kiss_fft(cfg, cpx_from, cpx_to);

    // Copy the first (window_size/2 + 1) data points into spectrogram.
    // We do this because the FFT output is mirrored about the nyquist
    // frequency, so the second half of the data is redundant. This is how
    // Matlab's spectrogram routine works.
    for (int i = 0; i < to.freq; i++) {
      write_index = chunk_position + i;
	  to.sg[chunks_processed][i] = log_transform(cpx_to[i]);
    }
    chunk_position += window_size*(1 - overlap);
    chunks_processed++;
  }
  free(cpx_from);
  free(cpx_to);
  free(window);
  kiss_fft_free(cfg);
  return to;
}

inline float log_transform(kiss_fft_cpx val) {
  return 10*log10(val.i * val.i + val.r * val.r);
}

float* windowing(int window_size) {
  float* window = (float*) malloc(window_size * sizeof(float));
  for (int i = 0; i < window_size; ++i) {   // hanning function applied
    window[i] = (1 - cos((2 * M_PI * i) / (window_size - 1) ));
  }
  return window;
}


