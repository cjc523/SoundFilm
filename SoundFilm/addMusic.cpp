//
//  addMusic.cpp
//  SoundFilm
//
//  Created by Jiacheng Chen on 27/05/2016.
//  Copyright © 2016 a. All rights reserved.
//

#include "addMusic.hpp"
#include <iostream>
#include <fstream>
#include <map>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

void addMusic(int totalClips) {
    map<int, int> song;
    map<int, double> time;
    ofstream outputFile("music.txt");
    
    for (int i = 1; i <= totalClips; i++) {
        ostringstream fname;
        fname << i << ".mp4";
        
        VideoCapture vid;
        vid.open(fname.str());
        if(!vid.isOpened()) {
            throw "Error when opening the file";
        }
        int fps = round(vid.get(CV_CAP_PROP_FPS));
        int frameCount = vid.get(CV_CAP_PROP_FRAME_COUNT);
        double duration = frameCount / fps;
        int max = 8;
        int min = 1;
        int ran = rand()%(max-min + 1) + min;
        
        outputFile << "file '" << i << ".wav" << endl;
        song[i] = ran;
        ostringstream cmd;
        cmd << "ffmpeg -ss " << 0 << " -i Resources/Audio/Moderate/" << ran << ".wav" << " -t "
            << duration /*<< " /Resources/output/video/"*/ << " " << i << ".wav" << endl;
        system(cmd.str().c_str());
        
        vid.release();
    }
    
    outputFile.close();
    
    ostringstream cmd;
    cmd << "ffmpeg -f concat -i music.txt -c copy output.wav" << endl;
    system(cmd.str().c_str());
}