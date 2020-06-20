#include <ros/ros.h>
#include <geometry_msgs/Pose.h>
#include <nav_msgs/Odometry.h>
#include <math.h>


int main(int argc, char **argv)
{
    ros::init(argc, argv, "publish_position");
    ros:: NodeHandle nh;
    ros::Subscriber subBot;
    ros::Publisher pubBot;
    pubBot = nh.advertise< geometry_msgs:: Pose>("position", 1000);
    ros::Rate rate(20);

    while(ros :: ok())
    {
        geometry_msgs::Pose msg;
        msg.position.x = 5;
        msg.position.y = 5;
        pubBot.publish(msg);
    }

}