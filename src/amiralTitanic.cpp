//~ #include <ros/ros.h>
#include "ros/ros.h"
//~ #include "std_msgs/String.h"
#include "qg/ordreAmiral.h"

//~ #include <sstream>
#include <cmath>

#include <tf/transform_listener.h>
//#include <occupancy_grid_utils/exceptions.h>
#include <nav_msgs/MapMetaData.h>
#include <nav_msgs/OccupancyGrid.h>

#define ROBOT_WIDTH 0.40
#define SAFE_DISTANCE 0.5

qg::ordreAmiral* msg;

nav_msgs::OccupancyGrid grid;
nav_msgs::MapMetaData meta;


ros::Publisher chatter_pub;
ros::Subscriber rtab_listener;
double robot_x, robot_y, robot_yaw, hz_dx, hz_dy, vt_dx, vt_dy;
double resolution, grid_x, grid_y;
int grid_height, grid_width;
bool valid_grid;
bool needDisplay;

int getCellIndex(double x, double y);
void gridListener(const nav_msgs::OccupancyGrid& msg);
void ordreSimple(float motor, float direction, float temps);
bool testPath();
void generateDx();

void gridListener(const nav_msgs::OccupancyGrid& msg)
{
	grid = msg;
	meta = grid.info;
	grid_x = meta.origin.position.x;
	grid_y = meta.origin.position.y;
	resolution = meta.resolution;
	grid_height = meta.height;
	grid_width = meta.width;
	//~ ROS_INFO("Updated map . Now %d %d", grid_height, grid_width);
	//~ std::cout << getCellIndex(grid_x , grid_y) << ":" << getCellIndex(grid_x + resolution, grid_y) << ":" << getCellIndex(grid_x, grid_y + resolution) <<std::endl;
	//~ std::cout << getCellIndex(grid_x + resolution, grid_y + resolution) << ":" << getCellIndex(grid_x + 2*resolution, grid_y) << ":" << getCellIndex(grid_x, grid_y + 2*resolution) <<std::endl;
	valid_grid = true;
	needDisplay = true;
}

void ordreSimple(float motor, float direction, float temps){
	msg->vitesse = motor;
	msg->direction = direction;
	chatter_pub.publish(*msg);
	
	ros::Duration(temps).sleep();
	
	msg->vitesse = 0.0;
	msg->direction = 0.0;
	chatter_pub.publish(*msg);

}

void printGrid(int8_t pgrid[]){
	if(needDisplay){
		std::cout<< "Printing Grid "<< grid_height << "x" << grid_width << "  " << grid_x << ";" << grid_y << "/" << resolution << std::endl ;
		int8_t val;
		for(int j = 0; j < grid_height ; j++){
			for(int i = 0 ; i < grid_width ; i++){
				val = pgrid[ i + (grid_height - j -1)* grid_width];
				if(val == 100){
					std::cout<<".";
				} else if(val == 101){
					std::cout<<"R";
				} else if(val == 102){
					std::cout<<"X";
				} else{
					std::cout<<" ";
				}
			}
			std::cout<<"|"<<std::endl;
			needDisplay = false;
		}
	}
}

void printBothGrid(int8_t pgrid[]){
	if(needDisplay){
		std::cout<< "Printing Grid "<< grid_height << "x" << grid_width << "  " << grid_x << ";" << grid_y << "/" << resolution << std::endl ;
		int8_t val;
		for(int j = 0; j < grid_height ; j++){
			for(int i = 0 ; i < grid_width ; i++){
				val = pgrid[ i + (grid_height - j -1)* grid_width];
				if(val == 100){
					std::cout<<".";
				} else if(val == 101){
					std::cout<<"R";
				} else if(val == 102){
					std::cout<<"X";
				} else{
					std::cout<<" ";
				}
			}
			std::cout<<" | ";
			for(int i = 0 ; i < grid_width ; i++){
				val = grid.data[ i + (grid_height - j -1)* grid_width];
				if(val == 100){
					std::cout<<".";
				} else if(val == 101){
					std::cout<<"R";
				} else if(val == 102){
					std::cout<<"X";
				} else if(val == 0){
					std::cout<<"0";
				} else{
					std::cout<<" ";
				}
			}
			std::cout << "|" << std::endl
		}
		needDisplay = false;
		
	}
}

void printOccupancyGrid(){
	if(needDisplay){
		std::cout<< "Printing OccupancyGrid "<< std::endl;
		int8_t val;
		for(int j = 0; j < grid_height ; j++){
			for(int i = 0 ; i < grid_width ; i++){
				val = grid.data[ i + (grid_height - j -1)* grid_width];
				if(val == 100){
					std::cout<<".";
				} else if(val == 101){
					std::cout<<"R";
				} else if(val == 102){
					std::cout<<"X";
				} else if(val == 0){
					std::cout<<"0";
				} else{
					std::cout<<" ";
				}
			}
			std::cout<<"|"<<std::endl;
		}
	}
}

bool testPath(){
	//~ ROS_INFO("testing path");
	int w = (int) ceil(ROBOT_WIDTH / resolution / 2);
	int h = (int) ceil(SAFE_DISTANCE / resolution );
	int index;
	int8_t value;
	bool pathClear = true;
	
	if(!valid_grid){
		return true;
	}
	int8_t requestedCells[grid_height*grid_width] = {0};
	//~ printOccupancyGrid();
	//pirnt position of robot
	index = getCellIndex(robot_x, robot_y);
	//~ if(index != -1){
		//~ ROS_INFO("index of robot %d", index);
		//~ requestedCells[index] = 101;
	//~ } else {
		//~ ROS_INFO("robot not found ? R : %lf %lf G : %lf %lf  res : %lf", robot_x, robot_y, grid_x, grid_y, resolution);
	//~ }
	for(int i = -w; i <= w ; i++){
		// width of robot
		for(int j = 0; j < h ; j++){
			index = getCellIndex(robot_x + i * hz_dx + j * vt_dx, robot_y + i * hz_dy + j * vt_dy);
			if(index != -1){
				//~ ROS_INFO("lindex %d", index);
				value =  grid.data[index];
				requestedCells[index] = 100;
				//~ ROS_INFO("value %d", value);
				if(value > 60){
					//occupied cell
					requestedCells[index] = 102;
					//~ printGrid(requestedCells);
					//~ ROS_INFO("Found an occupied cell in coordinates %d;%d. Stopping.", i, j);
					pathClear = false;
					
				}
			}
		}
	}
	index = getCellIndex(robot_x, robot_y);
	if(index != -1){
		//~ ROS_INFO("index of robot %d", index);
		requestedCells[index] = 101;
	} else {
		ROS_INFO("robot not found ? R : %lf %lf G : %lf %lf  res : %lf", robot_x, robot_y, grid_x, grid_y, resolution);
	}
	printBothGrid(requestedCells);
	return pathClear;
}

void generateDx(){
	//called to update grid of points to test
	vt_dx = cos(robot_yaw) * resolution;
	vt_dy = sin(robot_yaw) * resolution;
	hz_dx = vt_dy;
	hz_dy = - vt_dx;
}

int getCellIndex(double x, double y){
	//checkBounds
	if( x < grid_x  || x > grid_x + grid_width * resolution || y < grid_y || y > grid_y + grid_height * resolution){
		//out of bounds
		
		return -1;
	}
	//double dx = x - robot_x, dy = y - robot_y;
	double dx = x - grid_x, dy = y - grid_y;
	int ix = (int) floor(dx / resolution );
	int iy = (int) floor(dy / resolution );
	int index = iy * grid_width + ix;
	return index;
}


//~ bool pathClear(){
	
	//~ return testPath();
	//~ }

int main(int argc, char** argv){
	double robot_x, robot_y, robot_yaw;
  ros::init(argc, argv, "titanic");

  ros::NodeHandle node;
  ros::Rate loop_rate(10);
  
  chatter_pub = node.advertise<qg::ordreAmiral>("ordreDeLAmiral", 1000);
  
  //declare le subscriber pour la grid
  ros::Subscriber grid_listener = node.subscribe("/rtabmap/grid_map", 100, gridListener);

  tf::TransformListener listener;
  
  //definiiotn des parametres de la commande
  float_t command = 20;
	//~ #if defined(WIN32) && !defined(__MINGW32__)
		//~ sscanf_s(argv[1], "%f", &command);
	//~ #else
		//~ sscanf(argv[1], "%f", &command);
	//~ #endif


  float_t angle = 0;
	//~ #if defined(WIN32) && !defined(__MINGW32__)
		//~ sscanf_s(argv[2], "%f", &angle);
	//~ #else
		//~ sscanf(argv[2], "%f", &angle);
	//~ #endif


  msg = new qg::ordreAmiral();
  msg->header.seq = 0;
	msg->header.frame_id = "base_link";
	msg->header.stamp = ros::Time::now();
	ordreSimple(0.0,0.0,0.5);
	
	
	
	
	while(ros::ok()){
		//recuperation de la localistation du robot
		tf::StampedTransform transform;
    try{
			ros::Time now = ros::Time::now();
			listener.waitForTransform("/map", "/base_link",
                              now, ros::Duration(3.0));
			
      listener.lookupTransform("/map", "/base_link",
                               ros::Time(0), transform);
    }
    catch (tf::TransformException &ex) {
      ROS_ERROR("%s",ex.what());
      ros::Duration(1.0).sleep();
      continue;
    }
    robot_x = transform.getOrigin().x();
    robot_y = transform.getOrigin().y();
    robot_yaw = tf::getYaw(transform.getRotation());
    generateDx();

		//~ ROS_INFO("%lf, %lf, %lf", robot_x, robot_y, robot_yaw);

		//decision
		if(testPath()){
		ordreSimple(command,angle,0.5);
		} else {
		ordreSimple(0.0,0.0,0.5);
		}
		
		//~ ROS_INFO("launched orders");
		ros::spinOnce();
		loop_rate.sleep();
		
		if(ros::isShuttingDown()){
			ROS_INFO("Detected shutdown request");
			ordreSimple(0.0,0.0,1);
		}
	}
  ROS_INFO("Detected shutdown request");
  //terminaison
	ordreSimple(0.0,0.0,1);
	ros::shutdown();

  return 0;
};
