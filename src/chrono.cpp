#include "ros/ros.h"
#include <algorithm>
#include "command.h"
#include "qg/servo_command.h"

#define MILLIS 1000

using std::vector;
using std::string;
using std::sort;
using std::reverse;

ros::Publisher command_pub;
vector<Command> commandList;
ros::Time tstart;


void launchCommand(const ros::TimerEvent&);

int main(int argc, char **argv)
{
  ros::init(argc, argv, "chrono");
  ros::NodeHandle n;

  //déclare le publisher pour le topic servo_command
  command_pub = n.advertise<qg::servo_command>("servo_command", 100);
  ros::Rate loop_rate(10);

  //lecture du tableau de paramètres
  vector<string> strCommands;
  n.getParam("chrono_command", strCommands);
  for(int i=0; i<strCommands.size(); i++)
  {
    commandList.push_back(Command(strCommands[i]));
  }
  //trier le tableau pour que le premier élément à tirer soit à la fin.
  std::sort(commandList.begin(), commandList.end());
  std::reverse(commandList.begin(), commandList.end());
  
  ROS_INFO("chrono: got %lu commands", commandList.size());

  //définition du timer
  tstart = ros::Time::now();
  ros::Timer timer = n.createTimer(ros::Duration(0.1), launchCommand);
  
  
  //ros::Subscriber sub = n.subscribe("servo_command", 100, interpretCommand);
  ros::spin();

  return 0;
}




void launchCommand(const ros::TimerEvent& tevent )
{
  Command cmd;
  qg::servo_command servoCommand;
  while(commandList.back().time < MILLIS * (tevent.current_real - tstart).toSec())
  {
    //le dernier message de CommandList doit etre envoyé
    cmd = commandList.back();
    servoCommand.device = cmd.device;
    servoCommand.value = cmd.value;
    ROS_INFO("%d : executed (%s,%f) planned at time %d ", (int) (MILLIS * (tevent.current_real - tstart).toSec()), cmd.device.c_str(), cmd.value, (int)cmd.time);
    command_pub.publish(servoCommand);
    commandList.pop_back();
    if(commandList.empty())
    {//no more commands to execute...
      ros::shutdown();
    }
  }
}
    
    
    
    
    
