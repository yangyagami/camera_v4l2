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
extern "C" {
#endif

#endif  // CAMERA_V4L2_H_

#ifdef CAMERA_V4L2_IMPLEMENTATION

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

#define	CAMERA_V4L2_BUFFER_COUNT (4)

struct camera_v4l2_camera {
	int fd;
	uint8_t opened;
	camera_v4l2_buffer_t *buf;
};

static int camera_v4l2_io_control(camera_v4l2_camera_t *camera, int request,
				   void *output) {
	assert(camera != NULL && "Object is null!!!");

ioctl_retry:
	int ret = ioctl(camera->fd, request, output);
	if (ret >= 0) return 1;
	if (errno != EINPROGRESS && errno != EAGAIN && errno != EBUSY) {
		if (errno == EBADF || errno == ENOENT || errno == ENODEV) {
			camera->opened = 0;
			camera_v4l2_close(camera);
		}
		fprintf(stderr, "ioctl failed: %s\n", strerror(errno));
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
			fprintf(stderr, "select failed: %s\n", strerror(errno));
			return 0;
		}
	}

	return 1;
}

static void camera_v4l2_query_capability(camera_v4l2_camera_t *camera) {
	assert(camera != NULL && "Object is null!!!");

	struct v4l2_capability capability;

	if (camera_v4l2_io_control(camera, VIDIOC_QUERYCAP, &capability) == 0) {
		fprintf(stderr, "Query capability failed\n");
		return;
	}

	printf(
	       "Driver name: %s\n"
	       "Device name: %s\n"
	       "Bus info: %s\n"
	       "Version: %d\n\n",
	       capability.driver,
	       capability.card,
	       capability.bus_info,
	       capability.version
	);
}

static void camera_v4l2_query_frame_interval(
	camera_v4l2_camera_t *camera,
	uint32_t pixelfmt,
	int width,
	int height) {
	assert(camera != NULL && "Object is null!!!");

	struct v4l2_frmivalenum frmival;
	frmival.index = 0;
	frmival.pixel_format = pixelfmt;
	frmival.width = width;
	frmival.height = height;

	while (camera_v4l2_io_control(camera, VIDIOC_ENUM_FRAMEINTERVALS,
				      &frmival)) {
		printf(
		       "%d/%d ",
		       frmival.discrete.numerator,
		       frmival.discrete.denominator
		);

		frmival.index++;
	}
	printf("\n");
}

static void camera_v4l2_query_frame_size(camera_v4l2_camera_t *camera,
					 uint32_t pixelfmt) {
	assert(camera != NULL && "Object is null!!!");

	struct v4l2_frmsizeenum frmsize;
	frmsize.pixel_format = pixelfmt;
	frmsize.index = 0;

	while (camera_v4l2_io_control(camera, VIDIOC_ENUM_FRAMESIZES,
				      &frmsize)) {
		printf(
		       "width: %d, height: %d, fps: ",
		       frmsize.discrete.width,
		       frmsize.discrete.height
		);

		camera_v4l2_query_frame_interval(camera, pixelfmt, frmsize.discrete.width, frmsize.discrete.height);

		frmsize.index++;
	}
	printf("\n");
}

static void camera_v4l2_query_fmt_desc(camera_v4l2_camera_t *camera) {
	assert(camera != NULL && "Object is null!!!");

	struct v4l2_fmtdesc fmtdesc;
	fmtdesc.index = 0;
	fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	while (camera_v4l2_io_control(camera, VIDIOC_ENUM_FMT, &fmtdesc)) {
		printf(
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

static void camera_v4l2_query_info(camera_v4l2_camera_t *camera) {
	assert(camera != NULL && "Object is null!!!");

	camera_v4l2_query_capability(camera);
	camera_v4l2_query_fmt_desc(camera);
}

static void camera_v4l2_set_param(
	camera_v4l2_camera_t *camera,
	camera_v4l2_param_t *param) {
	assert(camera != NULL && "Object is null!!!");

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

	if (!camera_v4l2_io_control(camera, VIDIOC_S_FMT, &fmt)) {
		fprintf(stderr, "Cannot set param!\n");
	}
}

static int camera_v4l2_request_buffers(camera_v4l2_camera_t *camera) {
	assert(camera != NULL && "Object is null!!!");

	struct v4l2_requestbuffers reqbufs;
	reqbufs.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	reqbufs.memory = V4L2_MEMORY_MMAP;
	reqbufs.count = CAMERA_V4L2_BUFFER_COUNT;

	if (!camera_v4l2_io_control(camera, VIDIOC_REQBUFS, &reqbufs)) {
		fprintf(stderr, "Request buffer failed\n");
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
			fprintf(stderr, "Failed to query buffer\n");
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
			fprintf(stderr, "Map buffer failed\n");
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
		    fprintf(stderr, "Failed to queue buffer\n");
		    return 0;
		}
	}

	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (!camera_v4l2_io_control(camera, VIDIOC_STREAMON, &type)) {
	    fprintf(stderr, "Failed to start capture\n");
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
	assert(camera != NULL && "Object is NULL!!!");

	camera_v4l2_close(camera);

	free(camera->buf);

	free(camera);
}

int camera_v4l2_open(camera_v4l2_camera_t *camera,
		     int index, camera_v4l2_param_t *param) {
	assert(camera != NULL && "Object is null!!!");

	char path[32] = { 0 };
	sprintf(path, "/dev/video%d", index);
	camera->fd = open(path, O_RDWR | O_NONBLOCK);
	if (camera->fd < 0) {
		camera->opened = 0;
		camera->fd = -1;
		fprintf(stderr, "Cannot open: %s, err: %s\n", path,
			strerror(errno));
		return 0;
	}

	camera->opened = 1;

	printf("Open %s success\n", path);

	camera_v4l2_query_info(camera);

	if (param != NULL) {
		camera_v4l2_set_param(camera, param);
	}

	if (!camera_v4l2_request_buffers(camera)) {
		fprintf(stderr, "Cannot request buffer!");
	}

	return camera_v4l2_stream_on(camera);
}

void camera_v4l2_close(camera_v4l2_camera_t *camera) {
	assert(camera != NULL && "Object is null!!!");

	if (camera->fd != -1) {
		camera->opened = 0;
		close(camera->fd);
	}
}

int camera_v4l2_read(camera_v4l2_camera_t *camera,
		     camera_v4l2_buffer_t *frame) {
	assert(camera != NULL && "Object is null!!!");

	struct v4l2_buffer buf;
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (!camera_v4l2_io_control(camera, VIDIOC_DQBUF, &buf)) {
		fprintf(stderr, "Dequeue buffer failed\n");
		return 0;
	}

	frame->start = camera->buf[buf.index].start;
	frame->length = buf.bytesused;

	if (!camera_v4l2_io_control(camera, VIDIOC_QBUF, &buf)) {
		fprintf(stderr, "Queue buffer failed\n");
		return 0;
	}

	return 1;
}

int camera_v4l2_isopened(camera_v4l2_camera_t *camera) {
	assert(camera != NULL && "Object is null!!!");
	return camera->opened;
}

#undef CAMERA_V4L2_BUFFER_COUNT

#endif  // CAMERA_V4L2_IMPLEMENTATION
