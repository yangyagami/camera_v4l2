#include <stdio.h>

#include <opencv2/opencv.hpp>

#define CAMERA_V4L2_IMPLEMENTATION
#include "camera_v4l2.h"

int main() {
	camera_v4l2_camera_t *camera = camera_v4l2_create();

	camera_v4l2_param_t param;
	param.frame_width = 640;
	param.frame_height = 480;
	param.fmt = MJPEG;

	camera_v4l2_open(camera, 0, &param);

	if (camera_v4l2_isopened(camera) == 0) {
		printf(":(\n");
		std::terminate();
	}

	while (true) {
		camera_v4l2_buffer_t frame;

		if (camera_v4l2_isopened(camera) == 0) {
			printf("Camera disconnected!\n");
			sleep(2);
			camera_v4l2_open(camera, 0, &param);
			continue;
		}

		[[maybe_unused]]
		int ret = camera_v4l2_read(camera, &frame);
		if (ret) {
			cv::Mat f = cv::imdecode(cv::Mat(1, frame.length, CV_8UC1, frame.start), cv::IMREAD_COLOR);
			cv::imshow("frame", f);
		}

		if (cv::waitKey(5) >= 0) {
			break;
		}
	}

	// TODO: should destroy window. Never mind :).

	camera_v4l2_destroy(camera);
	return 0;
}
