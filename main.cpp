#include <iostream>
#include "yaacrl.h"

int main() {
    Yaacrl y;
    //y.add_file("audio/snaar.wav");
    //y.add_file("audio/brad.wav");


    std::cout << "RECOGN BRAD: " << y.recognize_file("audio/brad_cut.wav") << std::endl;
    //std::cout << "RECOGN SNAAR: " << y.recognize_file("audio/snaar_cut.wav") << std::endl;
    return 0;
}