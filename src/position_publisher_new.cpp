#include <ros/ros.h>
#include <geometry_msgs/Pose.h>
#include <nav_msgs/Odometry.h>
#include <math.h>
#include <std_msgs/String.h>
#include <swarm_control/Pattern.h>

// classes so that we can save and manulpilate the data recieved
class odomMessageClass
{

public:
    nav_msgs::Odometry odometry;
    void odomCallback(const nav_msgs::Odometry &msg);

};
void odomMessageClass:: odomCallback(const nav_msgs::Odometry &msg)
{   
    odometry = msg;
}
class patternMessageClass
{
    public:
        swarm_control::Pattern pt;
        void patternCallback(const swarm_control::Pattern &msg);
};
void patternMessageClass:: patternCallback(const swarm_control::Pattern &msg)
{
    pt = msg;
} 

class stringMessageClass
{
    public:
    std_msgs::String st;
    void stringCallback(const std_msgs::String msg);
    // constructor to give intial value to the variable
    stringMessageClass()
    {
        st.data = "q";
    }
};

void stringMessageClass:: stringCallback( const std_msgs::String msg)
{
    st = msg;
}

// this function assigns the target points to the bots based on distance
// i.e the bot with least distance to the given point is assigned that point as 
// target
// needs improvement as it does not give the best solution  

void publish(double position[6][2], ros::Publisher (&pubBot)[6], odomMessageClass (&odomMessage)[6])
{

    geometry_msgs:: Pose msg;
    bool bot[6];
    for(int i =0; i < 6; i++)
        bot[i] = true;
    for(int i=0; i<6 ; i++)
    {
        float min = 1000; // arbitary large value just for intialization
        int minIndex = 0; // arbritary value
        for(int j=0; j<6; j++)
        {
            if(bot[j])
            {
                float distance = sqrt( pow((odomMessage[j].odometry.pose.pose.position.x - position[i][0]), 2) + pow((odomMessage[j].odometry.pose.pose.position.y - position[i][1]), 2));
                if (distance < min)
                {
                    min = distance;
                    minIndex = j;
                }
            }
        }
        bot[minIndex] = false;
        msg.position.x = position[i][0];
        msg.position.y = position[i][1];
        pubBot[minIndex].publish(msg);
    }

}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "position_publisher_new");
    ros:: NodeHandle nh;
    ros::Publisher pubBot[6];
    ros::Subscriber subBot[6];
    stringMessageClass stringMessage;
    ros::Subscriber subK = nh.subscribe("keys", 1000, &stringMessageClass::stringCallback, &stringMessage);
    odomMessageClass odomMessage[6];
    patternMessageClass patternMessage;
    ros::Subscriber subP = nh.subscribe("pattern",1000,&patternMessageClass::patternCallback, &patternMessage);
    ros::Rate rate(20);

    // some hardcoded shapes you can add your own shapes in similar format
    double hexagon[6][2] =   { {0,6},     {5.2,3},
                               {5.2,-3},  {0,-6},
                               {-5.2,-3}, {-5.2,3}
                             };
    double parallel[6][2] =   { {0,5},     {5,5},
                               {5,-5},    {0,-5},
                               {-5,-5},   {-5,5}
                             };
    double rocket[6][2] =   { {0,6},     {0,4},
                               {0,2},    {0,0},
                               {-2,-2},   {2,-2}
                             };
    double a[6][2];
    for(int i=0; i<6; i++)
    {
        std::ostringstream os;
        os <<"bot"<<i<<"/position";
        std::string s = os.str();
        pubBot[i] = nh.advertise< geometry_msgs::Pose >(s,1000);
        std::ostringstream os1;
        os1 <<"bot"<<i<<"/odom";
        std::string s1 = os1.str();
        subBot[i] = nh.subscribe(s1,1000, &odomMessageClass::odomCallback, &odomMessage[i]);
    }
    while (ros::ok())
    {
        ros::spinOnce();
        if(stringMessage.st.data == "q")
        {
            publish(hexagon,pubBot,odomMessage);
        }
        if(stringMessage.st.data == "w")
        {
            publish(parallel,pubBot,odomMessage);
        }
        if(stringMessage.st.data == "e")
        {
            publish(rocket,pubBot,odomMessage);
        }
        if(stringMessage.st.data == "r")
        {
            for( int i =0; i<6; i++)
            {
                a[i][0] = patternMessage.pt.pattern[i].x;
                a[i][1] = patternMessage.pt.pattern[i].y;
            }
            publish(a,pubBot,odomMessage);
        }
        rate.sleep();
    }
}