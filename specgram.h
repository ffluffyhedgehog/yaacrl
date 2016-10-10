/*
 * Calculates signal specgram using fft, and then applies log transform to it.
 * * * * * * * * * * * * * * * * * * * * *
 * Takes raw float data (-1 to 1)
 * * * * * * * * * * * * * * * * * * * * *
 * The place "to" is pointing will be cleared and re-initialized, so there is
 * no need for initializing it.
 * * * * * * * * * * * * * * * * * * * * *
 * Size of to is (signal_length/window_size + 1)x(window_size/2 + 1). The first is time, the second is frequency
 */
void specgram(float * from, float ** to, int signal_length, int window_size = 2048, float overlap = 0.5);

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
