#include "ros/ros.h"
#include "command.h"
#include "qg/servo_command.h"
#include "qg/ordreAmiral.h"
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>
#include "std_msgs/String.h"
#include "std_msgs/Bool.h"
#include "nav_msgs/Odometry.h"
#include <utility>  
#include <cmath>
#include <tf2/LinearMath/Quaternion.h>

#define MILLIS 1000

using std::vector;
using std::string;
using std::sort;
using std::reverse;

ros::Publisher commandRobotLocalization;
ros::Publisher orientationRobotLocalization;

float motorCommand = 0.0;

float coef = 0.017;
float coef2 = 0.0084;
std::pair<float,float> posActu;  
std::pair<float,float> posAvant;
tf2::Quaternion q;

nav_msgs::Odometry msgCommand;
nav_msgs::Odometry msgOrientation;

float covarianceCommand = 0.0;
float covarianceOrientation = 0.0;

void callBackVitesse(const qg::servo_command::ConstPtr& msg)
{
	if(msg->device=="motor"){
		if(msg->value==0.0){
			motorCommand = 0.0;
			covarianceCommand = 0.0001;
		}
		else if(msg->value >0.0){
			motorCommand = coef * msg->value;
			covarianceCommand = 0.07*0.07;
		}
		else {
			motorCommand = coef2 * msg->value;
			covarianceCommand = 0.06*0.06;
		}
	}
}

void callBackOrientation(const nav_msgs::Odometry::ConstPtr& msg)
{
	posAvant = posActu;
	posActu = std::make_pair(msg->pose.pose.position.x,msg->pose.pose.position.y);
	
	float delta_x = posActu.first - posAvant.first;
	float delta_y = posActu.second - posAvant.second;
	float distance = sqrt(delta_x*delta_x+delta_y*delta_y);
	
	delta_x/=distance;
	delta_y/=distance;
	
	float theta = 0.0;
	
	if(delta_y>=0.0){
		theta =  acos(delta_x);
	}
	else{
		theta = -acos(delta_x);
	}
	
	if(motorCommand<0){
		theta += 3.1415296;  
	}

	q.setRPY(0.0,0.0,theta);
	if(distance>0.0){
		covarianceOrientation = 0.01/distance;
	}
}



void send(const ros::TimerEvent&){
	
	msgCommand.twist.twist.linear.x = motorCommand;
	msgCommand.twist.covariance[0]=covarianceCommand;
	msgCommand.header.stamp = ros::Time::now();
	
	commandRobotLocalization.publish(msgCommand);
	
	
	msgOrientation.header.stamp = ros::Time::now();
	msgOrientation.pose.pose.orientation.x = q.x();
	msgOrientation.pose.pose.orientation.y = q.y();
	msgOrientation.pose.pose.orientation.z = q.z();
	msgOrientation.pose.pose.orientation.w = q.w();
	msgOrientation.pose.covariance[35]=covarianceOrientation;
	
	orientationRobotLocalization.publish(msgOrientation);
	
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "assistantTimonier");
  ros::NodeHandle n;

	posActu = std::make_pair(0.0,0.0);
	posAvant = std::make_pair(0.0,0.0);
	
	ros::Subscriber command_sub = n.subscribe("servo_command", 1000, callBackVitesse);
	ros::Subscriber orientation_sub = n.subscribe("/odometry/filtered", 1000, callBackOrientation);
	commandRobotLocalization = n.advertise<nav_msgs::Odometry>("odometry/command",1000);
	orientationRobotLocalization = n.advertise<nav_msgs::Odometry>("odometry/orientation",1000);
	
	msgCommand.header.frame_id = "odom";
	msgCommand.child_frame_id = "base_link";
	msgCommand.twist.twist.linear.x = motorCommand;
	msgCommand.twist.covariance[0]=covarianceCommand;
	
	msgOrientation.header.frame_id = "odom";
	msgOrientation.child_frame_id = "base_link";
	msgOrientation.pose.pose.orientation.x = 0.0;
	msgOrientation.pose.pose.orientation.y = 0.0;
	msgOrientation.pose.covariance[0]=0.0;
	
	
	ros::Timer publisher = n.createTimer(ros::Duration(0.1), send);
  
	ros::spin();
	return 0;
}
