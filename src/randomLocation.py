#!/usr/bin/env python
import rospy
import random as rand
from swarm_control.msg import Pattern
from std_msgs.msg import String

rospy.init_node("random_location")
rate = rospy.Rate(20)

string_publish = rospy.Publisher('keys',String,queue_size=1)
pattern_publish = rospy.Publisher('pattern',Pattern, queue_size=1)

positionList = []
for i in range(12):
    positionList.append(rand.randint(-5,5))

str_msg = String()
str_msg.data = 'r'
pattern_msg = Pattern()
for i in range(6):
    pattern_msg.pattern[i].x = float(positionList[i*2])
    pattern_msg.pattern[i].y = float(positionList[i*2 + 1])
    print(i)

while not rospy.is_shutdown():
    string_publish.publish(str_msg)
    pattern_publish.publish(pattern_msg)
    print(pattern_msg)