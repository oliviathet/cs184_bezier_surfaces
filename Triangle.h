/*
 * Triangle.h
 *
 *  Created on: Apr 11, 2015
 *      Author: ryanyu
 */

#ifndef TRIANGLE_H_
#define TRIANGLE_H_

class Triangle {
public:
	DifferentialGeometry point1, point2, point3;

	Triangle() {

	}

	Triangle(DifferentialGeometry v1, DifferentialGeometry v2, DifferentialGeometry v3) {
		this->point1 = v1;
		this->point2 = v2;
		this->point3 = v3;
	}

	// Converts float to string
	std::string convertFloatToString(float number){
	    std::ostringstream buff;
	    buff<<number;
	    return buff.str();
	}

	std::string printTriangleInformation() {
		float ax = point1.position.x();
		float ay = point1.position.y();
		float az = point1.position.z();
		float bx = point2.position.x();
		float by = point2.position.y();
		float bz = point2.position.z();
		float cx = point3.position.x();
		float cy = point3.position.y();
		float cz = point3.position.z();

		std::string axString = convertFloatToString(ax);
		std::string ayString = convertFloatToString(ay);
		std::string azString = convertFloatToString(az);
		std::string bxString = convertFloatToString(bx);
		std::string byString = convertFloatToString(by);
		std::string bzString = convertFloatToString(bz);
		std::string cxString = convertFloatToString(cx);
		std::string cyString = convertFloatToString(cy);
		std::string czString = convertFloatToString(cz);

		std::string toReturn = "Vertex 1 is (";
		toReturn.append(axString).append(", ").append(ayString).append(", ").append(azString).append(").  ");
		toReturn.append("Vertex 3 is (").append(bxString).append(", ").append(byString).append(", ").append(bzString).append(").  ");
		toReturn.append("Vertex 3 is (").append(cxString).append(", ").append(cyString).append(", ").append(czString).append(").\n");

		return toReturn;
	}


};

#endif /* TRIANGLE_H_ */
