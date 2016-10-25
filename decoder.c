/*
 * Copyright 2016 yaacrl contributors, true logic
*/
/*
 * This file is a modified version of 
 * http://truelogic.org/wordpress/2015/09/04/parsing-a-wav-file-in-c/
*/
/**
 * Read and parse a wave file
 *
 **/
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "./decoder.h"
#define TRUE 1
#define FALSE 0

unsigned char buffer4[4];
unsigned char buffer2[2];

wave* decode(char* filename) {
  FILE *ptr;
  wave* audio = (wave*) malloc(sizeof(wave));

  if (filename == NULL) {
    printf("Error in malloc\n");
    free(audio);
    return NULL;
  }
  // open file
  printf("Opening  file..\n");
  ptr = fopen(filename, "rb");
  if (ptr == NULL) {
    printf("Error opening file\n");
    free(audio);
    return NULL;
  }
  int read = 0;

  // read header parts

  read = fread(audio->header.riff, sizeof(audio->header.riff), 1, ptr);
  printf("(1-4): %s \n", audio->header.riff);
  read = fread(buffer4, sizeof(buffer4), 1, ptr);
  printf("%u %u %u %u\n", buffer4[0], buffer4[1], buffer4[2], buffer4[3]);
  // convert little endian to big endian 4 byte int
  audio->header.overall_size  = buffer4[0] |
            (buffer4[1] << 8) |
            (buffer4[2] << 16) |
            (buffer4[3] << 24);
  printf("(5-8) Overall size: bytes:%u, Kb:%u \n",
    audio->header.overall_size,
    audio->header.overall_size/1024);
  read = fread(audio->header.wave,
    sizeof(audio->header.wave),
    1,
    ptr);
  printf("(9-12) Wave marker: %s\n", audio->header.wave);
  read = fread(audio->header.fmt_chunk_marker,
    sizeof(audio->header.fmt_chunk_marker),
    1,
    ptr);
  printf("(13-16) Fmt marker: %s\n", audio->header.fmt_chunk_marker);
  read = fread(buffer4, sizeof(buffer4), 1, ptr);
  printf("%u %u %u %u\n", buffer4[0], buffer4[1], buffer4[2], buffer4[3]);
  // convert little endian to big endian 4 byte integer
  audio->header.length_of_fmt = buffer4[0] |
              (buffer4[1] << 8) |
              (buffer4[2] << 16) |
              (buffer4[3] << 24);
  printf("(17-20) Length of Fmt header: %u \n", audio->header.length_of_fmt);

  read = fread(buffer2, sizeof(buffer2), 1, ptr);
  printf("%u %u \n", buffer2[0], buffer2[1]);

  audio->header.format_type = buffer2[0] | (buffer2[1] << 8);
  char format_name[10] = "";
  if (audio->header.format_type == 1)
    strcpy(format_name, "PCM");
  else if (audio->header.format_type == 6)
    strcpy(format_name, "A-law");
  else if (audio->header.format_type == 7)
    strcpy(format_name, "Mu-law");

  printf("(21-22) Format type: %u %s \n",
    audio->header.format_type,
    format_name);

  read = fread(buffer2, sizeof(buffer2), 1, ptr);
  printf("%u %u \n", buffer2[0], buffer2[1]);

  audio->header.channels = buffer2[0] | (buffer2[1] << 8);
  printf("(23-24) Channels: %u \n", audio->header.channels);

  read = fread(buffer4, sizeof(buffer4), 1, ptr);
  printf("%u %u %u %u\n", buffer4[0], buffer4[1], buffer4[2], buffer4[3]);

  audio->header.sample_rate = buffer4[0] |
            (buffer4[1] << 8) |
            (buffer4[2] << 16) |
            (buffer4[3] << 24);

  printf("(25-28) Sample rate: %u\n", audio->header.sample_rate);

  read = fread(buffer4, sizeof(buffer4), 1, ptr);
  printf("%u %u %u %u\n", buffer4[0], buffer4[1], buffer4[2], buffer4[3]);

  audio->header.byterate  = buffer4[0] |
            (buffer4[1] << 8) |
            (buffer4[2] << 16) |
            (buffer4[3] << 24);
  printf("(29-32) Byte Rate: %u , Bit Rate:%u\n",
    audio->header.byterate,
    audio->header.byterate*8);

  read = fread(buffer2, sizeof(buffer2), 1, ptr);
  printf("%u %u \n", buffer2[0], buffer2[1]);

  audio->header.block_align = buffer2[0] | (buffer2[1] << 8);
  printf("(33-34) Block Alignment: %u \n", audio->header.block_align);

  read = fread(buffer2, sizeof(buffer2), 1, ptr);
  printf("%u %u \n", buffer2[0], buffer2[1]);

  audio->header.bits_per_sample = buffer2[0] | (buffer2[1] << 8);
  printf("(35-36) Bits per sample: %u \n", audio->header.bits_per_sample);

  read = fread(audio->header.data_chunk_header,
    sizeof(audio->header.data_chunk_header),
    1,
    ptr);
  printf("(37-40) Data Marker: %s \n", audio->header.data_chunk_header);

  read = fread(buffer4, sizeof(buffer4), 1, ptr);
  printf("%u %u %u %u\n", buffer4[0], buffer4[1], buffer4[2], buffer4[3]);

  audio->header.data_size = buffer4[0] |
        (buffer4[1] << 8) |
        (buffer4[2] << 16) |
        (buffer4[3] << 24);
  printf("(41-44) Size of data chunk: %u \n", audio->header.data_size);


  // calculate no.of samples
  long num_samples = (8 * audio->header.data_size) /
    (audio->header.channels * audio->header.bits_per_sample);
  printf("Number of samples:%lu \n", num_samples);

  long size_of_each_sample = (audio->header.channels *
    audio->header.bits_per_sample) / 8;
  printf("Size of each sample:%ld bytes\n", size_of_each_sample);

  // calculate duration of file
  float duration_in_seconds = (float) audio->header.overall_size /
    audio->header.byterate;
  printf("Approx.Duration in seconds=%f\n", duration_in_seconds);



  // read each sample from data chunk if PCM
  if (audio->header.format_type == 1) {  // PCM
    long i = 0;
    char data_buffer[size_of_each_sample];
    int  size_is_correct = TRUE;
    audio->num_samples = num_samples;
    audio->samples = (float*) malloc(sizeof(float)*num_samples);
    // make sure that the bytes-per-sample
    // is completely divisible by num.of channels
    long bytes_in_each_channel = (size_of_each_sample / audio->header.channels);
    if ((bytes_in_each_channel * audio->header.channels) !=
                                    size_of_each_sample) {
      printf("Error: %ld x %ud <> %ld\n",
        bytes_in_each_channel,
        audio->header.channels,
        size_of_each_sample);
      size_is_correct = FALSE;
    }

    if (size_is_correct) {
          // the valid amplitude range for values based on the bits per sample
      long low_limit = 0l;
      long high_limit = 0l;
      switch (audio->header.bits_per_sample) {
        case 8:
          low_limit = -128;
          high_limit = 127;
          break;
        case 16:
          low_limit = -32768;
          high_limit = 32767;
          break;
        case 32:
          low_limit = -2147483648;
          high_limit = 2147483647;
          break;
      }
      for (i =1; i <= num_samples; i++) {
        read = fread(data_buffer, sizeof(data_buffer), 1, ptr);
        if (read == 1) {
          // dump the data read
          unsigned int  xchannels = 0;
          int data_in_channel = 0;
          for (xchannels = 0; xchannels < audio->header.channels; ++xchannels) {
            // convert data from little endian to big
            // endian based on bytes in each channel sample
            if (bytes_in_each_channel == 4) {
              data_in_channel = data_buffer[0] |
                        (data_buffer[1] << 8) |
                        (data_buffer[2] << 16) |
                        (data_buffer[3] << 24);
            } else if (bytes_in_each_channel == 2) {
              data_in_channel = data_buffer[0] |
                      (data_buffer[1] << 8);
            } else if (bytes_in_each_channel == 1) {
              data_in_channel = data_buffer[0];
            }

            // printf("%d\n", data_in_channel);
            audio->samples[i] = data_in_channel / ((float)high_limit);
          }
        } else {
          printf("Error reading file. %d bytes\n", read);
          break;
        }
      }  //  for (i =1; i <= num_samples; i++) {
    }  //  if (size_is_correct) {

  } else {  // if (audio->header.format_type == 1) {
    audio->samples = NULL;
    audio->num_samples = 0;
  }
  printf("Closing file..\n");
  fclose(ptr);
  return audio;
}
