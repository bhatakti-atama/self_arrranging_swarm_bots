requirements:
    pygame

This Document gives a brief overview of the package:
    1. launch folder as the name suggests contains all the launch files of the project
    description of the files:
        1. guiTest.launch - test for the pygame gui 
        2. robot.launch - sub-part of swarm.launch
        3. swarm.launch - this is the main launch file which launches the complete system.
        4. trial1.launch - launch file for a single bot
    
    2. msg folder contains the custom messages we defined for this project
    The custom message is used for sending information on which pattern the
    bots will be arranged.
        1. Pattern.msg defines an array of size 6 of Points
        2. Point.msg is th defination of a single Point i.e x and y
    
    3. src folder contains all our nodes:
        1. convert.cpp - this nodes converts data published in quantenerion angle system 
        to euler angle system and publishes the converted data under "odomTrans"
        2. driver.cpp - this node drives the bot to the targeted point
        3. gui2.py - node for the pygame gui
        4. key_publisher.py - node for keyboard imputs 
        5. position_publisher - this node is used for testing a single bot. this node publishes information
        on which position a particular bots need to go
        6. position _publisher_node - this node takes information about the pattern the bots should be and then assigns 
        a particular point to a bot based on distance ( i.e it selects a bot that is least distant from the point).
        this node controlls 6 bots but can be modified to controll more or less bots
    
    if some file was not mentioned then the file is not relevant but if you gent any doubt please feel free to ask
    
    
