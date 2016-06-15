//
//  addMusic.cpp
//  SoundFilm
//
//  Created by Jiacheng Chen on 27/05/2016.
//  Copyright © 2016 a. All rights reserved.
//

#include "addMusic.hpp"
#include "sceneDetect.hpp"
#include <iostream>
#include <fstream>
#include <map>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

void addMusic(string fname, int totalClips) {
    map<int, int> song;
    map<int, double> time;
    system("rm -rf Resources/output/audio/*");
    ofstream outputFile("Resources/output/audio/music.txt");
    for (int i = 1; i <= totalClips; i++) {
        ostringstream vname;
        vname << "Resources/output/video/" << i <<".mp4";
        VideoCapture vid;
        vid.open(vname.str());
        if(!vid.isOpened()) {
            throw "Error when opening the file";
        }
        double fps = vid.get(CV_CAP_PROP_FPS);
        double frameCount = vid.get(CV_CAP_PROP_FRAME_COUNT);
        string duration = getTimecode(fps, frameCount);
        int max = 9;
        int min = 1;
        int ran = rand()%(max-min + 1) + min;
        //cout << duration << endl;
        
        outputFile << "file " << i << ".wav" << endl;
        /*
        bool found = false;
        for (int j = i; j > 1; j--) {
            if (song[j] != 0) {
                if (compareVid(i,j)) {
                    song[i] = song[j];
                    ran = song[j];
                    time[ran] += duration;
                    found = true;
                    break;
                } else {
                }
            }
        }
        
        if (!found) {
            if (time.count(ran) != 0) {
                do {
                    ran = rand()%(max-min + 1) + min;
                } while (time.count(ran) != 0);
            } else {
                time[ran] = 0;
            }
            song[i] = ran;
        }*/
        
        ostringstream cmd;
        cmd << "ffmpeg -ss 00:00:00.000 -i Resources/Audio/Slow/" << ran << ".wav" << " -t "
            << duration << " Resources/output/audio/" << i << ".wav" << endl;
        system(cmd.str().c_str());
        //cout << cmd.str() << endl;
        vid.release();
    }
    
    outputFile.close();
    
    ostringstream cmd;
    cmd << "ffmpeg -f concat -i Resources/output/audio/music.txt -c copy Resources/output.wav" << endl;
    system(cmd.str().c_str());
}