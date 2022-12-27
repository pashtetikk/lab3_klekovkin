#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <std_msgs/String.h>

#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>

#include <map>

std::map<char, std::vector<float>> moveBindings
{
    {'w', {1, 0}},
    {'W', {1, 0}},
    {'a', {0, 1}},
    {'A', {0, 1}},
    {'s', {-1, 0}},
    {'S', {-1, 0}},
    {'d', {0, -1}},
    {'D', {0, -1}}, 

};

static float speed(0.5); // Linear velocity (m/s)
static float turn(1.0); // Angular velocity (rad/s)
static float lin(0), ang(0); // Forward/backward/neutral direction vars
static char key(' ');

int getch(void)
{
  int ch;
  struct termios oldt;
  struct termios newt;

  // Store old settings, and copy to new settings
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;

  // Make required changes and apply the settings
  newt.c_lflag &= ~(ICANON | ECHO);
  newt.c_iflag |= IGNBRK;
  newt.c_iflag &= ~(INLCR | ICRNL | IXON | IXOFF);
  newt.c_lflag &= ~(ICANON | ECHO | ECHOK | ECHOE | ECHONL | ISIG | IEXTEN);
  newt.c_cc[VMIN] = 1;
  newt.c_cc[VTIME] = 0;
  tcsetattr(fileno(stdin), TCSANOW, &newt);

  // Get the current character
  ch = getchar();

  // Reapply old settings
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

  return ch;
}

int main(int argc, char **argv){
    ros::init(argc, argv, "lab3_node");
    ros::NodeHandle nh;
    ros::Publisher cmdPub = nh.advertise<geometry_msgs::Twist>("cmd_vel", 100);
    ros::Publisher debPub = nh.advertise<std_msgs::String>("debTopic", 100);

    static geometry_msgs::Twist vel;
    std_msgs::String str;
    str.data = std::string("FU");
    vel.linear.x = 1;
    cmdPub.publish(vel);
    while(ros::ok()){
        key = getch();
        if (moveBindings.count(key) == 1)
        {
            // Grab the direction data
            lin = moveBindings[key][0];
            ang = moveBindings[key][1];
        }
        else
        {
            lin = 0;
            ang = 0;
        }

        if (key == '\x03')
        {
            vel.linear.x = 0;
            vel.linear.y = 0;
            cmdPub.publish(vel);
            break;
        }

        vel.linear.x = lin * speed;
        vel.angular.z = ang * speed;

        cmdPub.publish(vel);
        //debPub.publish(str);
        ros::spinOnce();
        ros::Duration(0.1).sleep();
    }
    
    return 0;
}