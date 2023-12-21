//serial_gps.cpp
#include <ros/ros.h>
#include <serial/serial.h>
#include <iostream>
#include <stdio.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sensor_msgs/NavSatFix.h>

#ifdef __cplusplus 
extern "C"{
#endif

#include <stdint.h>
#include <stdbool.h>

#include "decode_nmea.h"

#define GPS_SERIAL   "/dev/ttyUSB0"
#define BAUD         (115200)
#define BUF_SIZE     (1023)
#define RAD_TO_DEG   (57.29577951308232)

void publish_gps_data(nmea_raw_t *data, sensor_msgs::NavSatFix *gps_data);

#ifdef __cplusplus
}
#endif

static nmea_raw_t raw;

static uint8_t buf[2048];

int main(int argc, char** argv)
{
	int rev = 0;
	ros::init(argc, argv, "serial_GPS");
	ros::NodeHandle n;

	ros::Publisher GPS_pub = n.advertise<sensor_msgs::NavSatFix>("/GPS_data", 20);

	serial::Serial sp;

	serial::Timeout to = serial::Timeout::simpleTimeout(100);

	sp.setPort(GPS_SERIAL);

	sp.setBaudrate(BAUD);

	sp.setTimeout(to);
	
	try
	{
		sp.open();
	}
	catch(serial::IOException& e)
	{
		ROS_ERROR_STREAM("Unable to open port.");
		return -1;
	}
    
	if(sp.isOpen())
	{
		ROS_INFO_STREAM("/dev/ttyUSB0 is opened.");
	}
	else
	{
		return -1;
	}
	
	ros::Rate loop_rate(500);
	sensor_msgs::NavSatFix gps_data;

	while(ros::ok())
	{
		size_t num = sp.available();
		if(num!=0)
		{
			uint8_t buffer[BUF_SIZE]; 
	
			if(num > BUF_SIZE)
				num = BUF_SIZE;
			
			num = sp.read(buffer, num);
			if(num > 0)
			{
				gps_data.header.stamp = ros::Time::now();
				gps_data.header.frame_id = "base_link";


				for (int i = 0; i < num; i++)
				{
					rev = decode_nmea(&raw, buffer[i]);
					if(rev)
					{
						publish_gps_data(&raw, &gps_data);
						GPS_pub.publish(gps_data);
					}
				}
			}
		}
		loop_rate.sleep();
	}
    
	sp.close();
 
	return 0;
}

void publish_gps_data(nmea_raw_t *data, sensor_msgs::NavSatFix *gps_data)
{
	//nav sat fix -1 ---> NO FIX    0 ---> FIX   1 ---> SBAS FIX   2 ---> GBAS FIX  
	switch(data->solq)
	{
		case SOLQ_NONE:
			gps_data->status.status = -1;
			break;
		case SOLQ_SINGLE:
			gps_data->status.status = 0;
			break;
		case SOLQ_FIX:
			gps_data->status.status = 2;
			break;
		case SOLQ_SBAS:
			gps_data->status.status = 1;
			break;
		default:
			break;
	}
	
	gps_data->status.service = 1;

	gps_data->latitude = data->lat * RAD_TO_DEG;
	gps_data->longitude = data->lon * RAD_TO_DEG;
	gps_data->altitude = data->alt;
}

