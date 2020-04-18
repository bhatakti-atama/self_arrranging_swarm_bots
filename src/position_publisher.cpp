#include <ros/ros.h>
#include <geometry_msgs/Pose.h>
#include <nav_msgs/Odometry.h>
#include <math.h>
#include <std_msgs/String.h>
class odomMessageClass
{
    public: 
        nav_msgs :: Odometry odometry;
        void odomMessageCallback( const nav_msgs::Odometry& msg );
};

void odomMessageClass :: odomMessageCallback( const nav_msgs::Odometry& msg )
{
    odometry = msg ;
}

class stringMessageClass
{
    public:
        std_msgs :: String st;
        void stringMessageCallback( const std_msgs :: String& msg);
    stringMessageClass()
    {
        st.data[0] = 'a';
    }
};

void stringMessageClass :: stringMessageCallback( const std_msgs::String& msg)
{
    st = msg;
}


void calc(float (&list)[6][2],odomMessageClass (&position)[6], ros::Publisher (&pubBot)[6])
{
    float copy[6][2];
    for(int i=0; i < 6; i++)
    {
        copy[i][0] = list[i][0];
        copy[i][1] = list[i][1];
    }


    bool bots[6];
    std::fill_n(bots,6,true);
    for(int i =0; i < 6; i++)
    {
        int minIndex = 0;
        float min = 10000; // any outlandish value as a start
        for(int j =0; j < 6; j++)
        {
            if(bots[j])
            {
                double distance = sqrt (pow ( double( position[j].odometry.pose.pose.position.x - copy[i][0]),2) + pow(double(position[j].odometry.pose.pose.position.y - copy[i][1]),2)) ;
                if( min > distance )
                {
                    minIndex = j;
                    min = distance;
                }
            }
        }
        geometry_msgs::Pose msg;
        msg.position.x = copy[i][0];
        msg.position.x = copy[i][1];
        pubBot[minIndex].publish(msg);
        bots[minIndex] = false;
    }
}



int main(int argc, char **argv)
{
    ros::init(argc, argv, "publish_position");
    ros:: NodeHandle nh;
    odomMessageClass odomMessage[6];
    stringMessageClass stringMessage;
    ros::Subscriber subBot[6];
    ros::Publisher pubBot[6];
    ros::Subscriber subK = nh.subscribe("keys",1000, &stringMessageClass:: stringMessageCallback, &stringMessage);
    float a [6][2] ={   {4,2},  {4,0},
                        {4,-2}, {-4,-2},
                        {-4,0}, {-4,2}
                    };
    float b [6][2] ={   {0,5},  {0,0},
                        {0,-5}, {5,0},
                        {-5,0}, {7,7}
                    };


    for(int i=0; i<6; i++)
    {
        std::ostringstream os;
        os <<"bot"<<i<<"/odom";
        std::string s = os.str();
        subBot[i] = nh.subscribe(s,1000, &odomMessageClass::odomMessageCallback,  &odomMessage[i]);
        os <<"bot"<<i<<"/position";
        s = os.str();
        pubBot[i] = nh.advertise< geometry_msgs:: Pose>(s, 1000);
    }
    while(ros :: ok())
    {
        calc(b,odomMessage,pubBot);
        /*
        pub.publish(msg);
        rate.sleep();
        */
    }

}