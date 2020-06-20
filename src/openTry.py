#!/usr/bin/env python
import rospy
from sensor_msgs.msg import Image
import cv2,cv_bridge, numpy

class Follower:
    def __init__(self):
        self.bridge = cv_bridge.CvBridge()
        cv2.namedWindow("window",1)
        self.image_sub = rospy.Subscriber('/rrbot/camera1/image_raw', Image, self.image_callback)

    def image_callback(self, msg):
        image = self.bridge.imgmsg_to_cv2(msg, desired_encoding='bgr8')
        gray = cv2.cvtColor( image, cv2.COLOR_BGR2GRAY)
        lower_black = numpy.array([0])
        higher_black = numpy.array([150])
        mask = cv2.inRange(gray,lower_black,higher_black)
        masked = cv2.bitwise_and(gray, gray, mask=mask)
        img2, contours, hierchy = cv2.findContours(mask,cv2.RETR_EXTERNAL,cv2.CHAIN_APPROX_SIMPLE)
        cont_sorted = sorted(contours,key = cv2.contourArea, reverse = True)[:6]
        for x in cont_sorted:
            x,y,w,h = cv2.boundingRect(x)
            cv2.rectangle(image,(x,y),(x+w,y+h),(0,0,255),5)
        cv2.imshow("image",image)
        cv2.waitKey(3)

rospy.init_node('follower')
follower = Follower()
rospy.spin()