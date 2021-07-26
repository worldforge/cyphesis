/*
 Copyright (C) 2018 Erik Ogenvik

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "CyPy_Vector3D.h"
#include "CyPy_Point3D.h"
#include "CoordHelper.h"
#include "CyPy_Quaternion.h"

struct CyPy_Vector3DIterator : Py::PythonClass<CyPy_Vector3DIterator>
{
    //The owning element. Reference count is incremented at construction and decremented at destruction.
    CyPy_Vector3D* m_element;
    size_t iterator;

    CyPy_Vector3DIterator(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds)
            : PythonClass(self, args, kwds), m_element(nullptr), iterator(0)
    {
        throw Py::RuntimeError("Can not instantiate directly.");
    }


    CyPy_Vector3DIterator(Py::PythonClassInstance* self, CyPy_Vector3D* value)
            : PythonClass(self),
              m_element(value),
              iterator(0)
    {
        m_element->self().increment_reference_count();
    }

    ~CyPy_Vector3DIterator() override
    {
        m_element->self().decrement_reference_count();
    }

    Py::Object iter() override
    {
        return self();
    }

    PyObject* iternext() override
    {
        if (iterator < 3) {
            auto value = Py::Float(m_element->m_value[iterator]);
            value.increment_reference_count();
            iterator++;
            return value.ptr();
        } else {
            return nullptr;
        }
    }

    static void init_type()
    {
        behaviors().name("Vector3D iterator");
        behaviors().doc("");
        behaviors().supportIter(Py::PythonType::support_iter_iter | Py::PythonType::support_iter_iternext);

        behaviors().readyType();


    }

    static Py::PythonClassObject<CyPy_Vector3DIterator> wrap(CyPy_Vector3D* value)
    {
        auto obj = extension_object_new(type_object(), nullptr, nullptr);
        reinterpret_cast<Py::PythonClassInstance*>(obj)->m_pycxx_object = new CyPy_Vector3DIterator(reinterpret_cast<Py::PythonClassInstance*>(obj), value);
        return Py::PythonClassObject<CyPy_Vector3DIterator>(obj, true);
    }
};

CyPy_Vector3D::CyPy_Vector3D(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds)
    : WrapperBase(self, args, kwds)
{
    CoordHelper::init(m_value, args);
}

CyPy_Vector3D::CyPy_Vector3D(Py::PythonClassInstance* self, WFMath::Vector<3> value)
    : WrapperBase(self, std::move(value))
{

}

void CyPy_Vector3D::init_type()
{
    behaviors().name("Vector3D");
    behaviors().doc("");

    behaviors().supportRepr();
    behaviors().supportRichCompare();
    behaviors().supportSequenceType(Py::PythonType::support_sequence_item
                                    | Py::PythonType::support_sequence_length
                                    | Py::PythonType::support_sequence_ass_item);

    behaviors().supportNumberType(Py::PythonType::support_number_add
                                  | Py::PythonType::support_number_subtract
                                  | Py::PythonType::support_number_multiply
                                  | Py::PythonType::support_number_floor_divide
                                  | Py::PythonType::support_number_true_divide
                                  | Py::PythonType::support_number_negative);

    behaviors().supportIter(Py::PythonType::support_iter_iter);

    PYCXX_ADD_VARARGS_METHOD(dot, dot, "");
    PYCXX_ADD_VARARGS_METHOD(cross, cross, "");
    PYCXX_ADD_VARARGS_METHOD(rotatex, rotatex, "");
    PYCXX_ADD_VARARGS_METHOD(rotatey, rotatey, "");
    PYCXX_ADD_VARARGS_METHOD(rotatez, rotatez, "");
    PYCXX_ADD_VARARGS_METHOD(rotate, rotate, "");
    PYCXX_ADD_VARARGS_METHOD(angle, angle, "");
    PYCXX_ADD_VARARGS_METHOD(normalize, normalize, "");

    PYCXX_ADD_NOARGS_METHOD(sqr_mag, sqr_mag, "");
    PYCXX_ADD_NOARGS_METHOD(mag, mag, "");
    PYCXX_ADD_NOARGS_METHOD(is_valid, is_valid, "");
    PYCXX_ADD_NOARGS_METHOD(unit_vector, unit_vector, "");

    PYCXX_ADD_VARARGS_METHOD(unit_vector_to, unit_vector_to, "");

    behaviors().readyType();
}

WFMath::Vector<3> CyPy_Vector3D::parse(const Py::Object& object)
{
    return CoordHelper::parse<WFMath::Vector<3>, CyPy_Vector3D>(object);
}

Py::Object CyPy_Vector3D::getattro(const Py::String& name)
{
    return CoordHelper::getattro(*this, name);
}

int CyPy_Vector3D::setattro(const Py::String& name, const Py::Object& attr)
{
    return CoordHelper::setattro(m_value, name, attr);
}

Py::Object CyPy_Vector3D::repr()
{
    return CoordHelper::repr(m_value);
}

Py::Object CyPy_Vector3D::iter()
{
    return CyPy_Vector3DIterator::wrap(this);
}

Py::Object CyPy_Vector3D::dot(const Py::Tuple& args)
{
    args.verify_length(1, 1);

    if (!CyPy_Vector3D::check(args.front())) {
        throw Py::TypeError("Can only dot with Vector3D");
    }
    return Py::Float(WFMath::Dot(m_value, CyPy_Vector3D::value(args.front())));
}

Py::Object CyPy_Vector3D::cross(const Py::Tuple& args)
{
    args.verify_length(1, 1);

    if (!CyPy_Vector3D::check(args.front())) {
        throw Py::TypeError("Can only cross with Vector3D");
    }
    return CyPy_Vector3D::wrap(WFMath::Cross(m_value, CyPy_Vector3D::value(args.front())));
}

Py::Object CyPy_Vector3D::rotatex(const Py::Tuple& args)
{
    args.verify_length(1, 1);

    if (!args.front().isFloat()) {
        throw Py::TypeError("Can only rotate x with a float");
    }
    m_value.rotateX(Py::Float(args.front()));
    return self();
}

Py::Object CyPy_Vector3D::rotatey(const Py::Tuple& args)
{
    args.verify_length(1, 1);

    if (!args.front().isFloat()) {
        throw Py::TypeError("Can only rotate y with a float");
    }
    m_value.rotateY(Py::Float(args.front()));
    return self();
}

Py::Object CyPy_Vector3D::rotatez(const Py::Tuple& args)
{
    args.verify_length(1, 1);

    if (!args.front().isFloat()) {
        throw Py::TypeError("Can only rotate z with a float");
    }
    m_value.rotateZ(Py::Float(args.front()));
    return self();
}

Py::Object CyPy_Vector3D::rotate(const Py::Tuple& args)
{
    args.verify_length(1, 1);

    if (!CyPy_Quaternion::check(args.front())) {
        throw Py::TypeError("Can only rotate with a quaternion");
    }
    m_value.rotate(CyPy_Quaternion::value(args.front()));
    return self();
}

Py::Object CyPy_Vector3D::angle(const Py::Tuple& args)
{
    args.verify_length(1, 1);

    if (!CyPy_Vector3D::check(args.front())) {
        throw Py::TypeError("Can get angle to Vector3D");
    }
    return Py::Float(WFMath::Angle(m_value, CyPy_Vector3D::value(args.front())));
}

Py::Object CyPy_Vector3D::normalize(const Py::Tuple& args)
{
    args.verify_length(0, 1);

    WFMath::CoordType norm = 1.0;
    if (args.size()) {
        norm = verifyNumeric(args.front());
    }
    if (m_value != WFMath::Vector<3>::ZERO()) {
        m_value.normalize(norm);
    }

    return self();
}

Py::Object CyPy_Vector3D::sqr_mag()
{
    return Py::Float(m_value.sqrMag());
}

Py::Object CyPy_Vector3D::mag()
{
    return Py::Float(m_value.mag());
}

Py::Object CyPy_Vector3D::is_valid()
{
    return CoordHelper::is_valid(m_value);
}

Py::Object CyPy_Vector3D::unit_vector()
{
    auto value = m_value;

    WFMath::CoordType the_mag = value.mag();
    if (the_mag <= 0) {
        throw Py::ZeroDivisionError("Attempt to normalize a vector with zero magnitude");
    }
    value /= the_mag;
    return CyPy_Vector3D::wrap(value);
}

Py::Object CyPy_Vector3D::unit_vector_to(const Py::Tuple& args)
{
    args.verify_length(1, 1);

    if (!CyPy_Vector3D::check(args.front())) {
        throw Py::TypeError("Argument must be a Vector3D");
    }
    auto value = CyPy_Vector3D::value(args.front()) - m_value;
    WFMath::CoordType the_mag = value.mag();
    if (the_mag <= 0) {
        throw Py::ZeroDivisionError("Attempt to normalize a vector with zero magnitude");
    }
    value /= the_mag;
    return CyPy_Vector3D::wrap(value);
}

Py::Object CyPy_Vector3D::rich_compare(const Py::Object& other, int type)
{
    return CoordHelper::rich_compare<decltype(m_value), CyPy_Vector3D>(m_value, other, type);
}

PyCxx_ssize_t CyPy_Vector3D::sequence_length()
{
    if (m_value.isValid()) {
        return 3;
    } else {
        return 0;
    }
}

Py::Object CyPy_Vector3D::sequence_item(Py_ssize_t pos)
{
    return CoordHelper::sequence_item(m_value, pos);
}

int CyPy_Vector3D::sequence_ass_item(Py_ssize_t pos, const Py::Object& other)
{
    return CoordHelper::sequence_ass_item(m_value, pos, other);
}

Py::Object CyPy_Vector3D::number_add(const Py::Object& other)
{
    return CyPy_Vector3D::wrap(m_value + verifyObject<CyPy_Vector3D>(other, "Can only add Vector3D to Vector3D."));
}

Py::Object CyPy_Vector3D::number_subtract(const Py::Object& other)
{
    return CyPy_Vector3D::wrap(m_value - verifyObject<CyPy_Vector3D>(other, "Can only subtract Vector3D from Vector3D."));
}

Py::Object CyPy_Vector3D::number_multiply(const Py::Object& other)
{
    return CyPy_Vector3D::wrap(m_value * verifyNumeric(other, "Vector3D can only be multiplied by numeric value."));
}

Py::Object CyPy_Vector3D::number_floor_divide(const Py::Object& other)
{
    return CyPy_Vector3D::wrap(m_value / verifyNumeric(other, "Vector3D can only be divided by numeric value."));
}

Py::Object CyPy_Vector3D::number_true_divide(const Py::Object& other)
{
    return number_floor_divide(other);
}

Py::Object CyPy_Vector3D::number_negative()
{
    return CyPy_Vector3D::wrap(-m_value);

}
