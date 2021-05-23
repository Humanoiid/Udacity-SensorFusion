#include <iostream>
#include <numeric>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;

void gaussianSmoothing1()
{
    // load image from file
    cv::Mat img;
    img = cv::imread("../images/img1gray.png");

    // create filter kernel
    float gauss_data[25] = {1, 4, 7, 4, 1,
                            4, 16, 26, 16, 4,
                            7, 26, 41, 26, 7,
                            4, 16, 26, 16, 4,
                            1, 4, 7, 4, 1};
    cv::Mat kernel = cv::Mat(5, 5, CV_32F, gauss_data);

    int sum = 0;
    for(int i=0; i<sizeof(gauss_data)/sizeof(float); i++)
    {
        sum += gauss_data[i];
    }
    for(int j=0; j<sizeof(gauss_data)/sizeof(float); j++)
    {
        // gauss_data[j] = gauss_data[j]/sum;
        gauss_data[j] /= sum;
    }

    // apply filter
    cv::Mat result;
    // cv::filter2D(img, result, -1, kernel, cv::Point(-1, -1), 0, cv::BORDER_DEFAULT);
    cv::filter2D(img, result, -1, kernel, cv::Point(-1, -1), 0, cv::BORDER_DEFAULT);
    // ddepth=-1 : output will have same depth as the source.
    // + cv::Gaussian Blur working almost same way.


    // for the comparison
    string windowName_org = "orignal image";
    cv::namedWindow(windowName_org, 2);
    cv::imshow(windowName_org, img);

    // show result
    string windowName = "Gaussian Blurring";
    cv::namedWindow(windowName, 1); // create window
    cv::imshow(windowName, result);
    cv::waitKey(0); // wait for keyboard input before continuing
}

int main()
{
    gaussianSmoothing1();
}