#ifndef VECTOR_3D_H
#define VECTOR_3D_H

#include <Atlas/Message/Object.h>

#include <math.h>

using std::cos;
using std::sin;

using Atlas::Message::Object;

class Vector3D {
    double x,y,z;
    int _set;
  public:

    Vector3D() : x(0), y(0), z(0), _set(0) { }
    Vector3D(double x, double y, double z) : x(x), y(y), z(z), _set(1) { }
    Vector3D(Object::ListType vector) {
        x = vector.front().AsFloat();
        vector.pop_front();
        y = vector.front().AsFloat();
        vector.pop_front();
        z = vector.front().AsFloat();
    }

    double X() const { return x; }
    double Y() const { return y; }
    double Z() const { return z; }

    bool operator==(const Vector3D & other) const {
        //"Check if two vector are equal";
        return ((x==other.x) && (y==other.y) && (z==other.z));
    }

    Vector3D operator+(const Vector3D & other) const {
        // Add two vectors
        return Vector3D(x+other.x, y+other.y, z+other.z);
    }

    Vector3D operator-(const Vector3D & other) const {
        // Subtract two vectors
        return Vector3D(x-other.x, y-other.y, z-other.z);
    }

    Vector3D operator*(double other) const {
        // Multiply vector by number
        return Vector3D(x*other,y*other,z*other);
    }

    Vector3D operator/(double other) const {
        // Divide vector by number
        return Vector3D(x/other,y/other,z/other);
    }

    operator bool() const {
        return(_set);
    }

    double operator[](int index) const {
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

    double dot(const Vector3D & v) const {
        //"Dot product of a vector";
        return x * v.x + y * v.y + z * v.z;
    }

    Vector3D cross(const Vector3D & v) const {
        //"Cross product of a vector";
        return Vector3D(y*v.z - z*v.x, z*v.x - x*v.z, x*v.y - y*v.z);
    }

    void rotatex(double angle) {
        //"Rotate a vector around x axis by angle in radians" ;
        z = cos(angle) * z + sin(angle) * x;
        x = sin(angle) * z + cos(angle) * x;
    }

    void rotatey(double angle) {
        //"Rotate a vector around y axis by angle in radians"        ;
        y = cos(angle) * y + sin(angle) * z;
        z = sin(angle) * y + cos(angle) * z;
    }

    void rotatez(double angle) {
        //"Rotate a vector around z axis by angle in radians" ;
        x = cos(angle) * x + sin(angle) * y;
        y = sin(angle) * x + cos(angle) * y;
    }

    double angle(const Vector3D & v) const {
        //"Find the angle between two vectors";
        double d = v.x * x + v.y * y + v.z * z;
        return d / (v.mag() * mag());
    }

    double mag() const {
        //"Find the magnitude of a vector";
        return sqrt(x*x + y*y + z*z);
    }


    Vector3D unit_vector() const {
        //"return the unit vector of a vector";
	// This is could throw a wobbly
	return operator/(mag());
    }

    Vector3D unit_vector_to_another_vector(const Vector3D & v) const {
        // return the unit vector in the direction of another vector;
        Vector3D difference_vector = v - (*this);
        return difference_vector.unit_vector();
    }

    double distance(const Vector3D & v) const {
        //"Find the distance between two vectors";
        return sqrt((x - v.x)*(x - v.x) + (y - v.y)*(y - v.y) + (z - v.z)*(z - v.z));
    }

    Object asObject() const {
        Object::ListType coords;
        coords.push_back(Object(x));
        coords.push_back(Object(y));
        coords.push_back(Object(z));
        return(Object(coords));
    }


    friend ostream & operator<<(ostream& s, const Vector3D& v);
};

inline ostream & operator<<(ostream& s, const Vector3D& v) {
    return s << "[" << v.x << "," << v.y << "," << v.z << "]";
}


#endif VECTOR_3D_H
