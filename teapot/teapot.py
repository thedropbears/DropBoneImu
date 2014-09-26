from OpenGL.GLUT import *
from OpenGL.GLU import *
from OpenGL.GL import *
import sys

name = 'OpenGL Python Teapot'

######
# THESE NEED TO COME FROM UDP (somehow!!)
pitch = 0.0
roll = 0.0
yaw = 0.0
######

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
    glRotatef(180,1,0,0) # The teapot is upside down by default
    glRotatef(90,0,1,0) # Make it face spout forward
    ####
    # These next three might not be in the correct order - test it out! (Or look it up!)
    global roll, pitch, yaw
    glRotatef(roll,1,0,0)
    glRotatef(yaw,0,1,0)
    glRotatef(pitch,0,0,1)
    ####
    glutSolidTeapot(-2,20,-20)
    
    glPopMatrix()
    glutSwapBuffers()
    
    return

def animate():
    ####
    # This is the function that should be listening for the UDP messages and updating pitch, roll, yaw
    # For the time being, just increment so we can see it animating
    global roll, pitch, yaw
    roll += 1
    pitch += 2
    yaw += 3

    glutPostRedisplay()


if __name__ == '__main__': main()
