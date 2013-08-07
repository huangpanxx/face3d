#include "asm.h"
#include "asmmodel.h"
using namespace StatModel;
using namespace cv;

namespace StatModel {

FaceLocator::FaceLocator(const char* modelFile,const char* classifierFile) {
    this->model.loadFromFile(modelFile);
    this->classifier.load(classifierFile);
}

FaceLocator::FaceLocator(const StatModel::ASMModel& model,const cv::CascadeClassifier &classifier) {
    this->model = model;
    this->classifier = classifier;
}


std::vector<StatModel::ASMFitResult>
FaceLocator::fitAll(const cv::Mat &image,int verboseLevel) {
    std::vector< cv::Rect > faces;
    classifier.detectMultiScale(
        image, faces,
        1.2, 2, CV_HAAR_SCALE_IMAGE, Size(60, 60) );

    return this->model.fitAll(image, faces, verboseLevel);
}

FaceLocator::FaceLocator(const FaceLocator& locator) {
    this->model= locator.model;
    this->classifier = locator.classifier;
}

}
