
#include "asmdemo.h"


#include "asmmodel.h"
#include "modelfile.h"


#include <cstdio>
#include <string>
#include "opencv/highgui.h"


using std::string;
using cv::imshow;
using std::cerr;
using std::endl;

using namespace StatModel;

//! Build an ASM model.
void buildASMModel(ASMModel & asmModel,
                   string shapeDef, string ptsList, string modelPath)
{
    asmModel.buildModel(shapeDef, ptsList);
    asmModel.saveToFile(modelPath);
}

//! Load an ASM model.
void readASMModel(ASMModel & asmModel, string modelPath)
{
    asmModel.loadFromFile(modelPath);
}

//! Run OpenCV object detection and do ASM fitting on each detected object.
void searchAndFit(
        ASMModel & asmModel,
        cv::CascadeClassifier &objCascadeClassfifier,
        const string & picPath,
        int verboseL) {
    // Load image.
    Mat img = cv::imread(picPath);
    if (img.empty()) {
        cerr << "Load image '" << picPath << "' failed." << endl;
        exit(2);
    }

    // Face detection.
    vector< cv::Rect > faces;
    objCascadeClassfifier.detectMultiScale(
        img, faces,
        1.2, 2, CV_HAAR_SCALE_IMAGE, Size(60, 60) );

    // Fit to ASM!
    vector < ASMFitResult > fitResult = asmModel.fitAll(img, faces, verboseL);
    asmModel.showResult(img, fitResult);
    cvWaitKey();
}

//! ASM on webcam stream!
void asmOnWebCam(
        ASMModel & asmModel,
        cv::CascadeClassifier &objCascadeClassfifier,
        int verboseL) {
    Mat img, imgT;
    cv::VideoCapture capture;
    capture.open(0);
    if (!capture.isOpened()) {
        cerr << "Failed to open your webcam." << endl;
        exit(2);
    }
    while (cvWaitKey(5) == -1) {
        capture >> imgT;
        cv::flip(imgT, img, 1);

        vector< cv::Rect > faces;
        // Do face detection first.
        // Note: ONLY the largest face is processed.
        //       (otherwise face detection would be too slow)
        objCascadeClassfifier.detectMultiScale( img, faces,
            1.2, 2, CV_HAAR_FIND_BIGGEST_OBJECT
                        //|CV_HAAR_DO_ROUGH_SEARCH
                        |CV_HAAR_SCALE_IMAGE
            , Size(60, 60) );

        // Fit to ASM!
        vector < ASMFitResult > fitResult = asmModel.fitAll(img, faces, verboseL);
        asmModel.showResult(img, fitResult);
    }
}


bool test_asm(string picPath,string faceCascadePath,string asmModelPath,int verboseL)
{
    ASMModel asmModel;
    asmModel.loadFromFile(asmModelPath);
    cv::CascadeClassifier faceCascade;
    if (!faceCascade.load(faceCascadePath)) {
           cerr << "Now, a (face) detector is needed. "
                << "Please check the URL above."
                << endl;
                return false;
    }
    searchAndFit(asmModel, faceCascade, picPath, verboseL);
    return true;
}
