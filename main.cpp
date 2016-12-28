#include <iostream>
#include "yaacrl.h"

int main() {
    Yaacrl y("root", "tassadar");

    //std::cout << "ADD BRAD: " << y.add_file("audio/brad.wav") << std::endl;
    std::cout << "ADD grob: " << y.add_file("audio/met.wav") << std::endl;
    //int res = y.recognize_wav("audio/brad_cut.wav");
    //std::cout << "RECOGN BRAD: " << y.get_song_by_id(res)<< std::endl;
    //y.clear_database();
    //y.fingerprints_to_file("audio/brad_cut.wav");
    //std::map<std::string, int> res = y.recognize_fingerprints("audio/brad.wav.fingerprints");
    std::map<std::string, int> res = y.recognize_wav("audio/met_105.wav");
    std::cout << "RECOGN 105: \n";
    std::cout << "\n\tID: " << res["id"];
    std::cout << "\n\tOffset" << res["offset"];
    std::cout << "\n\tMatch %: " << res["percentage"] << std::endl;

    res = y.recognize_wav("audio/met_120.wav");
    std::cout << "RECOGN 120: \n";
    std::cout << "\n\tID: " << res["id"];
    std::cout << "\n\tOffset" << res["offset"];
    std::cout << "\n\tMatch %: " << res["percentage"] << std::endl;

    res = y.recognize_wav("audio/snaar_cut.wav");
    std::cout << "RECOGN SNAAR CUT: \n";
    std::cout << "\n\tID: " << res["id"];
    std::cout << "\n\tOffset" << res["offset"];
    std::cout << "\n\tMatch %: " << res["percentage"] << std::endl;


    res = y.recognize_wav("audio/rick_75.wav");
    std::cout << "RECOGN rick 75: \n";
    std::cout << "\n\tID: " << res["id"];
    std::cout << "\n\tOffset" << res["offset"];
    std::cout << "\n\tMatch %: " << res["percentage"] << std::endl;


    //y.recognize_fingerprints("audio/brad.wav.fingerprints");
    return 0;
}