#pragma once

#include "ofMain.h"
//#include "ofxAsio.h"
//#include <ofxRealSense2.h>
#include <librealsense2/rs.hpp>
#include "ofxNDI.h"

class ofApp : public ofBaseApp{
	public:
		void setup();

        void setupRSCamera();
        void setupNDIStream();

        void makeFunnyPictures();

		void update();
		void draw();

        void acquirePointCloud();

    private:
        rs2::pipeline pipe;
        rs2::device device;

        rs2::points points;
        rs2::pointcloud pc;

        bool acquiring;

        ofVboMesh mesh;
        ofEasyCam cam;

//        std::shared_ptr<ofxAsio::UdpSender> mSender;
        ofxNDIsender ndiSender;    // NDI sender
        string senderName;      // Sender name
        unsigned int senderWidth;  // Width of the sender output
        unsigned int senderHeight; // Height of the sender output
        ofFbo ndiFbo;              // Fbo used for graphics and sending
        ofImage textureImage;      // Texture image for the 3D cube graphics
        float rotX, rotY;          // Cube rotation increment
        ofImage ndiImage;          // Test image for sending
};
