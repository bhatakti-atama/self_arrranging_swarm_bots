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
    float angualarSpeed = 2.5;
    float linearSpeed = 1.0;
    float angularScale = 3.5;
    float linearScale = 1.5;
    float kp = 4;
    float kd = 0.5;
    float angularTolerance = 0.1;
    float linearTolerance = 0.15;

    ros::init(argc, argv, "driver");
    ros:: NodeHandle nh;
    poseMessageClass poseMessage;
    odomMessageClass odomMessage;
    ros:: Subscriber subPose = nh.subscribe("position",1000 , &poseMessageClass::poseMessageCallback  , &poseMessage);
    ros:: Subscriber subOdom = nh.subscribe("odom",1000,&odomMessageClass::odomMessageCallback, &odomMessage);
    ros:: Publisher pub = nh.advertise< geometry_msgs:: Twist>("cmd_vel", 1000);
    ros:: Rate rate(2);
    geometry_msgs:: Twist msg;
    
    float vectorX ;
    float vectorY ;
    float z ;
    double theta;
    double distance;
    float thetaOld;
    while(ros :: ok())
    {
        ros:: spinOnce();
        vectorX = (poseMessage.pose.position.x - odomMessage.odometry.pose.pose.position.x);
        vectorY = (poseMessage.pose.position.y - odomMessage.odometry.pose.pose.position.y);
        z = atan2(vectorY, vectorX);
        distance = sqrt(pow(vectorX, 2) + pow(vectorY, 2));
        thetaOld = theta;
        theta = z - odomMessage.odometry.pose.pose.orientation.z;

        if (distance > linearTolerance )
        {
            if ( std::abs(theta) > angularTolerance)
            {
                msg.linear.x = 0;
                msg.angular.z =  angualarSpeed * (theta/std::abs(theta)) * std::min( 1.0, std::abs(theta)/(M_PI/angularScale) );
            }
            else
            {
                msg.linear.x = linearSpeed * std::min(1.0, distance/linearScale);
                msg.angular.z = ( theta / std::abs(theta)) * ( kp * (std::abs(theta) + kd * ( std::abs(theta) - std::abs(thetaOld))));
            }

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