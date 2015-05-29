/*
 * BezierPatch.h
 *
 *  Created on: Apr 10, 2015
 *      Author: ryanyu
 */

#ifndef BEZIERPATCH_H_
#define BEZIERPATCH_H_

#include <queue>
#include <iostream>
#include <fstream>
#include <string>
#include <map>

class BezierPatch {
	public:
		std::vector<std::vector <Eigen::Vector3f> > listOfCurves;

		// final list of subdivided triangles, ready to feed to OpenGL display system
		std::vector<Triangle> listOfTriangles;

		// list of differential geometries (i.e. points) that we are evaluating the given patch at
		std::vector<DifferentialGeometry> listOfDifferentialGeometries;

		// queue of triangles for adaptive triangulation
		std::queue<Triangle> queueOfTriangles;

	BezierPatch() {

	}

	// Adds a curve to the list of curves.
	// NOTE: A curve, at initialization from the command line, is represented by a length-4 list of Vector3f's.
	//       That is, a curve is represented by a list of four points.
	void addCurve(std::vector<Eigen::Vector3f> curve) {
		listOfCurves.push_back(curve);
	}

	void addTriangle(DifferentialGeometry vertex1, DifferentialGeometry vertex2, DifferentialGeometry vertex3) {
		listOfTriangles.push_back(Triangle(vertex1, vertex2, vertex3));
	}

	void addDifferentialGeometry(Eigen::Vector3f position, Eigen::Vector3f normal, Eigen::Vector2f uvValues) {
		listOfDifferentialGeometries.push_back(DifferentialGeometry(position, normal, uvValues));
	}





	//****************************************************
	// Given the control points of a Bezier curve and a parametric value,
	// return the curve point and derivative.
	// This is a helper method that is used in 'evaulateDifferentialGeometry'.
	//
	// NOTE: This method is given in the last slide of CS184 Spring 2015 Lecture 14 (O'Brien)
	//***************************************************
	CurveLocalGeometry interpretBezierCurve(std::vector<Eigen::Vector3f> curve, float u) {
		// First, split each of the three segments to form two new ones, AB and BC
		// NOTE: 'curve' is a length-4 list of Vector3f's. Each Vector3f represents a control point of the curve.
		Eigen::Vector3f A = (curve[0] * (1.0 - u)) + (curve[1] * u);
		Eigen::Vector3f B = (curve[1] * (1.0 - u)) + (curve[2] * u);
		Eigen::Vector3f C = (curve[2] * (1.0 - u)) + (curve[3] * u);

		// Now, split AB and BC to form a new segment DE
		Eigen::Vector3f D = (A * (1.0 - u)) + (B * u);
		Eigen::Vector3f E = (B * (1.0 - u)) + (C * u);

		// Finally, pick the right point on DE; this is the point on the curve
		Eigen::Vector3f point = (D * (1.0 - u)) + (E * u);

		// Then, compute the derivative
		Eigen::Vector3f derivative = 3.0 * (E - D);

		return CurveLocalGeometry(point, derivative);
	}


	//****************************************************
	// Method that generates a DifferentialGeometry object that represents
	// the result of evaluating 'this' BezierPatch at (u, v)
	//
	// NOTE: This method is given in the last slide of CS184 Spring 2015 Lecture 14 (O'Brien)
	//***************************************************
	DifferentialGeometry evaluateDifferentialGeometry(float u, float v) {
		// listOfCurves[i] returns a list of points that represents one curve

		// Build control points for a Bezier curve in v
		Eigen::Vector3f vCurve0 = interpretBezierCurve(listOfCurves[0], u).point;
		Eigen::Vector3f vCurve1 = interpretBezierCurve(listOfCurves[1], u).point;
		Eigen::Vector3f vCurve2 = interpretBezierCurve(listOfCurves[2], u).point;
		Eigen::Vector3f vCurve3 = interpretBezierCurve(listOfCurves[3], u).point;

		std::vector<Eigen::Vector3f> uCurve0List;
		uCurve0List.push_back(listOfCurves[0][0]);
		uCurve0List.push_back(listOfCurves[1][0]);
		uCurve0List.push_back(listOfCurves[2][0]);
		uCurve0List.push_back(listOfCurves[3][0]);

		std::vector<Eigen::Vector3f> uCurve1List;
		uCurve1List.push_back(listOfCurves[0][1]);
		uCurve1List.push_back(listOfCurves[1][1]);
		uCurve1List.push_back(listOfCurves[2][1]);
		uCurve1List.push_back(listOfCurves[3][1]);

		std::vector<Eigen::Vector3f> uCurve2List;
		uCurve2List.push_back(listOfCurves[0][2]);
		uCurve2List.push_back(listOfCurves[1][2]);
		uCurve2List.push_back(listOfCurves[2][2]);
		uCurve2List.push_back(listOfCurves[3][2]);

		std::vector<Eigen::Vector3f> uCurve3List;
		uCurve3List.push_back(listOfCurves[0][3]);
		uCurve3List.push_back(listOfCurves[1][3]);
		uCurve3List.push_back(listOfCurves[2][3]);
		uCurve3List.push_back(listOfCurves[3][3]);

		// Build control points for a Bezier curve in v
		Eigen::Vector3f uCurve0 = interpretBezierCurve(uCurve0List, v).point;
		Eigen::Vector3f uCurve1 = interpretBezierCurve(uCurve1List, v).point;
		Eigen::Vector3f uCurve2 = interpretBezierCurve(uCurve2List, v).point;
		Eigen::Vector3f uCurve3 = interpretBezierCurve(uCurve3List, v).point;

		// Evaluate surface and derivative for u and v
		std::vector<Eigen::Vector3f> vCurve;
		vCurve.push_back(vCurve0);
		vCurve.push_back(vCurve1);
		vCurve.push_back(vCurve2);
		vCurve.push_back(vCurve3);

		std::vector<Eigen::Vector3f> uCurve;
		uCurve.push_back(uCurve0);
		uCurve.push_back(uCurve1);
		uCurve.push_back(uCurve2);
		uCurve.push_back(uCurve3);

		CurveLocalGeometry finalVCurve = interpretBezierCurve(vCurve, v);
		CurveLocalGeometry finalUCurve = interpretBezierCurve(uCurve, u);

		// Take cross product of partials to find normal
		Eigen::Vector3f normal = finalUCurve.derivative.cross(finalVCurve.derivative);
		normal.normalize();

		return DifferentialGeometry(finalUCurve.point, normal, Eigen::Vector2f(u, v));
	}


	//****************************************************
	// TODO: Method that populates each BezierPatch's list of DifferentialGeometries
	// and list of Triangles, based on adaptive subdivision
	//***************************************************
	void performAdaptiveSubdivision(float error) {

		listOfDifferentialGeometries.push_back(evaluateDifferentialGeometry(0, 0));
		listOfDifferentialGeometries.push_back(evaluateDifferentialGeometry(0, 1));
		listOfDifferentialGeometries.push_back(evaluateDifferentialGeometry(1, 0));
		listOfDifferentialGeometries.push_back(evaluateDifferentialGeometry(1, 1));

		queueOfTriangles.push(Triangle(listOfDifferentialGeometries[1], listOfDifferentialGeometries[2], listOfDifferentialGeometries[0]));
		queueOfTriangles.push(Triangle(listOfDifferentialGeometries[2], listOfDifferentialGeometries[1], listOfDifferentialGeometries[3]));

		DifferentialGeometry midpointInterpolatedValueAB;
		DifferentialGeometry midpointInterpolatedValueBC;
		DifferentialGeometry midpointInterpolatedValueAC;

		while (!queueOfTriangles.empty()) {

			Triangle currentTriangleToTest = queueOfTriangles.front();
			DifferentialGeometry pointA = currentTriangleToTest.point1;
			DifferentialGeometry pointB = currentTriangleToTest.point2;
			DifferentialGeometry pointC = currentTriangleToTest.point3;
			queueOfTriangles.pop();

			bool abSplit = false;
			bool bcSplit = false;
			bool acSplit = false;

			// Checking whether A -> B needs to be split
			Eigen::Vector2f uvValueToInterpolate = (pointA.uvValues + pointB.uvValues)/2.0f;
			midpointInterpolatedValueAB = evaluateDifferentialGeometry(uvValueToInterpolate.x(), uvValueToInterpolate.y());

			Eigen::Vector3f midpointApproximatedValue = (pointB.position - pointA.position)/2.0f + (pointA.position);

			Eigen::Vector3f errorVector = midpointInterpolatedValueAB.position - midpointApproximatedValue;
			float errorValue = sqrt(errorVector.dot(errorVector));


			if (errorValue >= error) {
				abSplit = true;
			}

			// Checking whether B -> C needs to be split
			uvValueToInterpolate = (pointB.uvValues + pointC.uvValues)/2.0f;
			midpointInterpolatedValueBC = evaluateDifferentialGeometry(uvValueToInterpolate.x(), uvValueToInterpolate.y());

			midpointApproximatedValue = (pointC.position - pointB.position)/2.0f + (pointB.position);

			errorVector = midpointInterpolatedValueBC.position - midpointApproximatedValue;
			errorValue = sqrt(errorVector.dot(errorVector));

			if (errorValue >= error) {
				bcSplit = true;
			}

			// Checking whether A -> C needs to be split
			uvValueToInterpolate = (pointA.uvValues + pointC.uvValues)/2.0f;
			midpointInterpolatedValueAC = evaluateDifferentialGeometry(uvValueToInterpolate.x(), uvValueToInterpolate.y());

			midpointApproximatedValue = (pointC.position - pointA.position)/2.0f + (pointA.position);

			errorVector = midpointInterpolatedValueAC.position - midpointApproximatedValue;
			errorValue = sqrt(errorVector.dot(errorVector));
			if (errorValue >= error) {
				acSplit = true;
			}

			// Case 1
			if (!abSplit && !bcSplit && !acSplit) {
				listOfTriangles.push_back(currentTriangleToTest);
			}
			// Case 2
			else if (!abSplit && !bcSplit && acSplit) {
				listOfDifferentialGeometries.push_back(midpointInterpolatedValueAC);
				queueOfTriangles.push(Triangle(pointA, pointB, midpointInterpolatedValueAC));
				queueOfTriangles.push(Triangle(midpointInterpolatedValueAC, pointB, pointC));
			}
			// Case 3
			else if (abSplit && !bcSplit && !acSplit) {
				listOfDifferentialGeometries.push_back(midpointInterpolatedValueAB);
				queueOfTriangles.push(Triangle(pointA, midpointInterpolatedValueAB, pointC));
				queueOfTriangles.push(Triangle(midpointInterpolatedValueAB, pointB, pointC));
			}
			// Case 4
			else if (!abSplit && bcSplit && !acSplit) {
				listOfDifferentialGeometries.push_back(midpointInterpolatedValueBC);
				queueOfTriangles.push(Triangle(pointA, pointB, midpointInterpolatedValueBC));
				queueOfTriangles.push(Triangle(pointA, midpointInterpolatedValueBC, pointC));
			}
			// Case 5
			else if (abSplit && !bcSplit && acSplit) {
				listOfDifferentialGeometries.push_back(midpointInterpolatedValueAB);
				listOfDifferentialGeometries.push_back(midpointInterpolatedValueAC);
				queueOfTriangles.push(Triangle(pointA, midpointInterpolatedValueAB, midpointInterpolatedValueAC));
				queueOfTriangles.push(Triangle(midpointInterpolatedValueAC, midpointInterpolatedValueAB, pointC));
				queueOfTriangles.push(Triangle(midpointInterpolatedValueAB, pointB, pointC));
			}
			// Case 6
			else if (abSplit && bcSplit && !acSplit) {
				listOfDifferentialGeometries.push_back(midpointInterpolatedValueAB);
				listOfDifferentialGeometries.push_back(midpointInterpolatedValueBC);
				queueOfTriangles.push(Triangle(pointA, midpointInterpolatedValueBC, pointC));
				queueOfTriangles.push(Triangle(pointA, midpointInterpolatedValueAB, midpointInterpolatedValueBC));
				queueOfTriangles.push(Triangle(midpointInterpolatedValueAB, pointB, midpointInterpolatedValueBC));
			}
			// Case 7
			else if (!abSplit && bcSplit && acSplit) {
				listOfDifferentialGeometries.push_back(midpointInterpolatedValueAC);
				listOfDifferentialGeometries.push_back(midpointInterpolatedValueBC);
				queueOfTriangles.push(Triangle(pointA, pointB, midpointInterpolatedValueAC));
				queueOfTriangles.push(Triangle(midpointInterpolatedValueAC, pointB, midpointInterpolatedValueBC));
				queueOfTriangles.push(Triangle(midpointInterpolatedValueAC, midpointInterpolatedValueBC, pointC));
			}
			// Case 8
			else if (abSplit && bcSplit && acSplit) {
				listOfDifferentialGeometries.push_back(midpointInterpolatedValueAC);
				listOfDifferentialGeometries.push_back(midpointInterpolatedValueBC);
				listOfDifferentialGeometries.push_back(midpointInterpolatedValueAB);
				queueOfTriangles.push(Triangle(pointA, midpointInterpolatedValueAB, midpointInterpolatedValueAC));
				queueOfTriangles.push(Triangle(midpointInterpolatedValueAB, pointB, midpointInterpolatedValueBC));
				queueOfTriangles.push(Triangle(midpointInterpolatedValueAC, midpointInterpolatedValueBC, pointC));
				queueOfTriangles.push(Triangle(midpointInterpolatedValueAC, midpointInterpolatedValueAB, midpointInterpolatedValueBC));
			}
		}
		// Algorithm:
		//
		// First, we add the DifferentialGeometries (u,v) = (0,0) , (0,1) , (1,0) , (1,1)
		// to our BezierPatch's list of differential geometries.
		//
		// 1  3
		// 2  4
		//
		// Next, we add two triangles to our triangle queue: 1-2-3  &  4-3-2
		//
		// while triangle_queue.is_not_empty:
		//     current_triangle = triangle_queue.pop();
		//
		//     (our triangle now looks like  A--C )
		//                                   | /
		//                                   B
		//
		//     for each edge (X -> Y) in edges [ (A -> B) ; (A -> C) ; (B -> C) ]:
		//          first, calculate the (u, v) value of the MIDPOINT of (X -> Y).
		//          this is given by: [(u, v) value of X   +   (u, v) value of Y] / 2.0f
		//
		//          then, interpolate this MIDPOINT (u, v) value with evaluateDifferentialGeometry(u, v).
		//          this results in a DifferentialGeometry midpointInterpolatedValue.
		//
		//          now, we need to calculate the APPROXIMATED (i.e. non interpolated) value of the differential geometry
		//          at this midpoint.
		//          this is given by:   [ [(x, y, z) value of Y    -   (x, y, z) value of X] / 2.0 ]  +  (x, y, z) value of X
		//          this results in a Eigen::Vector3f midpointApproximatedValue.
		//
		//          now, we compare midpointInterpolatedValue.position and midpointApproximatedValue
		//          i.e. subtract the two Vector3f's, and take the magnitude of their difference (i.e. square root of dot product with self)
		//          let us call this magnitude the 'errorValue'
		//
		//          if (errorValue >= error):
		//              then we need to mark (X -> Y) as NEEDING TO BE SPLIT
		//
		//    now, we have 3 boolean variables that correspond to whether (A -> B), (A -> C), (B -> C) need to be split.
		//    this corresponds with 8 total cases.
		//
		//    based on these 8 cases, we simply add relevant vertices to listOfDiferentialGeometries
		//    and add newly split triangles to our triangle queue as specified in the lecture slides
		//
		//    (if all 3 boolean variables are false, i.e. no splits necessary, then we add our popped triangle
		//     to the current bezier patch's listOfTriangles)
	}


	//****************************************************
	// Method that populates each BezierPatch's list of DifferentialGeometries
	// and list of Triangles, based on uniform subdivision
	//***************************************************
	void performUniformSubdivision(float stepSize) {
		float epsilon = 0.001f;
		int numberOfSteps = (1.0 + epsilon) / stepSize;
		for (int u = 0; u <= numberOfSteps; u++) {
			for (int v = 0; v <= numberOfSteps; v++) {
				// Evaluate the differential geometry at (u * stepSize, v * stepSize(
				// For instance, if stepSize = 0.1, then we would evaluate at (0, 0), (0, 0.1), (0, 0.2), etc
				listOfDifferentialGeometries.push_back(evaluateDifferentialGeometry(u * stepSize, v * stepSize));
			}
		}

		// NOTE: Code confirmed as working (tested)
		// Populate the list of Triangles, based on the list of points in listOfDifferentialGeometries
		// By ordering above, the DifferentialGeometries are ordered with the following numbering:
		//
		// 1 6  11 16 21
		// 2 7  12 17 22
		// 3 8  13 18 23
		// 4 9  14 19 24
		// 5 10 15 20 25
		//
		// where numberOfSteps = 5
		//
		// NOTE: By construction, our grid will always be n x n (i.e. a square)
		//
		// Basic Algorithm: for every 4 points in a rectangle, like below,
		//
		// 1 2
		// 3 4
		//
		// pair the points up in two triangles like so:
		//
		// 1--2
		// |/ |
		// 3--4
		//
		// so that 1-2-3 form a triangle (tri-1) and 2-3-4 form a triangle (tri-2)

		// Iterate through all of our differential geometries, but do NOT touch the right-most column and the bottom-most row
		for (int u = 0; u < numberOfSteps; u++) {
			for (int v = 0; v < numberOfSteps; v++) {
				// (u, v) represents the index in the above grid that we're triangulating
				// This index represents the TOP LEFT corner of the 4-point rectangle that is described above

				// Index of listOfDifferentialGeometries that corresponds with position (u, v)
				int differentialGeometrixIndex = (u * (numberOfSteps + 1)) + v;

				// NOTE: If stepSize = 0.2, then 1 / 0.2 = 5, but since we INCLUDE the fifth point, we actually have
				// 36 differential geometries in our list, so if you move RIGHT one point, you have to go
				// (numberOfSteps + 1) indexes down in the listOfDifferentialGeometries

				// Construct tri-1
				listOfTriangles.push_back(Triangle(
						listOfDifferentialGeometries[differentialGeometrixIndex + numberOfSteps + 1], // top right
						listOfDifferentialGeometries[differentialGeometrixIndex], // top left
						listOfDifferentialGeometries[differentialGeometrixIndex + 1])); // bottom left

				// Construct tri-2
				listOfTriangles.push_back(Triangle(
						listOfDifferentialGeometries[differentialGeometrixIndex + numberOfSteps + 1], // top right
						listOfDifferentialGeometries[differentialGeometrixIndex + 1], // bottom left
						listOfDifferentialGeometries[differentialGeometrixIndex + numberOfSteps + 2])); // bottom right
			}
		}
		// We should have (numberOfSteps - 1) * (numberOfSteps - 1) * 2 triangles
	}
};

#endif /* BEZIERPATCH_H_ */
