#include <Atlas/Message/Object.h>

#include "Vector3D.h"

#if 0
//Vector3D::Vector3D(double x, double y, double z) : x(x), y(y), z(z) { }


bool Vector3D::operator==(Vector3D other) const
{
        //"Check if two vector are equal";
        return ((x==other.x) && (y==other.y) && (z==other.z));
}


Vector3D Vector3D::operator+(Vector3D other)
{
        // Add two vectors
        return Vector3D(x+other.x, y+other.y, z+other.z);
}

Vector3D Vector3D::operator-(Vector3D other)
{
        // Subtract two vectors
        return Vector3D(x-other.x, y-other.y, z-other.z);
}

Vector3D Vector3D::operator*(double other)
{
        return Vector3D(x*other,y*other,z*other);
}

Vector3D Vector3D::operator/(double other)
{
        return Vector3D(x/other,y/other,z/other);
}


double Vector3D::operator[](int index)
{
	switch(index) {
		case 0:
			return(x);
		case 1:
			return(y);
		case 2:
			return(z);
		default:
			//Throw an exception here maybe
			return(0);
	}
}

bool Vector3D::operator!() const
{
    return(!_set);
}

double Vector3D::dot(Vector3D v)
{
        //"Dot product of a vector";
        return x * v.x + y * v.y + z * v.z;
}


Vector3D Vector3D::cross(Vector3D v)
{
        //"Cross product of a vector";
        return Vector3D(y*v.z - z*v.x, z*v.x - x*v.z, x*v.y - y*v.z);
}


void Vector3D::rotatey(double angle)
{
        //"Rotate a vector around y axis by angle in radians"        ;
        y = cos(angle) * y + sin(angle) * z;
        z = sin(angle) * y + cos(angle) * z;
        //return this;
}


void Vector3D::rotatex(double angle)
{
        //"Rotate a vector around x axis by angle in radians" ;
        z = cos(angle) * z + sin(angle) * x;
        x = sin(angle) * z + cos(angle) * x;
}


void Vector3D::rotatez(double angle)
{
        //"Rotate a vector around z axis by angle in radians" ;
        x = cos(angle) * x + sin(angle) * y;
        y = sin(angle) * x + cos(angle) * y;
}


double Vector3D::angle(Vector3D v)
{
        //"Find the angle between two vectors";
        double d = v.x * x + v.y * y + v.z * z;
        return d / (v.mag() * mag());
}


Vector3D Vector3D::unit_vector()
{
        //"return the unit vector of a vector";
	// This is could throw a wobbly
	return operator/(mag());
}


Vector3D Vector3D::unit_vector_to_another_vector(Vector3D v)
{
        //"return the unit vector in the direction of another vector";
        Vector3D difference_vector = v - (*this);
        return difference_vector.unit_vector();
}


double Vector3D::mag()
{
        //"Find the magnitude of a vector";
        return sqrt(x*x + y*y + z*z);
}


double Vector3D::distance(Vector3D v) const
{
        //"Find the distance between two vectors";
        return sqrt((x - v.x)*(x - v.x) + (y - v.y)*(y - v.y) + (z - v.z)*(z - v.z));
}

//ostream & operator<<(ostream& s, Vector3D& v) {
    //return s << "[" << v.x << "," << v.y << "," << v.z << "]";
//}


using namespace Atlas;
using namespace Message;

Object Vector3D::asObject() const
{
    Object::ListType coords;
    coords.push_back(Object(x));
    coords.push_back(Object(y));
    coords.push_back(Object(z));
    return(Object(coords));
}

#endif
