#include "ros/ros.h"
//#include <algorithm>
#include "command.h"
#include "qg/servo_command.h"
#include "qg/ordreAmiral.h"

#include "std_msgs/String.h"
#include "std_msgs/Bool.h"

#define MILLIS 1000

using std::vector;
using std::string;
using std::sort;
using std::reverse;

ros::Publisher command_pub;
//vector<Command*> commandList;
//vector<Command*> sentCommandList;
ros::Time tstart;
Command* currentCommand;
//Command* previousCommand;
Command* askedCommand;
Command* lastSentCommand;
std::string id;
bool isReversing;

// -----------------------------------partie autorisation emergency ------------------------

bool autorisation = false;
ros::Time lastbeat;


void heartbeat(const std_msgs::Bool::ConstPtr& msg){
	if(msg->data){lastbeat = ros::Time::now();}
}

void testTimeOut(const ros::TimerEvent&){
	if(ros::Time::now()-lastbeat>=ros::Duration(2.0)){
		autorisation = false;	
		qg::servo_command motorCommand;
		motorCommand.device = "motor";
		motorCommand.value= 0.0;
		lastSentCommand->value = 0.0;
		currentCommand->value = 0.0;
		currentCommand->angle = 0.0;
		command_pub.publish(motorCommand);
	}
	else{
		autorisation=true;
	}
}

// -----------------------------------fin de cette partie -----------------------------------




void launchCommand(const ros::TimerEvent&){
	if(isReversing==true || !autorisation){return;}
	else{
		qg::servo_command motorCommand;
		motorCommand.device = "motor";
		qg::servo_command gearCommand;
		gearCommand.device = "vitesse";
		gearCommand.value = 2;
		qg::servo_command directionCommand;
		directionCommand.device = "direction";
		
		float Vprecedent = lastSentCommand->value;
		float Vcible = currentCommand->value;
		
		double Vincrement = std::min((double)std::abs(Vprecedent-Vcible), 2.0);
		
		if(Vprecedent > Vcible){
			motorCommand.value= Vprecedent - Vincrement;
		}
		else{
			motorCommand.value= Vprecedent + Vincrement;
		}
		
		
		float Dprecedent = lastSentCommand->angle;
		float Dcible = currentCommand->angle;
		
		double Dincrement = std::min((double)std::abs(Dprecedent-Dcible), 10.0);
		
		if(motorCommand.value < 10 && motorCommand.value > -25){
			directionCommand.value = Dprecedent;
		}
		
		else if(Dprecedent > Dcible){
			directionCommand.value= Dprecedent - Dincrement;
		}
		else{
			directionCommand.value= Dprecedent + Dincrement;
		}
		
		if(directionCommand.value == 10){
			directionCommand.value = 9;
		}
		
		command_pub.publish(motorCommand);
		command_pub.publish(directionCommand);
		
		lastSentCommand->value = motorCommand.value;
		lastSentCommand->angle = directionCommand.value;
		lastSentCommand->stamp = ros::Time::now();
		return;
	}

}

void reverse(){
	if(autorisation){
		qg::servo_command servoCommand;
		
		ROS_INFO("warning : reversing!");
		
		servoCommand.device = "motor";
		
		servoCommand.value = 0;
  	command_pub.publish(servoCommand);
  	ros::Duration(0.1).sleep();
  	servoCommand.value = -5;
  	command_pub.publish(servoCommand);
  	ros::Duration(0.1).sleep();
  	servoCommand.value = -11.45;
  	command_pub.publish(servoCommand);
  	ros::Duration(0.15).sleep();
  	servoCommand.value = -11.5;
  	command_pub.publish(servoCommand);
  	ros::Duration(0.15).sleep();
  	servoCommand.value = -11.55;
  	command_pub.publish(servoCommand);
  	ros::Duration(0.1).sleep();
//	  servoCommand.value = -25;
//	  command_pub.publish(servoCommand);
//	  ros::Duration(0.5).sleep();
  	
  	ROS_INFO("warning : reserving done !");
  	
  	lastSentCommand->value = -15;
  	lastSentCommand->stamp = ros::Time::now();
  }
}


void enAvantToute(const qg::ordreAmiral::ConstPtr& msg)
{
	if(isReversing==true)return;
	
	//commandList.push_back(new Command(previousCommand->stamp,previousCommand->value,previousCommand->angle ));
	//previousCommand = currentCommand;
	askedCommand = new Command(msg->header.stamp, msg->vitesse , msg->direction);
	
	if ((lastSentCommand->value)*(askedCommand->value)>0){
		currentCommand = askedCommand;
	}
	else{
		if(askedCommand->value >= 0){
			currentCommand = askedCommand;
		}
		else{
			
			isReversing=true;
			reverse();
			isReversing=false;
			currentCommand = askedCommand;			
		}
	}
}



int main(int argc, char **argv)
{
  ros::init(argc, argv, "timonier");
  ros::NodeHandle n;

	//emergencyBreak on:
  ros::Subscriber emergencyBond = n.subscribe("bondServo",100,heartbeat);
  ros::Publisher iAmAlive = n.advertise<std_msgs::String>("servoListenerIsAlive",1000);
  lastbeat = ros::Time::now();



  //déclare le publisher pour le topic servo_command
  command_pub = n.advertise<qg::servo_command>("servo_command", 1000);
  
  ros::Subscriber ordre = n.subscribe("ordreDeLAmiral", 1000, enAvantToute);
  isReversing=false;
  tstart = ros::Time::now();
  lastSentCommand = new Command(tstart,1,0);
  currentCommand = new Command(tstart,1,0);
  askedCommand = new Command(tstart,1,0);
  ros::Rate loop_rate(10);
	
  //lecture du tableau de paramètres
  
  ros::Timer timer = n.createTimer(ros::Duration(0.1), launchCommand);
  ros::Timer heartbeat = n.createTimer(ros::Duration(0.1), testTimeOut);
  
  //ros::Subscriber sub = n.subscribe("servo_command", 100, interpretCommand);
  ros::spin();

  return 0;
}
