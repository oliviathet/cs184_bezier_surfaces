/*
 * DifferentialGeometry.h
 *
 *  Created on: Apr 11, 2015
 *      Author: ryanyu
 */

#ifndef DIFFERENTIALGEOMETRY_H_
#define DIFFERENTIALGEOMETRY_H_

// Class that represents a point on a Primitive object, such as a sphere.
// We will primarily use this class for intersection points of primitives and rays
class DifferentialGeometry {
	public:
		// Position of the differential geometry, in world (x, y, z) coordinates
		Eigen::Vector3f position;

		// Normal of the differential geometry
		Eigen::Vector3f normal;

		// The (u, v) coordinate of the BezierPatch that corresponds to this differential geometry
		Eigen::Vector2f uvValues;

	DifferentialGeometry() {

	}

	DifferentialGeometry(Eigen::Vector3f position, Eigen::Vector3f normal, Eigen::Vector2f uvValues) {
		this->position = position;
		this->normal = normal;
		this->uvValues = uvValues;
	}

	DifferentialGeometry(Eigen::Vector3f position) {
		this->position = position;
	}
};


#endif /* DIFFERENTIALGEOMETRY_H_ */
