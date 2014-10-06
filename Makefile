CC=gcc
CFLAGS= -Wall -g
LIBS= -lm
SOURCES=drop_bone_imu.c inv_mpu.c inv_mpu_dmp_motion_driver.c udp.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=dropboneimu

all: $(EXECUTABLE)
	@echo Team 4774\'s IMU - dropboneimu successfully built!
	
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf *o dropboneimu
