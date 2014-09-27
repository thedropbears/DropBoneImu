CC=gcc
CFLAGS=-c -Wall
LDFLAGS= -lm
SOURCES=drop_bone_imu.c inv_mpu.c inv_mpu_dmp_motion_driver.c udp.c udp.h
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=dropboneimu

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *o dropboneimu
