/**
* This file is part of ORB-SLAM2.
*
* Copyright (C) 2014-2016 Raúl Mur-Artal <raulmur at unizar dot es> (University of Zaragoza)
* For more information see <https://github.com/raulmur/ORB_SLAM2>
*
* ORB-SLAM2 is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* ORB-SLAM2 is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with ORB-SLAM2. If not, see <http://www.gnu.org/licenses/>.
*/


#include<iostream>
#include<algorithm>
#include<fstream>
#include<chrono>

#include<opencv2/core/core.hpp>
#include<unistd.h>
#include<System.h>

#include<stdio.h>
#include<string.h>

#include "Josh.h"

using namespace std;

void LoadImages(const string &strFile, vector<string> &vstrImageFilenames,
                vector<double> &vTimestamps);
void LoadImagesORB(const string &strAssociationFilename, vector<string> &vstrImageFilenamesRGB,
                vector<string> &vstrOrbFilenames, vector<double> &vTimestamps);
vector<ORB_line> ReadORBFile(const string &file);

int main(int argc, char **argv)
{

    if(argc != 4)
    {
        cerr << endl << "Usage: ./mono_tum path_to_vocabulary path_to_settings path_to_sequence" << endl;
        return 1;
    }

// Retrieve paths to images
    vector<string> vstrImageFilenames;
    vector<double> vTimestamps;

#if READ_ORB == 1
    string strFile = string(argv[3])+"/rgb_orb.txt";
    vector<string> vstrOrbFilenames;
    LoadImagesORB(strFile, vstrImageFilenames, vstrOrbFilenames, vTimestamps);
#else    
    string strFile = string(argv[3])+"/rgb.txt";
    LoadImages(strFile, vstrImageFilenames, vTimestamps);
#endif

    int nImages = vstrImageFilenames.size();

    // Create SLAM system. It initializes all system threads and gets ready to process frames.
    ORB_SLAM2::System SLAM(argv[1],argv[2],ORB_SLAM2::System::MONOCULAR,true);

    // Vector for tracking time statistics
    vector<float> vTimesTrack;
    vTimesTrack.resize(nImages);

    cout << endl << "-------" << endl;
    cout << "Start processing sequence ..." << endl;
    cout << "Images in the sequence: " << nImages << endl << endl;

    // Main loop
    cv::Mat im;
    for(int ni=0; ni<nImages; ni++)
    {
        // Read image from file
        im = cv::imread(string(argv[3])+"/"+vstrImageFilenames[ni],CV_LOAD_IMAGE_UNCHANGED);
        double tframe = vTimestamps[ni];
#if PRINT_ORB ==1	
	cout << "FILE: " << vstrImageFilenames[ni] << endl;
#endif
        if(im.empty())
        {
            cerr << endl << "Failed to load image at: "
                 << string(argv[3]) << "/" << vstrImageFilenames[ni] << endl;
            return 1;
        }

#ifdef COMPILEDWITHC11
        std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
#else
        std::chrono::monotonic_clock::time_point t1 = std::chrono::monotonic_clock::now();
#endif
#if READ_ORB == 1
    	vector<ORB_line> data = ReadORBFile(string(argv[3])+"/"+vstrOrbFilenames[ni]);
	SLAM.TrackMonocular(im,tframe,data);  
#else
	// Pass the image to the SLAM system
        SLAM.TrackMonocular(im,tframe);
#endif

#ifdef COMPILEDWITHC11
        std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
#else
        std::chrono::monotonic_clock::time_point t2 = std::chrono::monotonic_clock::now();
#endif

        double ttrack= std::chrono::duration_cast<std::chrono::duration<double> >(t2 - t1).count();

        vTimesTrack[ni]=ttrack;

        // Wait to load the next frame
        double T=0;
        if(ni<nImages-1)
            T = vTimestamps[ni+1]-tframe;
        else if(ni>0)
            T = tframe-vTimestamps[ni-1];

        if(ttrack<T)
            usleep((T-ttrack)*1e6);
    }

    // Stop all threads
    SLAM.Shutdown();

    // Tracking time statistics
    sort(vTimesTrack.begin(),vTimesTrack.end());
    float totaltime = 0;
    for(int ni=0; ni<nImages; ni++)
    {
        totaltime+=vTimesTrack[ni];
    }
    cout << "-------" << endl << endl;
    cout << "median tracking time: " << vTimesTrack[nImages/2] << endl;
    cout << "mean tracking time: " << totaltime/nImages << endl;

    // Save camera trajectory
    SLAM.SaveKeyFrameTrajectoryTUM("KeyFrameTrajectory.txt");

    return 0;
}

vector<ORB_line> ReadORBFile(const string &file){
	ifstream f;
	f.open(file.c_str());
	vector<ORB_line> toReturn;
	string s0;
	getline(f,s0);
        getline(f,s0);
        getline(f,s0);
        while(!f.eof()){
		string line;
		getline(f,line);
		if (!line.empty()){
			ORB_line oneLine;
			stringstream ss;
			ss << line;
			ss >> oneLine.level;
			ss >> oneLine.x;
			ss >> oneLine.y;
			for (int i = 0; i < 32; i++){
				int temp;
				ss >> hex >> temp;
				oneLine.descriptors[i] = (unsigned char)temp;
			}
			toReturn.push_back(oneLine);
		}
		
	}
	return toReturn;
}	
		
void LoadImages(const string &strFile, vector<string> &vstrImageFilenames, vector<double> &vTimestamps)
{
    ifstream f;
    f.open(strFile.c_str());

    // skip first three lines
    string s0;
    getline(f,s0);
    getline(f,s0);
    getline(f,s0);

    while(!f.eof())
    {
        string s;
        getline(f,s);
        if(!s.empty())
        {
            stringstream ss;
            ss << s;
            double t;
            string sRGB;
            ss >> t;
            vTimestamps.push_back(t);
            ss >> sRGB;
            vstrImageFilenames.push_back(sRGB);
        }
    }
}

void LoadImagesORB(const string &strAssociationFilename, vector<string> &vstrImageFilenamesRGB,
                vector<string> &vstrOrbFilenames, vector<double> &vTimestamps)
{
    ifstream fAssociation;
    fAssociation.open(strAssociationFilename.c_str());

    string s;

    getline(fAssociation,s);
    getline(fAssociation,s);
    getline(fAssociation,s);
    
    while(!fAssociation.eof())
    {
    	getline(fAssociation,s);
    	if(!s.empty())
        {
            stringstream ss;
            ss << s;
            double t;
            string sRGB, sD;
            ss >> t;
            vTimestamps.push_back(t);
            ss >> sRGB;
            vstrImageFilenamesRGB.push_back(sRGB);
            ss >> t;
            ss >> sD;
            vstrOrbFilenames.push_back(sD);

        }
    }
}
