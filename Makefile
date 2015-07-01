CC=g++
CFLAGS = `pkg-config --cflags opencv`
LIBS = `pkg-config --libs opencv`


executable: main.cpp shotdetector.cpp
	$(CC) main.cpp shotdetector.cpp -o ShotDetection $(LIBS) $(CFLAGS)
