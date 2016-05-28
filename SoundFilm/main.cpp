//
//  main.cpp
//  SoundFilm
//
//  Created by Jiacheng Chen on 15/05/2016.
//  Copyright © 2016 a. All rights reserved.
//

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include "sceneDetect.hpp"
#include "addMusic.hpp"
#include <iostream>

using namespace std;
using namespace cv;
int main( int argc, char** argv )
{
    if(argc <= 1) {
        throw "No input file.";
    } else if (argc == 2) {
        
         String fname = argv[1];
        int index = detect(fname);
        
        //addMusic(index);
    } else {
        String fname = argv[1];
        String fname2 = argv[2];
        comparePic(fname, fname2);
    }
    
    cout << "finished processing" << endl;
}