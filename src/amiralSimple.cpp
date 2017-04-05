#include "ros/ros.h"
#include "std_msgs/String.h"
#include "qg/ordreAmiral.h"

#include <sstream>

qg::ordreAmiral* msg;
ros::Publisher chatter_pub;

void ordreSimple(float motor, float direction, float temps){
	msg->vitesse = motor;
	msg->direction = direction;
	chatter_pub.publish(*msg);
	
	ros::Duration(temps).sleep();
	
	msg->vitesse = 0.0;
	msg->direction = 0.0;
	chatter_pub.publish(*msg);
	
	

}


int main(int argc, char **argv)
{

  ros::init(argc, argv, "amiralSimple");


  ros::NodeHandle n;


  chatter_pub = n.advertise<qg::ordreAmiral>("ordreDeLAmiral", 1000);

  ros::Rate loop_rate(10);

  float_t command = 0;
#if defined(WIN32) && !defined(__MINGW32__)
  sscanf_s(argv[1], "%f", &command);
#else
  sscanf(argv[1], "%f", &command);
#endif


  float_t angle = 0;
#if defined(WIN32) && !defined(__MINGW32__)
  sscanf_s(argv[2], "%f", &angle);
#else
  sscanf(argv[2], "%f", &angle);
#endif

  float_t temps = 0;
#if defined(WIN32) && !defined(__MINGW32__)
  sscanf_s(argv[3], "%f", &temps);
#else
  sscanf(argv[3], "%f", &temps);
#endif


  msg = new qg::ordreAmiral();
  
  msg->header.seq = 0;
		
	msg->header.frame_id = "base_link";

	msg->header.stamp = ros::Time::now();
	
	
	
	while(ros::ok()){
		
		
		ordreSimple(0.0,0.0,0.5);
	
		ordreSimple(command,angle,temps);
		ordreSimple(command,0.0,0.5);
		ordreSimple(0.0,0.0,1);
		
		ros::spinOnce();
		ros::shutdown();
		
	}
  

	

  return 0;
}
