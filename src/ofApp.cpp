#include "ofApp.h"
#include "alvinutil.h"
#include <string.h>

void ofApp::setupNDIStream() {
    senderName = "ofndisender";
    //strcpy_s(senderName, 256, "Openframeworks NDI Sender"); // Set the sender name
    ofSetWindowTitle(senderName); // show it on the title bar

    string ver = ndiSender.GetNDIversion();

    // setup NDI stuff
    ofLogNotice() << ver << " (http://ndi.tv/)" << endl;
    // Set the dimensions of the sender output here
    // This is independent of the size of the display window
    senderWidth  = 1920;
    senderHeight = 1080;

    // optionally set fbo readback using OpenGL pixel buffers
    ndiSender.SetReadback(); // Change to false to compare

    // optionally set NDI asynchronous sending
    // instead of clocked at the specified frame rate (60fps default)
    ndiSender.SetAsync();

    // Create a sender with default RGBA output format
    ndiSender.CreateSender(senderName.c_str(), senderWidth, senderHeight);
    ofLogNotice() << "Created NDI sender [" << senderName << "] (" << senderWidth << "x" << senderHeight << ")" << endl;

    // Image for pixel sending examples
    ndiImage.load("Test_Pattern.jpg");
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
    ofSetFrameRate(30);
    ofSetVerticalSync(true);
    ofSetLogLevel(OF_LOG_NOTICE);

    setupNDIStream();
    setupRSCamera();
}

void ofApp::makeFunnyPictures() {

    ofDisableAlphaBlending(); // Or we can get trails with the rotating cube

    // Draw graphics into an fbo for the example
    ndiFbo.begin();

    // ======== your application draw goes here ========
    ofEnableDepthTest();
    ofClear(13, 25, 76, 255);

    ofPushMatrix();
    ofTranslate((float)senderWidth / 2.0, (float)senderHeight / 2.0, 0);
    ofRotateYDeg(rotX);
    ofRotateXDeg(rotY);
    textureImage.getTexture().bind();
    ofDrawBox(0.4*(float)senderHeight);
    ofPopMatrix();

    ofDisableDepthTest();
    rotX += 0.6;
    rotY += 0.6;
    // ============ end application draw ==============

    // End the fbo
    ndiFbo.end();

    // Draw the fbo result fitted to the display window
    ndiFbo.draw(0, 0, ofGetWidth(), ofGetHeight());
}

void ofApp::acquirePointCloud() {
    // Get depth data from camera
    auto frames = pipe.wait_for_frames();
    auto color = frames.get_color_frame();
    auto depth = frames.get_depth_frame();

    // tell pointcloud object to map colors from video frame
    pc.map_to(color);
    points = pc.calculate(depth);


    // Option 1 : Send ofFbo
    // ofFbo and ofTexture must be RGBA
    // Pixel data extraction from fbo or texture
    // is optimised using ndiSender.SetReadback();
    makeFunnyPictures();
    ndiSender.SendImage(ndiFbo);

    // Option 2 : Send ofTexture
    // DrawGraphics();
    // ndiSender.SendImage(ndiFbo.getTexture());

    // Option 3 Send ofImage
    // ofImage or ofPixels are converted to RGBA if not already
    // ndiImage.draw(0, 0, ofGetWidth(), ofGetHeight());
    // ndiSender.SendImage(ndiImage);

    // Option 4 Send ofPixels
    // ndiImage.draw(0, 0, ofGetWidth(), ofGetHeight());
    // ndiSender.SendImage(ndiImage.getPixels());

    // Option 5 Send char buffer
    // The pixels must be rgba
    // if (ndiImage.getImageType() != OF_IMAGE_COLOR_ALPHA)
       // ndiImage.setImageType(OF_IMAGE_COLOR_ALPHA);
    // ndiImage.draw(0, 0, ofGetWidth(), ofGetHeight());
    // ndiSender.SendImage(ndiImage.getPixels().getData(), senderWidth, senderHeight);

    // Show what it is sending
    if (ndiSender.SenderCreated()) {
        std::stringstream ss;

        ss << "Sending as : [" << senderName << "] (" << senderWidth << "x" << senderHeight << ")";
        ofDrawBitmapString(ss.str(), 20, 30);

        ss << "fps: " << (int)ofGetFrameRate() << " (" << ndiSender.GetFps() <<")";
        // Show fps
        ofDrawBitmapString(ss.str(), ofGetWidth() - 140, 30);
    }

    /*
    auto textureCoordinates = points.get_texture_coordinates();
    const unsigned char* colorData = static_cast<const unsigned char*>(color.get_data());

    for (int i = 0; i < rs_points.size(); i++) {
        int x = static_cast<int>(textureCoordinates[i].u * width);
        int y = static_cast<int>(textureCoordinates[i].v * height);
        int colorLocation = y * color_intrinsic.width + x;

        float px = rs_vertices[i].x;
        float py = rs_vertices[i].y;
        float pz = rs_vertices[i].z;
        unsigned char r = colorData[colorLoation];
        unsigned char g = colorData[colorLocation + 1];
        unsigned char b = colorData[colorLocation + 2];
        save(px, py, pz, r, g, b);
    }
    */
}

//--------------------------------------------------------------
void ofApp::update(){
/*
    std::shared_ptr<Message> datagram = std::make_shared<Message>("Hello world", "192.168.0.115", 8080);
    mSender->send(datagram);
*/

//    if(!acquiring) {
//        // do nothing
//        return;
//    } else {
//        acquirePointCloud();
//    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    acquirePointCloud();
}
