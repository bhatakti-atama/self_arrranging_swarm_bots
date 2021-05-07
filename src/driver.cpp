#include <ros/ros.h>
#include <geometry_msgs/Pose.h>
#include <geometry_msgs/Twist.h>
#include <nav_msgs/Odometry.h>
#include <std_msgs/Float32.h>
#include <math.h>
#include <angles/angles.h>


// classes so than we can save and maunpilate the recieved data
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

// this function calculates the angular distance between the vector connecting the bot and it's target position 
// and vector replresenting the current orietation of the bot 
// this function has a bug that it doesn't work properly for some particular angle but otherwise works fine
float calcTheta(float target, float start)
{
    float straight = target - start;
    float reverse;
    if(target > start)
    {
        reverse = (M_PI - target) + abs(start - M_PI); 
    }
    else
    {
        reverse = (M_PI - start) + abs(target - M_PI);
        reverse = -reverse; 
    }
    if(abs(straight) < abs(reverse))
    {
        return straight;
    }
    else
    {
        return reverse;
    }
}

int main(int argc, char **argv)
{
    // next block of code has all the parameters we can tune to get the desired performance 
    float angualarSpeed = 2.5;
    float linearSpeed = 1.0;
    float angularScale = 3.5;
    float linearScale = 1;
    float kp = 4;
    float kd = 0.5;
    float angularTolerance = 0.2;
    float linearTolerance = 0.05;

    ros::init(argc, argv, "driver");
    ros:: NodeHandle nh;
    poseMessageClass poseMessage;
    odomMessageClass odomMessage;
    ros:: Subscriber subPose = nh.subscribe("position",1000 , &poseMessageClass::poseMessageCallback  , &poseMessage);
    ros:: Subscriber subOdom = nh.subscribe("odomTrans",1000,&odomMessageClass::odomMessageCallback, &odomMessage);
    ros:: Publisher pub = nh.advertise< geometry_msgs:: Twist>("cmd_vel", 1000);
    ros:: Publisher pubz = nh.advertise< std_msgs::Float32>("z", 1000);
    ros:: Rate rate(20);
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
        // this block calculates the angle of vector connecting the bot and target and 
        // the angular distance between the current orientation of bot and that vector
        vectorX = (poseMessage.pose.position.x - odomMessage.odometry.pose.pose.position.x);
        vectorY = (poseMessage.pose.position.y - odomMessage.odometry.pose.pose.position.y);
        z = atan2(vectorY, vectorX);
        distance = sqrt(pow(vectorX, 2) + pow(vectorY, 2));
        thetaOld = theta;
        theta = angles::shortest_angular_distance(odomMessage.odometry.pose.pose.orientation.z,z);
        //theta = calcTheta(z,odomMessage.odometry.pose.pose.orientation.z);
        

        // first if statemnet cecks if the bot is at the target or not if bot is at the target then it we stop giving box any velocity
        if (distance > linearTolerance )
        {
            // this if statemnt checks if the orientation of the bot is aligned with the vector connecting the bot and target
            // wir some margin for error
            if ( std::abs(theta) > angularTolerance)
            {
                msg.linear.x = 0;
                // we give the bot angular velocity as a function of the angular distance between the vectors
                msg.angular.z =  angualarSpeed * (theta/std::abs(theta)) * std::min( 1.0, std::abs(theta)/(M_PI/angularScale) );
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