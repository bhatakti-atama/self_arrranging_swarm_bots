#include <ros/ros.h>
#include <geometry_msgs/Pose.h>
#include <geometry_msgs/Twist.h>
#include <nav_msgs/Odometry.h>
#include <std_msgs/Float32.h>
#include <math.h>
#include <angles/angles.h>
using namespace std;
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
    float linearTolerance = 0.022;
    float l = 0.5;
    float speedscale = 0.6;
    float angualarSpeed = 2.5;
    float linearSpeed = 1.0;
    float angularScale = 0.5;
    float linearScale = 1;
    float kp = 4;
    float kd = 0.5;
    float angularTolerance = 0.2;
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
    float vectorX ;
    float vectorY ;
    float z ;
    double theta;
    double distance;
    float thetaOld;
    float vectorXTrue;
    float vectorYTrue;
    float disX;
    float disY;
    float distanceTrue;
    bool choice = true;
    int direction;
    while (ros ::ok())
    {
        ros::spinOnce();
        vectorXTrue = (poseMessage.pose.position.x - odomMessage.odometry.pose.pose.position.x);
        vectorYTrue = (poseMessage.pose.position.y - odomMessage.odometry.pose.pose.position.y);



        if(disX == NULL || disY == NULL )
        {
            disX = vectorXTrue;
            disY = vectorYTrue;
        }
        if((abs(disX - vectorXTrue) > 1) || (abs(disY - vectorYTrue) > 1) )
        {
            choice = true;
            disX = vectorXTrue;
            disY = vectorYTrue;
        }
        if(choice)
        {
            direction = rand() % 2;
            choice = false;
        }
        if(abs(vectorXTrue) < linearTolerance  || abs(vectorYTrue) < linearTolerance)
        {
            if(abs(vectorXTrue) > linearTolerance )
            {
                vectorX = vectorXTrue;
                vectorX = (vectorX/abs(vectorX)) * min(float(1),abs(vectorX));
                vectorY = 0 ;
            }
            if(abs(vectorYTrue) > linearTolerance)
            {
                vectorX = 0;
                vectorY = vectorYTrue;
                vectorY = (vectorY/abs(vectorY)) * min(float(1),abs(vectorY));
            }
        }
        else
        {
            if(direction)
            {
                vectorX = vectorXTrue;
                vectorX = (vectorX/abs(vectorX)) * min(float(1),abs(vectorX));
                vectorY = 0 ;
            }
            else
            {
                vectorX = 0;
                vectorY = vectorYTrue;
                vectorY = (vectorY/abs(vectorY)) * min(float(1),abs(vectorY));
            }

        }

        z = atan2(vectorY, vectorX);
        distance = sqrt(pow(vectorX, 2) + pow(vectorY, 2));
        distanceTrue = sqrt(pow(vectorXTrue, 2) + pow(vectorYTrue, 2));
        thetaOld = theta;
        theta = angles::shortest_angular_distance(odomMessage.odometry.pose.pose.orientation.z,z);
        //theta = calcTheta(z,odomMessage.odometry.pose.pose.orientation.z);


        // first if statemnet cecks if the bot is at the target or not if bot is at the target then it we stop giving box any velocity
        if (distanceTrue > linearTolerance )
        {
            // this if statemnt checks if the orientation of the bot is aligned with the vector connecting the bot and target
            // wir some margin for error
            if ( std::abs(theta) > angularTolerance)
            {
                msg.linear.x = 0;
                // we give the bot angular velocity as a function of the angular distance between the vectors
                msg.angular.z =  angualarSpeed * (theta/std::abs(theta)) * std::min( 1.0, std::abs(theta)/(angularScale) );
            }
            // if bot is aligned then we drive the bot forward
            else
            {
                // we give bot linear velocity as a function of linear distance i.e amount of linear velocity we give becomes less as we go near the target
                msg.linear.x = linearSpeed * std::min(1.0, distance/linearScale);
                // this line corrects the orientation of the bot to align it with the vector connceting bot and the target. I have applied PID here and
                //but this still needs improvement
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