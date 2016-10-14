/*
 * Copyright 2016 yaacrl contributors
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "sha1/sha1.h"
#include "./fingerprint.h"
#include "./specgram.h"

#define ARRAY_INIT_SIZE 10
#define SIZE_FREQ 400
#define SIZE_TIME 200

// Function for loading array from "test_data/array.txt"
void load_test_data(Specgram * spec) {
  FILE * file = fopen("test_data/array.txt", "r");
  (*spec).sg = malloc((*spec).windows * sizeof(int*));
  for (int i = 0; i < (*spec).windows; i++)
    (*spec).sg[i] = malloc((*spec).freq * sizeof(int));
  for (int i = 0; i < (*spec).freq; i++) {
    for (int j = 0; j < (*spec).windows; j++) {
          fscanf(file, "%f ", &(*spec).sg[j][i]);
    }
    fscanf(file, "\n");
  }
  fclose(file);
}

// Sorting functions for qsort
int sort_by_freq(const void * p1, const void * p2) {
  return (*(Peak*)p1).freq - (*(Peak*)p2).freq;
}

int sort_by_time(const void * p1, const void * p2) {
  return (*(Peak*)p1).time - (*(Peak*)p2).time;
}


PeakHash * fingerprint(float * samples) {
  /*
  TODO: support for options (ie NBRHD_SIZE)

  Will be included in production and after decoder is ready:
  Specgram spec; = gen_specgram(channel_samples,
                                channel_length,
                                window_size,
                                window_ratio);
  For now - working with generated with python script 2-dim array
  */
  Specgram spec;
  spec.freq = SIZE_FREQ;
  spec.windows = SIZE_TIME;
  load_test_data(&spec);
  Peak * peaks = NULL;
  int peaks_count = detect_peaks(&peaks, spec);
  PeakHash * hashes = NULL;
  int hashes_count = generate_hashes(&hashes, peaks, peaks_count);
  free(peaks);
  for (int i = 0; i < spec.windows; i++)
    free(spec.sg[i]);
  free(spec.sg);
  return hashes;
}



int detect_peaks(Peak ** peaks, Specgram spec) {
  int current_size = ARRAY_INIT_SIZE;
  int peaks_found = 0;
  (*peaks) = (Peak *) realloc((*peaks), current_size * sizeof(Peak));
  for (int i = 0; i < spec.windows; i++) {
    for (int j = 0; j < spec.freq; j++) {
      // Checking if cell is local maximum
      float max = 0;
      // Walking through footprint
      for (int k = -NBRHD_SIZE; k <= NBRHD_SIZE; k++) {
        if (i+k < 0 || i+k >= spec.windows)
          continue;
        for (int h = abs(k) - NBRHD_SIZE; h <= -abs(k) + NBRHD_SIZE; h++) {
          if (j+h < 0 || j+h >= spec.freq)
            continue;
          if (spec.sg[i+k][j+h] > max)
            max = spec.sg[i+k][j+h];
        }
      }
      // Checking: if cell is local maximum
      //           if cell is part of background
      //           if cell`s value is lower than minimal bound
      if (spec.sg[i][j] == max && max && max > DEFAULT_AMP_MIN) {
          (*peaks)[peaks_found].freq = j;
          (*peaks)[peaks_found].time = i;
          peaks_found++;
          // Reallocing if needed
          if (peaks_found == current_size) {
            current_size *= 2;
            (*peaks) = (Peak *) realloc((*peaks), current_size * sizeof(Peak));
          }
        }
    }
  }
  (*peaks) = (Peak *) realloc((*peaks), peaks_found * sizeof(Peak));
  return peaks_found;
}

int generate_hashes(PeakHash ** peak_hashes, Peak * peaks, int count) {
  if (PEAK_SORT)
    qsort(peaks, count, sizeof(Peak), sort_by_time);
  int hashes_added = 0;
  int current_size = ARRAY_INIT_SIZE;
  (*peak_hashes) = (PeakHash *) realloc((*peak_hashes), current_size * sizeof(PeakHash));
  SHA1Context sha;
  int k, t1, t2, freq1, freq2, t_delta;
  char * sha1_input;
  for (int i = 0; i < count; i++) {
    for (int j = 1; j < DEFAULT_FAN_VALUE; j++) {
      if (i + j < count) {
        freq1 = peaks[i].freq;
        freq2 = peaks[i+j].freq;
        t1 = peaks[i].time;
        t2 = peaks[i+j].time;
        t_delta = t2 - t1;
        if (t_delta >= MIN_HASH_TIME_DELTA && t_delta <= MAX_HASH_TIME_DELTA) {
          // Getting length of upcoming string
          k = snprintf(NULL, 0, "%d|%d|%d", freq1, freq2, t2-t1);
          sha1_input = (char *) realloc(sha1_input, (k+1) * sizeof(char));
          snprintf(sha1_input, k+1, "%d|%d|%d", freq1, freq2, t2-t1);
          SHA1Reset(&sha);
          SHA1Input(&sha, sha1_input, k);
          if (SHA1Result(&sha)) {
            // Length of sha1 hash in hex is 40
            char hash[41];
            SHA1Hex(&sha, hash);
            strncpy((*peak_hashes)[hashes_added].hash, hash, FINGER_REDUCTION);
            (*peak_hashes)[hashes_added].hash[FINGER_REDUCTION] = '\0';
            (*peak_hashes)[hashes_added].time = t1;
            hashes_added++;
            // Reallocing if needed
            if (hashes_added == current_size) {
              current_size *= 2;
              (*peak_hashes) = (PeakHash *)
                               realloc((*peak_hashes),
                                       current_size * sizeof(PeakHash));
            }
          }
        }
      }
    }
  }
  (*peak_hashes) = (PeakHash *) realloc((*peak_hashes),
                                        hashes_added * sizeof(PeakHash));
  free(sha1_input);
  return hashes_added;
}


void main() {
  /*
  reworked fingerprint function for builing and testing
  TODO: support for not default oprions (ie NBRHD_SIZE)


  Will be included in production and after decoder is ready:
  Specgram spec; = gen_specgram(channel_samples,
                                channel_length,
                                window_size,
                                window_ratio);
  For now - working with generated with python script 2-dim array
  */
  Specgram spec;
  spec.freq = SIZE_FREQ;
  spec.windows = SIZE_TIME;
  load_test_data(&spec);
  Peak * peaks = NULL;
  int peaks_count = detect_peaks(&peaks, spec);
  // TEST 1 START
  qsort(peaks, peaks_count, sizeof(Peak), sort_by_freq);
  // Sorting peaks by freq, because in dejavu indices are swapped
  FILE * f = fopen("test_data/c_test1.txt", "w");
  for (int i = 0; i < peaks_count; i++)
    fprintf(f, "%d %d\n", peaks[i].freq, peaks[i].time);
  fclose(f);
  // TEST 1 END
  PeakHash * hashes = NULL;
  int hashes_count = generate_hashes(&hashes, peaks, peaks_count);
  // TEST 2 START
  f = fopen("test_data/c_test2.txt", "w");
  for (int i = 0; i < hashes_count; i++)
    fprintf(f, "%s %d\n", hashes[i].hash, hashes[i].time);
  fclose(f);
  // TEST 2 END
  free(peaks);
  for (int i = 0; i < spec.windows; i++)
    free(spec.sg[i]);
  free(spec.sg);
}
