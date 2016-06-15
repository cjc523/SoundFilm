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
    } else if (argc == 4) {
        
        String fname = argv[1];
        int method = atoi(argv[2]);
        double threshold = atof(argv[3]);
        int index = detect(fname, method, threshold);
        //addMusic(fname, index);
        /*
        VideoCapture vid;
        vid.open(fname);
        if(!vid.isOpened()) {
            throw "Error when opening the file";
        }
        double fps = vid.get(CV_CAP_PROP_FPS);
        double frameCount = vid.get(CV_CAP_PROP_FRAME_COUNT);
        cout << fps  << " " << frameCount << " " << getTimecode(fps, frameCount) << endl;;
         */
    } else {
        string fname = argv[1];
        string fname2 = argv[2];
        comparePic(fname, fname2);
    }
    //system("rm -rf Resources/output/video/*");
    //system("rm -rf Resources/output/audio/*");
    cout << "finished processing" << endl;
}