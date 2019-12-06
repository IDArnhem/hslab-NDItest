#pragma once

#include "ofMain.h"
//#include <ofxRealSense2.h>
#include <librealsense2/rs.hpp>
#include "ofxNDI.h"

class ofApp : public ofBaseApp{
	public:
		void setup();

        void setupRSCamera();
        void setupNDIStreams();
        void drawTestPattern();

		void update();
		void draw();

    private:
        rs2::pipeline pipe;
        rs2::device device;

        bool acquiring;

        ofxNDIsender ndiColor;
        ofxNDIsender ndiDepth;

        ofFbo color;
        ofFbo depth;

        float rx, ry;
        uint32_t streamwidth, streamheight;

        ofImage testpattern;
        ofImage unicorn;
};
