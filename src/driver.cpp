#include <ros/ros.h>
#include <geometry_msgs/Pose.h>
#include <geometry_msgs/Twist.h>
#include <nav_msgs/Odometry.h>
#include <math.h>

class poseMessageClass
{
    public: 
        geometry_msgs :: Pose pose;
        void poseMessageCallback( const geometry_msgs::Pose& msg );
};

class odomMessageClass
{
    public: 
        nav_msgs :: Odometry odometry;
        void odomMessageCallback( const nav_msgs::Odometry& msg );
};

void  poseMessageClass :: poseMessageCallback(const geometry_msgs::Pose& msg)
{
    pose = msg ;
}

void odomMessageClass :: odomMessageCallback( const nav_msgs::Odometry& msg )
{
    odometry = msg ;
}


int main(int argc, char **argv)
{
    ros::init(argc, argv, "driver");
    ros:: NodeHandle nh;
    poseMessageClass poseMessage;
    odomMessageClass odomMessage;
    ros:: Subscriber subPose = nh.subscribe("position",1000 , &poseMessageClass::poseMessageCallback  , &poseMessage);
    ros:: Subscriber subOdom = nh.subscribe("odom",1000,&odomMessageClass::odomMessageCallback, &odomMessage);
    ros:: Publisher pub = nh.advertise< geometry_msgs:: Twist>("cmd_vel", 1000);
    ros:: Rate rate(2);
    geometry_msgs:: Twist msg;
    while(ros :: ok())
    {
        ros:: spinOnce();
        float vectorX = (poseMessage.pose.position.x - odomMessage.odometry.pose.pose.position.x);
        float vectorY = (poseMessage.pose.position.y - odomMessage.odometry.pose.pose.position.y);
        float z = atan2(vectorY, vectorX);
        float distance = sqrt(pow(vectorX, 2) + pow(vectorY, 2));
        ROS_INFO_STREAM(odomMessage.odometry.pose.pose.position.x << " " << odomMessage.odometry.pose.pose.position.y);
        if ( distance < 0.3)
        {
            msg.linear.x = 0;
            msg.angular.z = 0;
        }
        else
        {
            float theta = z - odomMessage.odometry.pose.pose.orientation.z;
            msg.linear.x = cos(theta);
            msg.angular.z = sin(theta);
        }
        pub.publish(msg);
        rate.sleep();
    }
}