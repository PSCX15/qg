# -*- coding: utf-8 -*-


import matplotlib.pyplot as plt
import matplotlib.axes as mpax
import numpy as np


import rospy
from nav_msgs.msg import Odometry
from geometry_msgs.msg import Point, Quaternion



def callback(data):
    rospy.loginfo("I heard %s", data.data)

def tracer_points(nom_fichier):
    
    rospy.init_node('node_name', anonymous = True)

    rospy.Subscriber("/odometry/filtered_map", Point, callback)
 
    # spin() simply keeps python from exiting until this node is stopped
    rospy.spin()
    
    
    file = open(nom_fichier + ".txt","w")
    
    x = []
    y = []
    
    while not rospy.is_shutdown():
        file.write(str(geometry_msg/Point.position.x) + " " + str(geometry_msg/Point.position.y) + "\n")
        x.append(geometry_msg/Point.position.x)
        y.append(geometry_msg/Point.position.y)
    
    
    file.close()

    plt.scatter(x,y)

    plt.title('Nuage de points représentant la position du robot au cours du temps')
    plt.xlabel('Est')
    plt.ylabel('Nord')
    plt.savefig(nom_fichier + '.png')
    plt.show() 
    
    
    
def tracer_tout(liste_nom_fichier):

    couleurs = ['b', 'r', 'g', 'c', 'm', 'y', 'k', 'purple', 'brown', 'gray']
    for i in range (len(liste_nom_fichier)):    
    
        file = open(liste_nom_fichier[i],"r")
    
        X = []
        Y = []
        M = 0
        
        for line in file:
            x,y = "",""
            sec_col = False
            for k in range(len(line)):
                if line[k] == " ":
                    sec_col = True
                else:
                    if sec_col:
                        y = y + line[k]
                    else:
                        x = x + line[k]
                    
            X.append(float(x))
            Y.append(float(y))
            if np.sqrt(float(x)**2 + float(y)**2) > M:
                M = np.sqrt(float(x)**2 + float(y)**2)
            
        
        theta = np.linspace(0, 2*np.pi, 40)

        c = M*np.cos(theta)
        s = M*np.sin(theta)
        
        fig = plt.figure()
        ax = fig.add_subplot(1,1,1)
        ax.plot(c, s, c = couleurs[(k % len(couleurs))])
        plt.scatter(X,Y,c = couleurs[(k % len(couleurs))])
        ax.spines['left'].set_position('zero')          # Propriétés des axes
        ax.spines['right'].set_color('none')
        ax.spines['bottom'].set_position('zero')
        ax.spines['top'].set_color('none')
        plt.axis("equal")
        
        
    plt.title('Nuage de points représentant la position du robot au cours du temps')
    plt.xlabel('Est')
    plt.ylabel('Nord')
    plt.savefig('compilation_traces.png')
    #plt.show()
    
#liste = ["test1.txt","test2.txt"]
#tracer_tout(liste)
    
    

if(__name__ == '__main__'):
    tracer_points()
    

