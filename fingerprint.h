/*
 * Copyright 2016 yaacrl contributors
*/
#ifndef FINGERPRINT_H_
#define FINGERPRINT_H_

#include "./specgram.h"

/*
 * Constants for fingerprintinting 
 * Based on dejavu fingerprinting
 * https://github.com/worldveil/dejavu/blob/master/dejavu/fingerprint.py
*/

// Sampling rate, related to the Nyquist conditions, which affects
// the range frequencies we can detect.
#define DEFAULT_FS 44100

// Size of the FFT window, affects frequency granularity
#define DEFAULT_WINDOW_SIZE 4096

// Ratio by which each sequential window overlaps the last and the
// next window. Higher overlap will allow a higher granularity of offset
// matching, but potentially more fingerprints.
#define DEFAULT_OVERLAP_RATIO 0.5

// Degree to which a fingerprint can be paired with its neighbors --
// higher will cause more fingerprints, but potentially better accuracy.
#define DEFAULT_FAN_VALUE 15

// Minimum amplitude in spectrogram in order to be considered a peak.
// This can be raised to reduce number of fingerprints, but can negatively
// affect accuracy.
#define DEFAULT_AMP_MIN 10

// Number of cells around an amplitude peak in the spectrogram in order
// for Dejavu to consider it a spectral peak. Higher values mean less
// fingerprints and faster matching, but can potentially affect accuracy.
// In dejavu, called PEAK_NEIGHBORHOOD_SIZE
#define NBRHD_SIZE 20

// Thresholds on how close or far fingerprints can be in time in order
// to be paired as a fingerprint. If your max is too low, higher values of
// DEFAULT_FAN_VALUE may not perform as expected.
#define MIN_HASH_TIME_DELTA 0
#define MAX_HASH_TIME_DELTA 200

// If True, will sort peaks temporally for fingerprinting;
// not sorting will cut down number of fingerprints, but potentially
// affect performance.
#define PEAK_SORT true

// Number of bits to throw away from the front of the SHA1 hash in the
// fingerprint calculation. The more you throw away, the less storage, but
// potentially higher collisions and misclassifications when identifying songs.
// In dejavu, called FINGERPRINT_REDUCTION
#define FINGER_REDUCTION 20

typedef struct Peak {
  int freq;  // coordinate of frequency in specgram
  int time;  // coordinate of time in specgram
} Peak;

typedef struct PeakHash {
  char hash[FINGER_REDUCTION+1];
  int time;
} PeakHash;

typedef struct PeakHashCollection {
	PeakHash * peak_hashes;
	int count;
} PeakHashCollection;

/*
 * Fingerprinting function
 *  :params:
 *    samples - [-1:1] floats, representing samples of audio
 *  
 *  :returns:
 *    Pointer to PeakHash array, that has to be cleaned by free() after use,
 *    as it is dynamically created
*/
PeakHashCollection * fingerprint(float * samples, int samples_count);


/*
 * Function for detecting peaks in specgram
 * (detects peaks, filters bg-peaks, filters lower that AMP_MIN peaks)
 *  :params:
 *    peaks - pointer to array of Peaks to write peaks from specgram
 *            any information is overwritten
 *
 *    spec - specgram where peaks will be searched
 *  
 *  :returns:
 *    number of finded peaks (length of peaks array)
 *    
 *  :comments:
 *    As peaks array is dynamically created, it has to be cleaned
 *    by free() when does not needed anymore
*/
int detect_peaks(Peak ** peaks, Specgram spec);


/*
 * Function for generating hashes of suitable peaks
 * (sorts peaks if needed, finds suitable hashes)
 *  :params:
 *    hashes - pointer to array of PeakHashes to write hashes
 *             any information is overwritten
 *
 *    peaks - array of peaks to hash
 *
 *    peaks_count - length of peaks array
 *  
 *  :returns:
 *    number of generated hashes (length of hashes array)
 *
 *  :comments:
 *    As hashes array is dynamically created, it has to be cleaned
 *    by free() when does not needed anymore
*/
int generate_hashes(PeakHash ** hashes, Peak * peaks, int peaks_count);

#endif  // FINGERPRINT_H_
