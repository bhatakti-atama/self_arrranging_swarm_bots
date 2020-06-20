#!/usr/bin/env python
import rospy
import pygame 
from swarm_control.msg import Pattern
from std_msgs.msg import String

rospy.init_node("gui")
rate = rospy.Rate(20)
pygame.font.init()
(width, height) = (500,500)
screen = pygame.display.set_mode((width, height))
pygame.display.set_caption("swarm_input")
pointList = []
running = True
statusSend = False
statusPublish = False
counter = 0

# this fucntion draws the point we clicked on the GUI
def displayList():
    for i in pointList:
        pygame.draw.circle(screen, (0,0,255), i, 15 , 0)

while running:
    for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
            if event.type == pygame.MOUSEBUTTONDOWN:
                ( mouseX, mouseY) = pygame.mouse.get_pos()
                if mouseX > 0 and mouseX < 100 and mouseY > 350 and mouseY < 400:
                    if statusSend:
                        statusPublish = True
                elif mouseX > 0 and mouseX < 100 and mouseY > 450 and mouseY < 500:
                    pointList = []
                    counter = 0
                    statusSend = False
                else:
                    counter += 1
                    if counter <=6:
                        pointList.append((mouseX, mouseY))
                    if counter >=6:
                        statusSend = True
    if statusPublish:
        if statusSend:
            pointListSend = pointList
        pattern_publish = rospy.Publisher('pattern',Pattern, queue_size=1)
        string_publish = rospy.Publisher('keys',String,queue_size=1)
        msg = Pattern()
        cnt = 0
        for i in pointListSend:
            msg.pattern[cnt].x = 2*((i[0] - 250)/100.0) #float((i[0] - 500)/100)
            msg.pattern[cnt].y = 2*((250 - i[1])/100.0) #float((i[1] - 500)/100)
            cnt += 1
        pattern_publish.publish(msg)
        msg = String()
        msg.data = 'r'
        string_publish.publish(msg)
        rate.sleep()
    screen = pygame.display.set_mode((width, height))
    if statusSend:
        pygame.draw.rect(screen,(0,255,0),(0,350,100,50))
    else:
        pygame.draw.rect(screen,(255,0,0),(0,350,100,50))
    
    pygame.draw.rect(screen,(0,255,0),(0,450,100,50))

    font = pygame.font.Font('freesansbold.ttf', 25) 
    send = font.render('Send', True, (255,255,255))
    reset = font.render('Reset', True, (255,255,255))
    sendRect = send.get_rect()
    resetRect = reset.get_rect()
    sendRect = (10,360)
    resetRect = (10,460) 
    screen.blit(send, sendRect)
    screen.blit(reset, resetRect)
    displayList()
    pygame.display.flip()
    


    