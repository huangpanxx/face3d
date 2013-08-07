#include "mathead.h"


#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <fstream>
#include "common.h"

using namespace std;



MatHead::MatHead(const char *shapeFileName, const char*colorFileName)
{
    cv::Mat shape = read_mat(shapeFileName);
    cv::Mat color = read_mat(colorFileName);

    this->m_render.m_vexs.reserve(shape.rows);
    this->m_render.m_colors.reserve(shape.rows);

    for(int i=0;i<shape.rows;++i) {
        float x = -shape.at<float>(i,0);
        float y = -shape.at<float>(i,1);
        float z = shape.at<float>(i,2);
        GLvex3f vex(x,y,z);
        this->m_render.m_vexs.push_back(vex);
        float r = color.at<float>(i,0);
        float g = color.at<float>(i,1);
        float b = color.at<float>(i,2);
        GLvex3f rgb(r/255,g/255,b/255);
        this->m_render.m_colors.push_back(rgb);
    }


    this->position.z = -600;
    this->position.x = -150;
}

