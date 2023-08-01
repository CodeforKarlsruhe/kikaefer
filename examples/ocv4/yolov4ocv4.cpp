#include <opencv2/opencv.hpp>
#include <iostream>


// g++ -o yolov4 yolov4ocv4.cpp  -I /usr/include/opencv4/ -l opencv_core -l opencv_dnn  -l opencv_imgcodecs
// weights and docs: https://github.com/pjreddie/darknet/issues/2201


bool compareOutput(const std::tuple<float, cv::Rect, int>& a, const std::tuple<float, cv::Rect, int>& b)
{
    return std::get<0>(a) > std::get<0>(b);
}


int main()
{
    // Load the model and configuration
    std::cout << "Load weights." << std::endl;
    std::string model_weights = "yolov4-tiny.weights";
    std::cout << "Load config." << std::endl;
    std::string model_config = "cfg/yolov4-tiny.cfg";
    std::cout << "Create model." << std::endl;
    cv::dnn::Net net = cv::dnn::readNetFromDarknet(model_config, model_weights);


    std::cout << "Get layers." << std::endl;
    // Get the names of the layers in the network
    std::vector<cv::String> layerNames = net.getLayerNames();

    std::cout << "Print layers." << std::endl;
    // Print the parameters of each layer
    for (const auto& layerName : layerNames) {
        cv::Ptr<cv::dnn::Layer> layer = net.getLayer(layerName);
        std::cout << "Layer: " << layerName << std::endl;
        std::cout << "Type: " << layer->type << std::endl;
        std::cout << "----------------------" << std::endl;
    }


    // Get the indices of the unconnected output layers
    std::vector<int> unconnectedLayers = net.getUnconnectedOutLayers();

    // Print the indices of the unconnected output layers
    std::cout << "Unconnected Output Layers: ";
    for (const auto& layer : unconnectedLayers) {
        std::cout << layer << " ";
    }
    std::cout << std::endl;


    std::cout << "Set backend." << std::endl;
    // Set the backend and target for OpenCV DNN
    net.setPreferableBackend(cv::dnn::DNN_BACKEND_DEFAULT);
    net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);


    for (auto il = 0; il < 100; il++) {
       

        // Load the input image
        std::cout << "Load image." << std::endl;
        cv::Mat image = cv::imread("data/dog.jpg");

        std::cout << "Create blob." << std::endl;
        // Create a blob from the image and perform object detection
        cv::Mat blob = cv::dnn::blobFromImage(image, 1 / 255.0, cv::Size(416, 416), true, false);
        net.setInput(blob);
        std::vector<cv::Mat> outputs;

        std::cout << "Inference." << std::endl;
        net.forward(outputs, net.getUnconnectedOutLayersNames());
        //net.forward(outputs);
        // Forward pass to obtain output predictions
        //cv::Mat output = net.forward();

        std::cout << "Checking results." << std::endl;
        // Process the outputs for object detection results
        float confidence_threshold = 0.7; // was 0.5
        std::vector<cv::Rect> boxes;
        std::vector<float> confidences;
        std::vector<int> class_ids;

        std::cout << "Sort results." << std::endl;
        // Iterate over each detection output
        for (const auto& output : outputs)
        {
            for (int i = 0; i < output.rows; i++)
            {
                cv::Mat detection = output.row(i);
                cv::Mat scores = detection.colRange(5, output.cols);
                cv::Point class_id_point;
                double confidence;
                cv::minMaxLoc(scores, nullptr, &confidence, nullptr, &class_id_point);

                int class_id = class_id_point.x;
                confidence = scores.at<float>(class_id);

                // Filter out weak detections
                if (confidence > confidence_threshold)
                {
                    int center_x = static_cast<int>(detection.at<float>(0) * image.cols);
                    int center_y = static_cast<int>(detection.at<float>(1) * image.rows);
                    int width = static_cast<int>(detection.at<float>(2) * image.cols);
                    int height = static_cast<int>(detection.at<float>(3) * image.rows);

                    // Calculate the top-left coordinates of the bounding box
                    int x = center_x - width / 2;
                    int y = center_y - height / 2;

                    // Store the bounding box coordinates, confidence, and class ID
                    boxes.push_back(cv::Rect(x, y, width, height));
                    confidences.push_back(static_cast<float>(confidence));
                    class_ids.push_back(class_id);
                }
            }
        }

        // Apply non-maximum suppression to remove overlapping bounding boxes
        std::vector<int> indices;
        cv::dnn::NMSBoxes(boxes, confidences, confidence_threshold, 0.4, indices);

        // Get the three highest confident detections
        std::vector<std::tuple<float, cv::Rect, int>> highest_outputs;
        for (int idx : indices)
        {
            highest_outputs.emplace_back(confidences[idx], boxes[idx], class_ids[idx]);
        }

        /*
        std::sort(highest_outputs.rbegin(), highest_outputs.rend());
        highest_outputs.resize(std::min(3, static_cast<int>(highest_outputs.size())));
        */
        std::sort(highest_outputs.begin(), highest_outputs.end(), compareOutput);
        highest_outputs.resize(std::min(3, static_cast<int>(highest_outputs.size())));

        // Print the three highest outputs
        for (const auto& output : highest_outputs)
        {
            float confidence;
            cv::Rect box;
            int class_id;
            std::tie(confidence, box, class_id) = output;

            int x = box.x;
            int y = box.y;
            int width = box.width;
            int height = box.height;
            std::string label = "Class " + std::to_string(class_id);
            std::cout << "Label: " << label << ", Confidence: " << std::fixed << std::setprecision(2)
                    << confidence << ", Bounding Box: (" << x << ", " << y << ", " << width << ", " << height << ")" << std::endl;
        }

    }


    return 0;
}
