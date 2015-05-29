/*
 * CurveLocalGeometry.h
 *
 *  Created on: Apr 12, 2015
 *      Author: ryanyu
 */

#ifndef CURVELOCALGEOMETRY_H_
#define CURVELOCALGEOMETRY_H_

// This class serves as an intermediary to return the result of evaluating an interpolated Bezier curve
// given its control points and a parametric value
class CurveLocalGeometry {

	public:
		Eigen::Vector3f point, derivative;

	CurveLocalGeometry() {

	}

	CurveLocalGeometry(Eigen::Vector3f point, Eigen::Vector3f derivative) {
		this->point = point;
		this->derivative = derivative;
	}

};



#endif /* CURVELOCALGEOMETRY_H_ */
