#include "ofApp.h"
#include "alvinutil.h"
#include <string.h>

void ofApp::setupNDIStreams() {
    streamwidth = 1920;
    streamheight= 1080;

    color.allocate(streamwidth, streamheight, GL_RGBA);
    depth.allocate(streamwidth, streamheight, GL_RGBA);

    std::string name = "depth";
    ndiDepth.SetReadback(); // Change to false to compare
    // optionally set NDI asynchronous sending
    // instead of clocked at the specified frame rate (60fps default)
    ndiDepth.SetAsync();
    // Create a sender with default RGBA output format
    ndiDepth.CreateSender(name.c_str(), streamwidth, streamheight);
    ofLogNotice() << "Created NDI sender [" << name << "] (" << streamwidth << "x" << streamheight << ")" << endl;

    name = "color";
    ndiColor.SetReadback(); // Change to false to compare
    // optionally set NDI asynchronous sending
    // instead of clocked at the specified frame rate (60fps default)
    ndiColor.SetAsync();
    // Create a sender with default RGBA output format
    ndiColor.CreateSender(name.c_str(), streamwidth, streamheight);
    ofLogNotice() << "Created NDI sender [" << name << "] (" << streamwidth << "x" << streamheight << ")" << endl;
}


void ofApp::setupRSCamera() {
    // set RealSense streaming stuff
    acquiring = false;

    rs2::context ctx;
    std::vector<rs2::pipeline>  pipelines;

    // enumerate available devices
    for(auto &&dev : ctx.query_devices()) {
        rs2::config cfg;
        ofLogNotice() << "RealSense device found with ID " << dev.get_info(RS2_CAMERA_INFO_SERIAL_NUMBER);
        cfg.enable_device( dev.get_info(RS2_CAMERA_INFO_SERIAL_NUMBER) );
        acquiring = true; // seems like we have a data source
        pipe.start( cfg ); // open it
        pipelines.push_back(pipe); // keep track of this pipeline
    }

    // show result of camera scan
    if(acquiring) {
        ofLogNotice() << "Number of camera pipelines " << pipelines.size();
    } else {
        ofLogWarning() << "No RealSense cameras were found! make sure you have one plugged in";
    }
}

void ofApp::setup() {
    ofSetWindowTitle("NDI stream preview window");
    ofSetFrameRate(30);
    ofSetVerticalSync(true);
    ofSetLogLevel(OF_LOG_NOTICE);

    string ver = ndiColor.GetNDIversion();
    ofLogNotice() << ver << " (http://ndi.tv/)" << endl;

    setupNDIStreams();

    // image for pixel sending examples
    unicorn.load("unicorn.jpeg");
    testpattern.load("testpattern.jpg");
}

void ofApp::drawTestPattern() {
    ofClear(0, 0, 0, 255);


    ofDisableAlphaBlending(); // Or we can get trails with the rotating cube

    color.begin();
        ofEnableDepthTest();
        ofClear(13, 25, 76, 255);

        ofPushMatrix();
        ofTranslate((float)streamwidth/ 2.0, (float)streamheight / 2.0, 0);
        ofRotateYDeg(rx);
        ofRotateXDeg(ry);
        unicorn.getTexture().bind();
        ofDrawBox( (streamheight/2)*0.5f );
        ofPopMatrix();

        ofDisableDepthTest();
    color.end();

    int ypos = (ofGetHeight()/2) - 300;
    color.draw(0, ypos, ofGetWidth()/2, ofGetHeight()/2);

    depth.begin();
        ofEnableDepthTest();
        ofClear(200, 180, 180, 255);

        ofPushMatrix();
        ofTranslate((float)streamwidth/ 2.0, (float)streamheight / 2.0, 0);
        ofRotateYDeg(rx);
        ofRotateXDeg(ry);
        unicorn.getTexture().bind();
        ofDrawBox( (streamheight/2)*0.75f );
        ofPopMatrix();

        ofDisableDepthTest();
    depth.end();

    depth.draw(ofGetWidth()/2, ypos, ofGetWidth()/2, ofGetHeight()/2);

    rx += 0.6f;
    ry += 0.6f;
}

void ofApp::update() {
}

void ofApp::draw(){
    drawTestPattern();

    ndiColor.SendImage( color );
    ndiDepth.SendImage( depth );

    ofPushStyle();
        ofSetColor(255, 255, 255);
        ofFill();

        if ( ndiColor.SenderCreated() ) {
            std::stringstream ss1;

            ss1 << "Sending as : " << ndiColor.GetSenderName() << "(" << ndiColor.GetWidth() << "x" << ndiColor.GetHeight() << ")";
            ofDrawBitmapString(ss1.str(), 20, 30);

            std::stringstream ss2;
            ss2 << "fps: " << ofGetFrameRate() << " (" << ndiColor.GetFps() << ")";
            ofDrawBitmapString(ss2.str(), ofGetWidth() - 340, 30);
        } else {
            ofLogNotice() << "sender not created";
        }
    ofPopStyle();
}
