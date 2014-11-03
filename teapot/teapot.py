from OpenGL.GLUT import *
from OpenGL.GLU import *
from OpenGL.GL import *
import sys
from socket import socket, AF_INET, SOCK_DGRAM
import socket
import math

name = 'BeaglePotBlack'

port = 4774 # port of the data broadcast
buf = 1024 # maximum size of the data from the BBB
sock = False

imu_zero_points = [0.0, 0.0, 0,0, 0.0] # subtract these values from all of the next values (set when this program gets it's first batch of values)

"""
######
# THESE COME FROM UDP USING MAGIC AND CODE... hopefully
pitch = 0.0
roll = 0.0
yaw = 0.0
######
"""

quat = [0.0, 0.0, 0.0, 0.0]

def main():
    glutInit(sys.argv)
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH)
    glutInitWindowSize(400,400)
    glutCreateWindow(name)

    glClearColor(0.,0.,1.,1.)
    glShadeModel(GL_SMOOTH)
    glEnable(GL_CULL_FACE)
    glEnable(GL_DEPTH_TEST)
    glEnable(GL_LIGHTING)
    lightZeroPosition = [-20.,2.,-2.,1.]
    lightZeroColor = [1.8,1.0,0.8,1.0] #green tinged
    glLightfv(GL_LIGHT0, GL_POSITION, lightZeroPosition)
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightZeroColor)
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.1)
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.05)
    glEnable(GL_LIGHT0)
    glutDisplayFunc(display)
    glutIdleFunc(animate)
    glMatrixMode(GL_PROJECTION)
    gluPerspective(40.,1.,1.,40.)
    glMatrixMode(GL_MODELVIEW)
    gluLookAt(0,0,10,
              0,0,0,
              0,1,0)
    glPushMatrix()
    glutMainLoop()
    return

def display():

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT)
    glPushMatrix()
    color = [1.0,0.,0.,1.]
    glMaterialfv(GL_FRONT,GL_DIFFUSE,color)
    glRotatef(90,0,1,0) # Align to the screen axes
    glRotatef(-90,1,0,0)
    """
    ####
    global yaw, roll, pitch
    glRotatef(yaw,0,-1,0)
    glRotatef(pitch,0,0,-1)
    glRotatef(roll,-1,0,0)
    """
    ####
    global quat
    w = math.degrees(2.0*math.acos(quat[0]))
    glRotatef(w, quat[1], quat[2], quat[3])
    
    glRotatef(180,0,1,0) # Make it face spout forward
    glRotatef(90,1,0,0) # The teapot is upside down by default
    
    glutSolidTeapot(-2,20,-20)
    
    glPopMatrix()
    glutSwapBuffers()
    
    return

def animate():
    #gets the values that are being broadcast over udp.
    """global roll, pitch, yaw
    [yaw, roll, pitch] = [math.degrees(val) for val in get_data()[0:3]]"""
    global quat
    quat = get_data()[9:]
    glutPostRedisplay()

def make_sock(port):
    sock = socket.socket(AF_INET, SOCK_DGRAM)
    sock.bind(('', port)) # bind to all interfaces/addresses by default
    return sock

#returns an array of floats or 0 if fail
#it should return the values from udp...
def get_data():
    global sock#, imu_zero_points
    if not sock:
        sock = make_sock(port)
    packet = sock.recv(buf)
    exploded = [float(val) for val in packet.split(',')]
    print exploded
    """if not imu_zero_points[0]:
        imu_zero_point = exploded
    else:
        for value in exploded:
            for zero_point in imu_zero_points:
                value -= zero_point
    """
    return exploded

if __name__ == '__main__': main()
