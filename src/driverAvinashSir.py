#!/usr/bin/python2
import rospy
from nav_msgs.msg import Odometry
from tf.transformations import euler_from_quaternion
from geometry_msgs.msg import PoseStamped, Twist
from math import atan2, cos, sin, pi
 
class poseMessageClass(object):
  def __init__(self):
    # super().__init__()
    self.pose=PoseStamped()
  def linear(self):
    posn = self.pose.pose.position
    return [posn.x, posn.y, posn.z]
  def euler(self):
    orn = self.pose.pose.orientation
    return euler_from_quaternion([orn.x, orn.y, orn.z, orn.w])
  def cb(self, msg):
    self.pose = msg
  def __str__(self):
    return 'l:'+str(self.linear())+',r:'+str(self.euler())
  def __repr__(self):
    return self.__str__()
 
class odomMessageClass(object):
  def __init__(self):
    # super().__init__()
    self.odom = Odometry()
  def linear(self):
    posn = self.odom.pose.pose.position
    return [posn.x, posn.y, posn.z]
  def euler(self):
    orn = self.odom.pose.pose.orientation
    return euler_from_quaternion([orn.x, orn.y, orn.z, orn.w])
  def cb(self, msg):
    self.odom = msg
  def __str__(self):
    return 'l:'+str(self.linear())+',r:'+str(self.euler())
  def __repr__(self):
    return self.__str__()
 
def normTheta(theta):
  if(theta >= -pi and theta<=pi ): return theta
  elif(theta < -pi): return normTheta(2*pi+theta)
  else: return normTheta(theta-2*pi)
 
def normSpd(linx, angz):
  MAX_LIN_SPD, MAX_ROT_SPD = 1,1
  MIN_LIN_SPD, MIN_ROT_SPD = .2,.2
  sign_linx = -1 if linx<0 else (1 if linx>0 else 0)
  sign_angz = -1 if angz<0 else (1 if angz>0 else 0)
  linx, angz = abs(linx), abs(angz)
  linx = min(MAX_LIN_SPD, linx)
  linx = max(MIN_LIN_SPD, linx)
  linx = sign_linx*linx
  angz = min(MAX_ROT_SPD, angz)
  angz = max(MIN_ROT_SPD, angz)
  angz = sign_angz*angz
  return linx, angz
 
 
def getSpdAvi(dist, theta):
  LIN_SCALE, ROT_SCALE = 1,1
  spd = Twist()
  if abs(theta)>0.01:
    linx,angz = normSpd(0, theta*ROT_SCALE)
  elif(dist<0.05):
    linx,angz = 0,0
  else:
    linx,angz = normSpd(dist*LIN_SCALE, theta*ROT_SCALE)
  print('spd',[linx,angz])
  spd.linear.x = linx
  spd.angular.z = angz
  return spd
 
def getSpdJai(dist, theta):
  spd = Twist()
  if dist<0.1:
    spd.linear.x = 0
    spd.angular.z = 0
  else:
    spd.linear.x = cos(theta)
    spd.angular.z = sin(theta)
  return spd
 
if __name__ == "__main__":
    rospy.init_node('driver2')
    poseMessage = poseMessageClass()
    odomMessage = odomMessageClass()
    subPose = rospy.Subscriber('/position', PoseStamped, poseMessage.cb)#, poseMessage)
    # subPose = rospy.Subscriber('/move_base_simple/goal', PoseStamped, poseMessage.cb)#, poseMessage)
    subPose = rospy.Subscriber('/odom', Odometry, odomMessage.cb)#, odomMessage)
    pubCmd = rospy.Publisher('/cmd_vel',Twist, queue_size=1)
    rate = rospy.Rate(50)
    spd = Twist()
    while not rospy.is_shutdown():
      [px,py,_] = poseMessage.linear()
      # [_,_,ptheta] = poseMessage.euler()
      [ox,oy,_] = odomMessage.linear()
      [_,_,otheta] = odomMessage.euler()
      dy,dx = py-oy, px-ox
      ptheta = atan2(dy, dx)
 
      dist = (dy**2 + dx**2)**0.5
      theta = normTheta(ptheta-otheta)
      print('pm:',poseMessage.__str__())
      print('om',odomMessage.__str__())
      print('dist',dist,'theta',theta)
      pubCmd.publish( getSpdAvi(dist, theta) )
      rate.sleep()