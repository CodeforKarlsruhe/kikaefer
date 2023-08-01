#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    std::cout << "Start image conversion." << std::endl;

    // Read the input image
    cv::Mat image = cv::imread("data/dog.jpg");

    // Check if the image was successfully loaded
    if (image.empty()) {
        std::cout << "Failed to read the image." << std::endl;
        return -1;
    }

    // Scale the image to double size
    cv::Mat scaledImage;
    cv::resize(image, scaledImage, cv::Size(), 2.0, 2.0);

    // Convert the image to grayscale
    cv::Mat grayImage;
    cv::cvtColor(scaledImage, grayImage, cv::COLOR_BGR2GRAY);

    // Write the processed images to files
    cv::imwrite("scaled_dog.jpg", scaledImage);
    cv::imwrite("gray_dog.jpg", grayImage);

    return 0;
}

