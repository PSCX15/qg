#include "ros/ros.h"
#include "std_msgs/String.h"
#include "qg/ordreAmiral.h"

#include <sstream>


int main(int argc, char **argv)
{

  ros::init(argc, argv, "testDeTimonier");


  ros::NodeHandle n;


  ros::Publisher chatter_pub = n.advertise<qg::ordreAmiral>("ordreDeLAmiral", 1000);

  ros::Rate loop_rate(10);

	qg::ordreAmiral msg;
  int count = 0;
  while (ros::ok())
  {
		msg.header.seq = 0;
		
		msg.header.frame_id = "test de l'amiral";

		msg.header.stamp = ros::Time::now();

		msg.vitesse = 0;
		msg.direction = 0;
		
    chatter_pub.publish(msg);

		ros::Duration(3.0).sleep();
		
		msg.header.stamp = ros::Time::now();

		msg.vitesse = 18;
		msg.direction = 50;
		
    chatter_pub.publish(msg);

		ros::Duration(3.0).sleep();


		msg.header.stamp = ros::Time::now();

		msg.vitesse = -20;
		msg.direction = 30;
		
    chatter_pub.publish(msg);

		ros::Duration(4.).sleep();
		
		msg.vitesse = 18;
		msg.direction = 50;
		
    chatter_pub.publish(msg);

		ros::Duration(3.0).sleep();
		
		msg.header.stamp = ros::Time::now();

		msg.vitesse = 0;
		msg.direction = 0;
		
    chatter_pub.publish(msg);
    
    ros::Duration(.5).sleep();
    
    msg.header.stamp = ros::Time::now();

		msg.vitesse = 0;
		msg.direction = 0;
		
    chatter_pub.publish(msg);
		
		ros::shutdown();

  }
	msg.header.stamp = ros::Time::now();

	msg.vitesse = 0;
	msg.direction = 0;
		
  chatter_pub.publish(msg);

	

  return 0;
}
