#ifndef VECTOR_3D_H
#define VECTOR_3D_H


class Vector3D {
  public:
    double x,y,z;

    Vector3D::Vector3D() : x(0), y(0), z(0) { }
    Vector3D::Vector3D(double x, double y, double z) : x(x), y(y), z(z) { }

    bool Vector3D::operator==(Vector3D other);
    Vector3D Vector3D::operator+(Vector3D other);
    Vector3D Vector3D::operator-(Vector3D other);
    Vector3D Vector3D::operator*(double other);
    Vector3D Vector3D::operator/(double other);
    double Vector3D::operator[](int index);
    double Vector3D::dot(Vector3D v);
    Vector3D Vector3D::cross(Vector3D v);
    void Vector3D::rotatey(double angle);
    void Vector3D::rotatex(double angle);
    void Vector3D::rotatez(double angle);
    double Vector3D::angle(Vector3D v);
    Vector3D Vector3D::unit_vector();
    Vector3D Vector3D::unit_vector_to_another_vector(Vector3D v);
    double Vector3D::mag();
    double Vector3D::distance(Vector3D v);
};

#endif VECTOR_3D_H
