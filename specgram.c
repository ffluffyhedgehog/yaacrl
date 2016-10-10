#include <stdlib.h>
#include <math.h>

#include "kiss_fft/kiss_fft.h"
#include "specgram.h"

//Used some code from http://ofdsp.blogspot.com/2011/08/short-time-fourier-transform-with-fftw3.html
void specgram(float * from, float ** to, int signal_length, int window_size = 2048, float overlap = 0.5) {
  float * window;
  hanning(window_size, window);
  int chunk_position = 0;
  int read_index, write_index;

  int num_chunks = signal_length/window_size + 1, chunks_processed = 0;
  kiss_fft_cpx ** fft_result = (kiss_fft_cpx**) malloc(num_chunks * sizeof(kiss_fft_cpx*));
  to = (float**) malloc(num_chunks * sizeof(float*));
  for (int i = 0; i < num_chunks; i++) {
    fft_result[i] = (kiss_fft_cpx*) malloc((window_size/2 + 1) * sizeof(kiss_fft_cpx));
	to[i] = (float*) malloc ((window_size/2 + 1) * sizeof(float));
  }

  //Calculate the window function
  for (int i = 0; i < signal_length; ++i) {
    to[i] = from[i]*to[i];
  }

  kiss_fft_cfg cfg = kiss_fft_alloc(1024, 0, NULL, NULL);

  kiss_fft_cpx * cpx_from, *cpx_to;

  // Should we stop reading in chunks?
  int stop = 0;

  // Process each chunk of the signal
  cpx_to = (kiss_fft_cpx*) malloc(window_size * sizeof(kiss_fft_cpx));
  cpx_from = (kiss_fft_cpx*) malloc(window_size * sizeof(kiss_fft_cpx));

  while(chunk_position < signal_length && !bStop) {


    // Copy the chunk into our buffer
    for(int i = 0; i < window_size; i++) {
      read_index = chunk_position + i;
      if(read_index < signal_length) {
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

    // Copy the first (window_size/2 + 1) data points into your spectrogram.
    // We do this because the FFT output is mirrored about the nyquist
    // frequency, so the second half of the data is redundant. This is how
    // Matlab's spectrogram routine works.
    for(int i = 0; i < window_size/2 + 1; i++) {
      write_index = chunk_position + i;
      fft_result[chunks_processed][i].r = cpx_to[i].r;
      fft_result[chunks_processed][i].i = cpx_to[i].i;
    }
    chunk_position += window_size*(1 - overlap);
    chunks_processed++;
  }
  //process the fft_result data into the spectrogram
  for(int i = 0; i < num_chunks; ++i) {
    for(int j = 0; j < window_size/2 + 1; ++j) {
      to[i][j] = log_transform(fft_result[i][j]);
	}
  }
  return;
}

inline float log_transform(kiss_fft_cpx val) {
	return 10*log10(val.i * val.i + val.r * val.r);
}

void hanning(int window_size; float * window) {
  window = (float*) malloc(window_size * sizeof(float));
  for (int i = 0; i < window_size; ++i) {
    window[i] = (1 - cos( (2 * M_PI * i) / (window_size - 1) ));
  }
}


