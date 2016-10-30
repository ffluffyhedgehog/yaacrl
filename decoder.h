/*
 * Copyright 2016 yaacrl contributors, true logic
*/
/*
 * This file is a modified version of 
 * http://truelogic.org/wordpress/2015/09/04/parsing-a-wav-file-in-c/
*/
#ifndef DECODER_H_
#define DECODER_H_


typedef struct {
// WAVE file header format
  unsigned char riff[4];
  // RIFF string
  unsigned int overall_size;
  // overall size of file in bytes
  unsigned char wave[4];
  // WAVE string
  unsigned char fmt_chunk_marker[4];
  // fmt string with trailing null char
  unsigned int length_of_fmt;
  // length of the format data
  unsigned int format_type;
  // format type. 1-PCM, 3- IEEE float, 6 - 8bit A law, 7 - 8bit mu law
  unsigned int channels;
  // no.of channels
  unsigned int sample_rate;
  // sampling rate (blocks per second)
  unsigned int byterate;
  // SampleRate * NumChannels * BitsPerSample/8
  unsigned int block_align;
  // NumChannels * BitsPerSample/8
  unsigned int bits_per_sample;
  // bits per sample, 8- 8bits, 16- 16 bits etc
  unsigned char data_chunk_header[4];
  // DATA string or FLLR string
  unsigned int data_size;
  // NumSamples * NumChannels * BitsPerSample/8
  // - size of the next chunk that will be read
}wav_header;

typedef struct {
  wav_header header;  // wav metadata
  float* samples;  // pcm samples, -1 < x < 1
  unsigned int num_samples;  // number of samples
}wave;

wave* decode(char* filename);  // reads wav pcm file into wave struct
/* 
DECODER USAGE
Returns NULL if malloc or file read error, returns header+null in samples if not pcm. PCM is transformed into float,

void main() {
  wave * bouree = decode("bouree.wav");
  if (bouree==NULL) {
    printf("File reading or malloc error");
    return;
  }
  if (bouree->samples==NULL) {
    printf("Not a pcm wav");
    return;
  }
  FILE* samples;
  samples = fopen("samples.txt", "w");
  printf("%d\n", bouree->num_samples);
  for(int i = 0; i < bouree->num_samples; i++) {
    fprintf(samples, "%50.50f\n", bouree->samples[i]);
  }
  fclose(samples);
  free(bouree->samples);
  free(bouree);
}

*/
#endif  // DECODER_H_
