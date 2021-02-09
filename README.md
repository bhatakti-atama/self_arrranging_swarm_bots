# Self Arranging Swarm Robots 

Made as a first project in swarm robotics mainly for learning purposes. Turtlebots arrange themselves in the pattern inputted by the user 


![Swarm GIF](https://media.giphy.com/media/fcYchN7yX8PHTkw0ne/giphy.gif)


## Requirements:

### Libraries :
- [`ROS Melodic`](http://wiki.ros.org/melodic/Installation/Ubuntu)
- [`OpenCv 3.2`](https://opencv.org/opencv-3-2/)
- [`PyGame`](https://pypi.org/project/pygame/)

### ROS Packages :
- [`turtlebot3`](https://github.com/ROBOTIS-GIT/turtlebot3)
- [`turtlebot3 Messages`](https://github.com/ROBOTIS-GIT/turtlebot3_msgs)
- [`turtlebot3_simulations`](https://github.com/ROBOTIS-GIT/turtlebot3_simulations)



## Brief overview of the package: 

### launch :
- guiTest.launch - test for the pygame gui
- robot.launch - sub-part of swarm.launch
- swarm.launch - this is the main launch file which launches the complete system.
- trial1.launch - launch file for a single bot
- swarmTest.launch - same as swarm.launch but has camera and lighting 
    
### msg :
The custom message is used for sending information on which pattern the bots will be arranged :
- Pattern.msg defines an array of size 6 of Points
- Point.msg is th defination of a single Point i.e x and y
    
### src :
-convert.cpp - this nodes converts data published in quantenerion angle system to euler angle system and publishes the converted data under "odomTrans"
- driver.cpp - Now redundant and has been replaced with driverNew this node drives the bot to the targeted point
- driverNew.cpp - Does the same job as driver.cpp but useses a better method to modwl the diffrential drive bot
- gui2.py - node for the pygame gui
- key_publisher.py - node for keyboard imputs 
- position_publisher - this node is used for testing a single bot. this node publishes information on which position a particular bots need to go
- position _publisher_node - this node takes information about the pattern the bots should be and then assigns a particular point to a bot based on distance ( i.e it selects a bot that is least distant from the point). this node controlls 6 bots but can be modified to controll more or less bots
- openTry.py -This node is under development will be used for pose detection currently detects position but not orientation  

### Worlds :
- contains empty world with camera and light

### if some file was not mentioned then the file is not relevant but if you gent any doubt please feel free to ask
 
