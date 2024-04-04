CFLAGS := -Wall -Wextra -Werror -O3
CFLAGS_DEBUG := -Wall -Wextra -Werror -g -O0
CFLAGS_CHECK := -Wall -Wextra -Werror -fsanitize=address
INCLUDE_FLAGS := -I/usr/include/opencv4
LD_FLAGS := -lopencv_core -lopencv_highgui -lopencv_imgcodecs

test_check: main.c camera_v4l2.h
	g++ -o $@ main.c $(CFLAGS_CHECK) $(INCLUDE_FLAGS) $(LD_FLAGS)
test_debug: main.c camera_v4l2.h
	gcc -o $@ main.c $(CFLAGS_DEBUG)
test: main.c camera_v4l2.h
	gcc -o $@ main.c $(CFLAGS)
