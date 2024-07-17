#ifndef CAMERA_V4L2_H_
#define CAMERA_V4L2_H_

#ifdef __cpluscplus
extern "C" {
#endif

struct camera_v4l2_buffer {
	void *start;
	size_t length;
};
typedef struct camera_v4l2_buffer camera_v4l2_buffer_t;

enum camera_v4l2_frame_format {
	MJPEG = 0,
	YUYV,
};
typedef enum camera_v4l2_frame_format camera_v4l2_frame_format_t;

struct camera_v4l2_param {
	int frame_width;
	int frame_height;
	camera_v4l2_frame_format_t fmt;
};
typedef struct camera_v4l2_param camera_v4l2_param_t;

struct camera_v4l2_camera;
typedef struct camera_v4l2_camera camera_v4l2_camera_t;

camera_v4l2_camera_t *camera_v4l2_create();
void camera_v4l2_destroy(camera_v4l2_camera_t *camera);

int camera_v4l2_open(camera_v4l2_camera_t *camera,
		     int index, camera_v4l2_param_t *param);
int camera_v4l2_isopened(camera_v4l2_camera_t *camera);
void camera_v4l2_close(camera_v4l2_camera_t *camera);
int camera_v4l2_read(camera_v4l2_camera_t *camera,
		     camera_v4l2_buffer_t *frame);

#ifdef __cpluscplus
}
#endif

#endif  // CAMERA_V4L2_H_

#ifdef CAMERA_V4L2_IMPLEMENTATION

#ifdef __cpluscplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <stdint.h>

#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>
#include <sys/mman.h>

#define	CAMERA_V4L2_BUFFER_COUNT (12)
#define CAMERA_V4L2_ASSERT(cond, msg) \
do { \
	if (!(cond)) { \
		fprintf(stderr, "\x1B[31m Assert failed: [%s][%d] %s \e[0m\n", __FUNCTION__, __LINE__, msg); \
		exit(1); \
	} \
} while(0)

#define CAMERA_V4L2_LOG_ERROR(msg, ...)	\
do { \
	fprintf(stderr, "\x1B[31mERROR: [%s][%d] " msg "\e[0m\n", __FUNCTION__, __LINE__, ##__VA_ARGS__); \
} while(0)

#define CAMERA_V4L2_LOG_INFO(msg, ...)	\
do { \
	fprintf(stdout, "INFO: [%s][%d] " msg "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__); \
} while(0)

#define CAMERA_V4L2_LOG_WARN(msg, ...)	\
do { \
	fprintf(stderr, "\x1B[32mWARN: [%s][%d] " msg "\e[0m\n", __FUNCTION__, __LINE__, ##__VA_ARGS__); \
} while(0)

struct camera_v4l2_camera {
	int streaming;
	int fd;
	camera_v4l2_buffer_t *buf;
};

static int camera_v4l2_io_control(camera_v4l2_camera_t *camera, int request,
				   void *output) {
	CAMERA_V4L2_ASSERT(camera != NULL, "Object is null!!!");

	if (camera->fd == -1) return 0;

ioctl_retry:
	int ret = ioctl(camera->fd, request, output);
	if (ret >= 0) return 1;
	if (errno != EINPROGRESS && errno != EAGAIN && errno != EBUSY) {
		if (errno == EBADF || errno == ENOENT ||
		    errno == ENODEV || errno == EPIPE) {
			camera->streaming = 0;  // If camera disconnected, the streaming should be 0.
			camera_v4l2_close(camera);
		}
		CAMERA_V4L2_LOG_ERROR("ioctl failed: %s", strerror(errno));
		return 0;
	} else {
		fd_set fdset;
		FD_ZERO(&fdset);
		FD_SET(camera->fd, &fdset);
		struct timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = 1000 * 50;
		int select_ret = select(camera->fd + 1, &fdset, NULL, NULL,
					&tv);
		if (select_ret > 0) {
			goto ioctl_retry;
		} else {
			CAMERA_V4L2_LOG_ERROR("select failed: %s", strerror(errno));
			return 0;
		}
	}

	return 1;
}

static void camera_v4l2_query_frame_interval(
	camera_v4l2_camera_t *camera,
	uint32_t pixelfmt,
	int width,
	int height) {
	CAMERA_V4L2_ASSERT(camera != NULL, "Object is null!!!");

	struct v4l2_frmivalenum frmival;
	frmival.index = 0;
	frmival.pixel_format = pixelfmt;
	frmival.width = width;
	frmival.height = height;

	while (camera_v4l2_io_control(camera, VIDIOC_ENUM_FRAMEINTERVALS,
				      &frmival)) {
		CAMERA_V4L2_LOG_INFO(
		       "%d/%d ",
		       frmival.discrete.numerator,
		       frmival.discrete.denominator
		);

		frmival.index++;
	}
	CAMERA_V4L2_LOG_INFO("\n");
}

static void camera_v4l2_query_frame_size(camera_v4l2_camera_t *camera,
					 uint32_t pixelfmt) {
	CAMERA_V4L2_ASSERT(camera != NULL, "Object is null!!!");

	struct v4l2_frmsizeenum frmsize;
	frmsize.pixel_format = pixelfmt;
	frmsize.index = 0;

	while (camera_v4l2_io_control(camera, VIDIOC_ENUM_FRAMESIZES,
				      &frmsize)) {
		CAMERA_V4L2_LOG_INFO(
		       "width: %d, height: %d, fps: ",
		       frmsize.discrete.width,
		       frmsize.discrete.height
		);

		camera_v4l2_query_frame_interval(camera, pixelfmt, frmsize.discrete.width, frmsize.discrete.height);

		frmsize.index++;
	}
	CAMERA_V4L2_LOG_INFO("\n");
}

static void camera_v4l2_query_fmt_desc(camera_v4l2_camera_t *camera) {
	CAMERA_V4L2_ASSERT(camera != NULL, "Object is null!!!");

	struct v4l2_fmtdesc fmtdesc;
	fmtdesc.index = 0;
	fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	while (camera_v4l2_io_control(camera, VIDIOC_ENUM_FMT, &fmtdesc)) {
		CAMERA_V4L2_LOG_INFO(
		       "Format: %c %c %c %c\n"
		       "Format description: %s\n",
        	       fmtdesc.pixelformat & 0xFF,
		       (fmtdesc.pixelformat >> 8) & 0xFF,
		       (fmtdesc.pixelformat >> 16) & 0xFF,
		       (fmtdesc.pixelformat >> 24) & 0xFF,
		       fmtdesc.description
		);

		camera_v4l2_query_frame_size(camera, fmtdesc.pixelformat);

		fmtdesc.index++;
	}

}

static int camera_v4l2_query_info(camera_v4l2_camera_t *camera) {
	CAMERA_V4L2_ASSERT(camera != NULL, "Object is null!!!");

	struct v4l2_capability capability;

	if (camera_v4l2_io_control(camera, VIDIOC_QUERYCAP, &capability) == 0) {
		CAMERA_V4L2_LOG_ERROR("Query capability failed\n");
		return 0;
	}

	CAMERA_V4L2_LOG_INFO(
	       "Driver name: %s\n"
	       "Device name: %s\n"
	       "Bus info: %s\n"
	       "Version: %d\n\n",
	       capability.driver,
	       capability.card,
	       capability.bus_info,
	       capability.version
	);

	camera_v4l2_query_fmt_desc(camera);

	return 1;
}

static int camera_v4l2_set_param(
	camera_v4l2_camera_t *camera,
	camera_v4l2_param_t *param) {
	CAMERA_V4L2_ASSERT(camera != NULL, "Object is null!!!");

	struct v4l2_format fmt;
    	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width = param->frame_width;
	fmt.fmt.pix.height = param->frame_height;
	switch (param->fmt) {
		case MJPEG: {
			fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
			break;
		}
		case YUYV: {
			fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
			break;
		}
	}

	return camera_v4l2_io_control(camera, VIDIOC_S_FMT, &fmt);
}

static int camera_v4l2_request_buffers(camera_v4l2_camera_t *camera) {
	CAMERA_V4L2_ASSERT(camera != NULL, "Object is null!!!");

	struct v4l2_requestbuffers reqbufs;
	reqbufs.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	reqbufs.memory = V4L2_MEMORY_MMAP;
	reqbufs.count = CAMERA_V4L2_BUFFER_COUNT;

	if (!camera_v4l2_io_control(camera, VIDIOC_REQBUFS, &reqbufs)) {
		CAMERA_V4L2_LOG_ERROR("Request buffer failed");
		return 0;
	}

	camera->buf = (camera_v4l2_buffer_t *) calloc(
		reqbufs.count,
		sizeof(*camera->buf));

	for (size_t i = 0; i < reqbufs.count; i++) {
		struct v4l2_buffer tmp;
		tmp.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		tmp.memory = V4L2_MEMORY_MMAP;
		tmp.index = i;

		if (!camera_v4l2_io_control(camera, VIDIOC_QUERYBUF, &tmp)) {
			CAMERA_V4L2_LOG_ERROR("Failed to query buffer");
			return 0;
		}
		camera->buf[i].length = tmp.length;
		camera->buf[i].start = mmap(
			NULL,
			tmp.length,
			PROT_READ | PROT_WRITE,
			MAP_SHARED,
			camera->fd,
			tmp.m.offset);
		if (camera->buf[i].start == MAP_FAILED) {
			CAMERA_V4L2_LOG_ERROR("Map buffer failed");
			return 0;
		}
	}

	return 1;
}

static int camera_v4l2_stream_on(camera_v4l2_camera_t *camera) {
	for (int i = 0; i < CAMERA_V4L2_BUFFER_COUNT; ++i) {
		struct v4l2_buffer buf;
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;
		if (!camera_v4l2_io_control(camera, VIDIOC_QBUF, &buf)) {
			CAMERA_V4L2_LOG_ERROR("Failed to queue buffer");
			return 0;
		}
	}

	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (!camera_v4l2_io_control(camera, VIDIOC_STREAMON, &type)) {
		CAMERA_V4L2_LOG_ERROR("Failed to start capture");
		return 0;
	}

	return 1;
}

static int camera_v4l2_stream_off(camera_v4l2_camera_t *camera) {
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (!camera_v4l2_io_control(camera, VIDIOC_STREAMOFF, &type)) {
		CAMERA_V4L2_LOG_ERROR("Failed to close capture");
		return 0;
	}
	return 1;
}

camera_v4l2_camera_t *camera_v4l2_create() {
	camera_v4l2_camera_t *camera = NULL;
	camera = (camera_v4l2_camera_t *) calloc(1, sizeof(*camera));

	camera->fd = -1;

	return camera;
}

void camera_v4l2_destroy(camera_v4l2_camera_t *camera) {
	CAMERA_V4L2_ASSERT(camera != NULL, "Object is NULL!!!");

	camera_v4l2_close(camera);

	free(camera->buf);

	free(camera);
}

int camera_v4l2_open(camera_v4l2_camera_t *camera,
		     int index, camera_v4l2_param_t *param) {
	CAMERA_V4L2_ASSERT(camera != NULL, "Object is NULL!!!");

	char path[32] = { 0 };
	sprintf(path, "/dev/video%d", index);
	camera->fd = open(path, O_RDWR | O_NONBLOCK);
	if (camera->fd < 0) {
		camera->fd = -1;
		CAMERA_V4L2_LOG_ERROR("Cannot open: %s, err: %s",
				      path, strerror(errno));
		return 0;
	}

	CAMERA_V4L2_LOG_INFO("Open %s success", path);

	if (!camera_v4l2_query_info(camera)) {
		CAMERA_V4L2_LOG_ERROR("Cannot query info!");
		goto failed;
	}

	if (param != NULL) {
		if (!camera_v4l2_set_param(camera, param)) {
			CAMERA_V4L2_LOG_ERROR("Cannot set param!");
			goto failed;
		}
	}

	if (!camera_v4l2_request_buffers(camera)) {
		CAMERA_V4L2_LOG_ERROR("Cannot request buffer!");
		goto failed;
	}

        if (!camera_v4l2_stream_on(camera)) {
		CAMERA_V4L2_LOG_ERROR("Open stream failed!");
		goto failed;
	}

	camera->streaming = 1;

	return 1;

failed:
	camera_v4l2_close(camera);
	return 0;
}

void camera_v4l2_close(camera_v4l2_camera_t *camera) {
	CAMERA_V4L2_ASSERT(camera != NULL, "Object is null");

	if (camera->fd != -1) {
		if (camera->streaming) {
			if (camera_v4l2_stream_off(camera)) {
				CAMERA_V4L2_LOG_INFO("Stream closed");
			} else {
				CAMERA_V4L2_LOG_ERROR("Stream close failed!");
			}
		}
		if (camera->buf != NULL) {
			for (int i = 0; i < CAMERA_V4L2_BUFFER_COUNT; i++) {
				if (camera->buf[i].length != 0 &&
				    camera->buf[i].start != NULL) {
					munmap(camera->buf[i].start,
					       camera->buf[i].length);

					camera->buf[i].start = NULL;
					camera->buf[i].length = 0;
				}
			}

			free(camera->buf);

			camera->buf = NULL;
		}

		close(camera->fd);
		camera->fd = -1;
		camera->streaming = 0;

		CAMERA_V4L2_LOG_INFO("Camera closed");
	}
}

int camera_v4l2_read(camera_v4l2_camera_t *camera,
		     camera_v4l2_buffer_t *frame) {
	CAMERA_V4L2_ASSERT(camera != NULL, "Object is null!!!");

	if (camera->fd == -1) {
		CAMERA_V4L2_LOG_ERROR("Invalid fd, do nothing");
		return 0;
	}

	struct v4l2_buffer buf;
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (!camera_v4l2_io_control(camera, VIDIOC_DQBUF, &buf)) {
		CAMERA_V4L2_LOG_ERROR("Dequeue buffer failed");
		return 0;
	}

	frame->start = camera->buf[buf.index].start;
	frame->length = buf.bytesused;

	if (!camera_v4l2_io_control(camera, VIDIOC_QBUF, &buf)) {
		CAMERA_V4L2_LOG_ERROR("Queue buffer failed");
		return 0;
	}

	return 1;
}

int camera_v4l2_isopened(camera_v4l2_camera_t *camera) {
	CAMERA_V4L2_ASSERT(camera != NULL, "Object is null!!!");
	return camera->fd != -1 && camera->streaming;
}

#undef CAMERA_V4L2_BUFFER_COUNT
#undef CAMERA_V4L2_ASSERT
#undef CAMERA_V4L2_LOG_ERROR
#undef CAMERA_V4L2_LOG_INFO
#undef CAMERA_V4L2_LOG_WARN

#ifdef __cpluscplus
}
#endif

#endif  // CAMERA_V4L2_IMPLEMENTATION
