#include <ros/ros.h>
#include <geometry_msgs/Pose.h>

int main(int argc, char **argv)
{
    ros::init(argc, argv, "publish_position");
    ros:: NodeHandle nh;
    ros:: Publisher pub = nh.advertise< geometry_msgs:: Pose>("position", 1000);
    ros:: Rate rate(2);
    geometry_msgs:: Pose msg;
    msg.position.x = 5;
    msg.position.y = 5;
    while(ros :: ok())
    {
        pub.publish(msg);
        rate.sleep();
    }

}