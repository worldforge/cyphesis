#ifndef VECTOR_3D_H
#define VECTOR_3D_H


class Vector3D {
  public:
    double x,y,z;

    Vector3D() : x(0), y(0), z(0) { }
    Vector3D(double x, double y, double z) : x(x), y(y), z(z) { }

    bool operator==(Vector3D other);
    Vector3D operator+(Vector3D other);
    Vector3D operator-(Vector3D other);
    Vector3D operator*(double other);
    Vector3D operator/(double other);
    double operator[](int index);
    double dot(Vector3D v);
    Vector3D cross(Vector3D v);
    void rotatey(double angle);
    void rotatex(double angle);
    void rotatez(double angle);
    double angle(Vector3D v);
    Vector3D unit_vector();
    Vector3D unit_vector_to_another_vector(Vector3D v);
    double mag();
    double distance(Vector3D v);
};

#endif VECTOR_3D_H
