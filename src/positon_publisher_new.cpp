#include <ros/ros.h>
#include <geometry_msgs/Pose.h>




void publish(double position[6][2], ros::Publisher (&pubBot)[6])
{
    geometry_msgs:: Pose msg;
    for (int i=0; i < 6; i++)
    {
        msg.position.x = position[i][0];
        msg.position.y = position[i][1];
        pubBot[i].publish(msg);
    }
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "position_publisher_new");
    ros:: NodeHandle nh;
    ros::Publisher pubBot[6];
    double hexagon[6][2] =   { {0,8},   {5,5},
                              {0,-8},  {5,-5},
                              {-5,-5}, {-5,-5}
                            };
    for(int i=0; i<6; i++)
    {
        std::ostringstream os;
        os <<"bot"<<i<<"/position";
        std::string s = os.str();
        pubBot[i] = nh.advertise< geometry_msgs::Pose >(s,1000);
    }
}