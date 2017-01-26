#include "ros/ros.h"
#include <algorithm>
#include "command.h"
#include "qg/servo_command.h"
#include "qg/ordreAmiral.h"

#define MILLIS 1000

using std::vector;
using std::string;
using std::sort;
using std::reverse;

ros::Publisher command_pub;
vector<Command*> commandList;
ros::Time tstart;
Command* currentCommand;
Command* previousCommand;
Command* askedCommand;

void enAvantToute(const qg::ordreAmiral::ConstPtr& msg)
{
	commandList.push_back(new Command(previousCommand->stamp,previousCommand->value,previousCommand->angle ));
	previousCommand = currentCommand;
	askedCommand = new Command(msg->header.stamp, msg->vitesse , msg->direction);
}


void launchCommand(const ros::TimerEvent&){


}



int main(int argc, char **argv)
{
  ros::init(argc, argv, "timonier");
  ros::NodeHandle n;

  //déclare le publisher pour le topic servo_command
  command_pub = n.advertise<qg::servo_command>("servo_command", 1000);
  
  ros::Subscriber ordre = n.subscribe("ordreDeLAmiral", 1000, enAvantToute);
  
  
  ros::Rate loop_rate(10);

  //lecture du tableau de paramètres
  
  ros::Timer timer = n.createTimer(ros::Duration(0.1), launchCommand);
  
  //ros::Subscriber sub = n.subscribe("servo_command", 100, interpretCommand);
  ros::spin();

  return 0;
}
