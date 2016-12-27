#include <iostream>
#include "yaacrl.h"

int main() {
    Yaacrl y;

    //std::cout << "ADD BRAD: " << y.add_file("audio/brad.wav") << std::endl;
    //std::cout << "ADD SNAAR: " << y.add_file("audio/snaar.wav") << std::endl;
    //int res = y.recognize_wav("audio/brad_cut.wav");
    //std::cout << "RECOGN BRAD: " << y.get_song_by_id(res)<< std::endl;
    //y.clear_database();
    //y.fingerprints_to_file("audio/brad_cut.wav");
    //std::map<std::string, int> res = y.recognize_fingerprints("audio/brad.wav.fingerprints");
    std::map<std::string, int> res = y.recognize_wav("audio/brad_cut.wav");
    std::cout << "RECOGN BRAD: \n";
    std::cout << "\n\tID: " << res["id"];
    std::cout << "\n\tOffset" << res["offset"];
    std::cout << "\n\tMatch %%: " << res["percentage"];

    //y.recognize_fingerprints("audio/brad.wav.fingerprints");
    return 0;
}