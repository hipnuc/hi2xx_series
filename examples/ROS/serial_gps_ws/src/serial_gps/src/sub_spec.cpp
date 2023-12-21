//subscriber gps specifcation data package
#include <ros/ros.h>
#include <iostream>
#include <sensor_msgs/NavSatFix.h>


int main(int argc, char **argv)
{
	ros::init(argc,argv,"sub_spec");
	ros::NodeHandle n;
	execlp("rostopic", "rostopic", "echo", "/GPS_data",NULL);

	ros::spin();

}
