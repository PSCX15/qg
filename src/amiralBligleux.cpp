
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

bool pathClear(){
	
	return false;
	}


int main(int argc, char **argv)
{

  ros::init(argc, argv, "amiralBigleux");


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


  msg = new qg::ordreAmiral();
  
  msg->header.seq = 0;
		
	msg->header.frame_id = "base_link";

	msg->header.stamp = ros::Time::now();
	
	ordreSimple(0.0,0.0,0.5);
	
	while(ros::ok()){
		
		if(pathClear()){
		ordreSimple(command,angle,0.5);
		}
		
		else{
		ordreSimple(0.0,0.0,0.5);
		}
		
		ros::spinOnce();
		
		if(ros::isShuttingDown()){
			ordreSimple(0.0,0.0,1);
		}
	}
  
  
	ordreSimple(0.0,0.0,1);
	ros::shutdown();

  return 0;
}
