#include <ros/ros.h>
#include <geometry_msgs/Pose.h>
#include <geometry_msgs/Twist.h>
#include <nav_msgs/Odometry.h>
#include <std_msgs/Float32.h>
#include <math.h>

// classes so than we can save and maunpilate the recieved data
class poseMessageClass
{
public:
    geometry_msgs ::Pose pose;
    void poseMessageCallback(const geometry_msgs::Pose &msg);
};

class odomMessageClass
{
public:
    nav_msgs ::Odometry odometry;
    void odomMessageCallback(const nav_msgs::Odometry &msg);
};

void poseMessageClass ::poseMessageCallback(const geometry_msgs::Pose &msg)
{
    pose = msg;
}

void odomMessageClass ::odomMessageCallback(const nav_msgs::Odometry &msg)
{
    odometry = msg;
}


int main(int argc, char **argv)
{
    float linearTolerance = 0.05;
    float l = 0.5;
    float speedscale = 0.6;
    ros::init(argc, argv, "driver");
    ros::NodeHandle nh;
    poseMessageClass poseMessage;
    odomMessageClass odomMessage;
    ros::Subscriber subPose = nh.subscribe("position", 1000, &poseMessageClass::poseMessageCallback, &poseMessage);
    ros::Subscriber subOdom = nh.subscribe("odomTrans", 1000, &odomMessageClass::odomMessageCallback, &odomMessage);
    ros::Publisher pub = nh.advertise<geometry_msgs::Twist>("cmd_vel", 1000);
    ros::Publisher pubz = nh.advertise<std_msgs::Float32>("z", 1000);
    ros::Rate rate(20);
    geometry_msgs::Twist msg;

    float vectorX;
    float vectorY;
    double distance;
    while (ros ::ok())
    {
        ros::spinOnce();
        vectorX = (poseMessage.pose.position.x - odomMessage.odometry.pose.pose.position.x);
        vectorY = (poseMessage.pose.position.y - odomMessage.odometry.pose.pose.position.y);
        distance = sqrt(pow(vectorX, 2) + pow(vectorY, 2));
        float orientation = odomMessage.odometry.pose.pose.orientation.z;
        if (distance > linearTolerance)
        {
            msg.linear.x = speedscale * (vectorX * cos(orientation) + vectorY * sin(orientation)) / (distance);
            msg.angular.z = speedscale * (vectorY * cos(orientation) - vectorX * sin(orientation)) / (l * distance);
           
        }
        else
        {
            msg.linear.x = 0;
            msg.angular.y = 0;
        }
        pub.publish(msg);
        rate.sleep();
    }
}