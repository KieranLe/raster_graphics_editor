// Name: Kieran Le


#include <iostream>
#include <string>
#include "opencv2/opencv.hpp"

// configuration parameters
#define NUM_COMMAND_LINE_ARGUMENTS 1

int x_1, y_1;       // Global varialbe to store coordinate of the first left click
cv::Vec3b eyeDropper(255,255,255);  //Set the inital value of [eyeDropper] tool to white
cv::Mat imageIn;    // Orignal image
cv::Mat tempImage;  // A copy of orignal image for [Reset] tool
int state = 1;      // 1 = EyeDropper tool (default)
                    // 2 = Crop tool
                    // 3 = Pencil tool
                    // 4 = Paint Bucket tool
                    // 5 = Reset tool

static void clickCallBack( int event, int x, int y, int flags, void* param);


static void clickCallBack(int event, int x, int y, int flags, void* param)
{
    // cast userdata to a cv::Mat
    cv::Mat imageIn = *(cv::Mat *)param;
    cv::Point p1,p2;
    

    // Requirement 2: EYEDROPPER
    // state == 2 just incase user uses the [CROP] tool which requires left click -> hold -> release
    if( event == cv::EVENT_LBUTTONDOWN && ( state == 1 || state == 2 ) )
    {
        // get the color value at the clicked pixel location and print to console
        // Only store value of [eyeDropper] when state = 1, avoiding store color when using [CROP] tool
        if( state == 1)
        {
            eyeDropper = imageIn.at<cv::Vec3b>(y,x);
            std::cout << "The current color value = " << eyeDropper << std::endl;
        }

        // Store the current coordinate to new variable to use for other tool 
        x_1 = x;
        y_1 = y;
        
    }
    // Requirement 3: CROP
    else if( state == 2 && event == cv::EVENT_LBUTTONUP )
    {
        // Create region of interest (ROI)
        cv::Point p1(x_1, y_1); // the intial left click
        cv::Point p2(x,y);      // coordinate when left click is hold then release
        cv::Rect region(p1,p2);

        // Crop the ROI
        imageIn = imageIn(region);
        // Remove the current window and show the new window for edited imaged
        // Called the [setMouseCallback] again so tools can be used on this current image 
        cv::destroyAllWindows();
        cv::imshow("imageResized", imageIn);
        cv::setMouseCallback("imageResized", clickCallBack, &imageIn);
        cv::waitKey();

    }
    // Requirement 4: Pencil
    // Usage: Pick a starting location with left click, hold it, drag the mouse around then release
    else if( state == 3 && event == cv::EVENT_LBUTTONDOWN )
    {
        // Change current pixel color with the [eyeDropper] value from eyeDropper tool
        imageIn.at<cv::Vec3b>( cv::Point(x,y) ) = eyeDropper;

        cv::destroyAllWindows();
        cv::imshow("changeColor", imageIn);
        cv::setMouseCallback("changeColor", clickCallBack, &imageIn);
        cv::waitKey();
    }
    // Requirement 4: Pencil (drags left mouse)
    else if( state == 3 && event == cv::EVENT_MOUSEMOVE  ) 
    {
        imageIn.at<cv::Vec3b>( cv::Point(x,y) ) = eyeDropper;
        
        // Check if the left mouse is released
        if( event == cv::EVENT_LBUTTONUP )
        {
            cv::destroyAllWindows();
            cv::imshow("dragColor", imageIn);
            cv::setMouseCallback("dragColor", clickCallBack, &imageIn);
            cv::waitKey();
        }
       
    }
    // Requirement 5: Paint Bucket
    else if( state == 4 && event == cv::EVENT_LBUTTONDOWN )
    {
        cv::destroyAllWindows();
        // Flood fill the current location color with [eyeDropper] color with 4-connectivity
        cv::floodFill(imageIn, cv::Point(x,y), eyeDropper, (cv::Rect*)0, 4);

        cv::imshow("paintBucket", imageIn);
        cv::setMouseCallback("paintBucket", clickCallBack, &imageIn);
        cv::waitKey();

    }
    // Requirement 6: Reset 
    else if( state == 5 && event == cv::EVENT_LBUTTONDBLCLK )
    {
        cv::destroyAllWindows();
        // Clone back the original image for [Reset] tool
        imageIn = tempImage.clone();
        cv::imshow("OGImage", imageIn);
        cv::setMouseCallback("OGImage", clickCallBack, &imageIn);
        cv::waitKey();
    }
    // Switching between tools with Right Click event
    else if( event == cv::EVENT_RBUTTONDOWN )
    {
        // Default value of state is 1 (eyeDropper)
        // Increase 1 every time right click is detected
        // if state is at 5, reset value back to 1 (eyeDropper tool)
        // Includes print statements to let the user know which tool is being used at the moment
        if( state == 5 )
        {
            state = 1;
            std::printf("\n---[EyeDropper] tool is being used--- \n");
        }
        else
        {
            state++;
            if(state == 2)
                std::printf("\n---[Crop] tool is being used--- \n");
            else if( state == 3 )
                std::printf("\n---[Pencil] tool is being used--- \n");
            else if( state == 4 )
                std::printf("\n---[Paint Bucket] tool is being used--- \n");
            else if( state == 5 )
                std::printf("\n---[Reset] tool is used--- \n");
        }
    }
}


int main( int argc, char **argv)
{
    // validate and parse the command line arguments
    if( argc != NUM_COMMAND_LINE_ARGUMENTS + 1 )
    {
        std::printf("USAGE: %s <image_path> \n ", argv[0]);
        return 0;
    }
    else
    {
        // open the input image
        std::string inputFileName = argv[1];

        imageIn = cv::imread(inputFileName, cv::IMREAD_COLOR);

        // store the OG image
        tempImage = imageIn.clone();

        // check for file error
        if( !imageIn.data )
        {
            std::cout << "Error while opening file " << inputFileName << std::endl;
            return 0;
        }


        // get the image size
        std::cout << "image width: " << imageIn.cols << std::endl;
        std::cout << "image height: " << imageIn.rows << std::endl;
        std::cout << "image channels: " << imageIn.channels() << std::endl;
        
        // get the image size using the size() member function
        std::cout << "image width: " << imageIn.size().width << std::endl;
        std::cout << "image height: " << imageIn.size().height << std::endl;


        cv::imshow("imageIn", imageIn);
        cv::setMouseCallback("imageIn", clickCallBack, &imageIn);
        cv::waitKey();
        
    }

    return 0;
}