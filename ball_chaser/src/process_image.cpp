#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    ROS_INFO_STREAM("Moving the arm to the center");
ROS_INFO("Driving robot with velocity linear:%1.2f, angular:%1.2f", lin_x, ang_z);

    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;

    // Call the safe_move service and pass the requested joint angles
    if (!client.call(srv))
        ROS_ERROR("Failed to call service command_robot");
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;
  float ball_position;
 float lin_x,ang_z;
bool white_ball = false;
for (int i=0;i<img.width*img.height; i++)
{
if (img.data[i*3]+img.data[i*3+1]+img.data[i*3+2] == white_pixel*3)
{
white_ball = true;
ball_position = (float)(i%img.width)/img.width;
break;
}
}

if (white_ball == true)
{
 if (ball_position < 0.333) 
{
	    drive_robot(0.2,0.2); }
else if(ball_position >= 0.333 && ball_position < 0.667)
{           
drive_robot(0.2,0.0);
    }
else
{
	    drive_robot(0.2,-0.2);
    }
}

if ( white_ball == false)
{drive_robot(0,0);
}
    
    }


int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
