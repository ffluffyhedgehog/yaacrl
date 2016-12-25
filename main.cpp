#include <iostream>
#include "yaacrl.h"

int main() {
    Yaacrl y;
    y.fingerprint_file("audio/snaar.wav");
    y.fingerprint_file("audio/brad.wav");
    y.recognize_file("audio/brad_cut.wav");
    y.recognize_file("audio/snaar_cut.wav");
    return 0;
}