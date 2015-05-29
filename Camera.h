/*
 * Camera.h
 *
 *  Created on: Apr 11, 2015
 *      Author: ryanyu
 */

#ifndef CAMERA_H_
#define CAMERA_H_

class Camera {
	public:
		Eigen::Vector3f position, lookAt, up;

		// Used to determine bounding box for OpenGL
		Eigen::Vector3f minimum, maximum;

		// Used for gluPerspective()
		float zNear, zFar;
		float FIELD_OF_VIEW;

		// Current amount that the camera is zoomed in (default = 1.0)
		float ZOOM_AMOUNT;

		// The delta that the zoom amount gets changed by on each key press
		float ZOOM_DELTA;

		// Handle rotation factor and delta
		float X_ROTATION_AMOUNT;
		float Y_ROTATION_AMOUNT;
		float Z_ROTATION_AMOUNT;
		float ROTATION_DELTA;

		// Handle translation factor and delta
		float X_TRANSLATION_AMOUNT;
		float Y_TRANSLATION_AMOUNT;
		float Z_TRANSLATION_AMOUNT;
		float TRANSLATION_DELTA;


	Camera() {
		position = lookAt = up = minimum = maximum = Eigen::Vector3f(0, 0, 0);
		zNear = zFar = 0.0;
		FIELD_OF_VIEW = 90.0f;
		ZOOM_AMOUNT = 1.0f;
		ZOOM_DELTA = 0.05;
		ROTATION_DELTA = 2.0f;
		X_ROTATION_AMOUNT = Y_ROTATION_AMOUNT = Z_ROTATION_AMOUNT = 0.0;
		X_TRANSLATION_AMOUNT = Y_TRANSLATION_AMOUNT = Z_TRANSLATION_AMOUNT = 0.0;
		TRANSLATION_DELTA = 0.1f;

	}

	void zoomIn() {
		ZOOM_AMOUNT -= ZOOM_DELTA;
	}

	void zoomOut() {
		ZOOM_AMOUNT += ZOOM_DELTA;
	}

	void resetCamera() {
		FIELD_OF_VIEW = 90.0f;
		ZOOM_AMOUNT = 1.0f;
		ZOOM_DELTA = 0.1;
		X_ROTATION_AMOUNT = 0.0;
		Y_ROTATION_AMOUNT = 0.0;
		Z_ROTATION_AMOUNT = 0.0;
		X_TRANSLATION_AMOUNT = Y_TRANSLATION_AMOUNT = Z_TRANSLATION_AMOUNT = 0.0;
	}

	void rotateLeft() {
		X_ROTATION_AMOUNT -= ROTATION_DELTA;
	}

	void rotateRight() {
		X_ROTATION_AMOUNT += ROTATION_DELTA;
	}

	void rotateUp() {
		Y_ROTATION_AMOUNT += ROTATION_DELTA;
	}

	void rotateDown() {
		Y_ROTATION_AMOUNT -= ROTATION_DELTA;
	}

	void rotateZUp() {
		Z_ROTATION_AMOUNT += ROTATION_DELTA;
	}

	void rotateZDown() {
		Z_ROTATION_AMOUNT -= ROTATION_DELTA;
	}

	void translateUp() {
		Y_TRANSLATION_AMOUNT += TRANSLATION_DELTA;
	}

	void translateDown() {
		Y_TRANSLATION_AMOUNT -= TRANSLATION_DELTA;
	}

	void translateRight() {
		X_TRANSLATION_AMOUNT += TRANSLATION_DELTA;
	}

	void translateLeft() {
		X_TRANSLATION_AMOUNT -= TRANSLATION_DELTA;
	}

	void translateZUp() {
		Z_TRANSLATION_AMOUNT += TRANSLATION_DELTA;
	}

	void translateZDown() {
		Z_TRANSLATION_AMOUNT -= TRANSLATION_DELTA;
	}


};



#endif /* CAMERA_H_ */
