#include "ros/ros.h"
#include "std_msgs/String.h"
#include "qg/ordreAmiral.h"

#include <sstream>

qg::ordreAmiral msg;
ros::Publisher chatter_pub;

void ordreSimple(float motor, float direction, float temps){
	msg.vitesse = motor;
	msg.direction = direction;
	chatter_pub.publish(msg);
	
	ros::Duration(temps).sleep();
	
	msg.vitesse = 0.0;
	msg.direction = 0.0;
	chatter_pub.publish(msg);
	
	

}


int main(int argc, char **argv)
{

  ros::init(argc, argv, "amiralSimple");


  ros::NodeHandle n;


  chatter_pub = n.advertise<qg::ordreAmiral>("ordreDeLAmiral", 1000);

  ros::Rate loop_rate(10);


  
  
  msg.header.seq = 0;
		
	msg.header.frame_id = "base_link";

	msg.header.stamp = ros::Time::now();
	
	ordreSimple(0.0,0.0,500);
	
	ordreSimple(20.0,0.0,5000);
	
	ordreSimple(0.0,0.0,1000);
  
  
  
  ros::spin();
	

  return 0;
}
