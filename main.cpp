#include <iostream>
#include "yaacrl.h"

int main() {
    Yaacrl y;

    //std::cout << "ADD BRAD: " << y.add_file("audio/brad.wav") << std::endl;
    //std::cout << "ADD SNAAR: " << y.add_file("audio/snaar.wav") << std::endl;
    //int res = y.recognize_file("audio/brad_cut.wav");
    //std::cout << "RECOGN BRAD: " << y.get_song_by_id(res)<< std::endl;
    y.clear_database();
    return 0;
}