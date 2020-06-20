#include <ros/ros.h>
#include <nav_msgs/Odometry.h>
#include <math.h>

// these classes are used so that we can save and manuplate data recieved 
class odomMessageClass
{
    public:
    nav_msgs::Odometry odometry;
    void odomCallback(const nav_msgs::Odometry msg);
};

void odomMessageClass:: odomCallback( const nav_msgs::Odometry msg)
{
    odometry = msg;
}

// function converts the qunatenerion to euler although it is limited onyly to 
// 2d coordinates as it outputs only position x,y and angle z
void converter(odomMessageClass &odo, ros::Publisher &a)
{
    nav_msgs::Odometry msg;
    msg.pose.pose.position.x = odo.odometry.pose.pose.position.x;
    msg.pose.pose.position.y = odo.odometry.pose.pose.position.y;
    double sinY = 2*(odo.odometry.pose.pose.orientation.w * odo.odometry.pose.pose.orientation.z + odo.odometry.pose.pose.orientation.x * odo.odometry.pose.pose.orientation.y);
    double cosY = 1 - 2 * (pow( float(odo.odometry.pose.pose.orientation.y), 2) + pow(float(odo.odometry.pose.pose.orientation.z), 2));
    msg.pose.pose.orientation.z = atan2(sinY, cosY);
    a.publish(msg);
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "convert");
    ros::NodeHandle nh;
    odomMessageClass odomMessage;
    ros::Publisher pub = nh.advertise<nav_msgs::Odometry>("odomTrans", 1000);
    //we are passing function of a class as an callback and also passing an object of the same class
    ros::Subscriber sub = nh.subscribe("odom", 1000 , &odomMessageClass::odomCallback, &odomMessage);
    ros::Rate rate(20);
    while (ros::ok())
    {
        ros::spinOnce();
        converter(odomMessage, pub);
        rate.sleep();
    }
    
}