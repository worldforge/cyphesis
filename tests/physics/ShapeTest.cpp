// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2011 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA


#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "physics/Shape.h"

#include "physics/Course.h"

#include <Atlas/Message/Element.h>

#include <wfmath/axisbox.h>
#include <wfmath/intersect.h>
#include <wfmath/line.h>
#include <wfmath/point.h>
#include <wfmath/polygon.h>

#include <iostream>

#include <cassert>

using Atlas::Message::ListType;
using Atlas::Message::MapType;

using WFMath::AxisBox;
using WFMath::Ball;
using WFMath::Line;
using WFMath::Point;
using WFMath::Polygon;
using WFMath::RotBox;
using WFMath::RotMatrix;
using WFMath::Vector;

namespace {
    void test_conversion(Shape &s) {
        Atlas::Message::MapType data;

        s.toAtlas(data);
        assert(!data.empty());
        assert(data.find("type") != data.end());
        assert(data["type"] != "unknown");

        auto copy = Shape::newFromAtlas(data);
        assert(copy);
        std::cout << "A: " << s << std::endl
                  << "B: " << *copy << std::endl;
        assert(s == *copy);
    }
}
// FIXME Use a C++11 template typedef once the are supported
template<int dim> class LinearCourse : public Course<dim, WFMath::Line>
{
};

int main()
{
    {
        MapType m;

        auto s = Shape::newFromAtlas(m);

        assert(!s);
    }

    // The AxisBox is a little different, and is not covered by the
    // name constructor
    {
        MathShape<AxisBox, 2> s(AxisBox<2>{});

        assert(!s.isValid());

    }

    {
        MathShape<AxisBox, 2> s(AxisBox<2>(Point<2>(0,0),
                                                         Point<2>(1,1)));

        assert(s.isValid());

    }

    {
        MathShape<AxisBox, 2> s(AxisBox<2>(Point<2>(0,0),
                                                        Point<2>(1,1)));

        assert(s.isValid());
        assert(s.intersect(Point<2>(0.5, 0.5)));
        assert(!s.intersect(Point<2>(1.5, 0.5)));
        assert(!s.intersect(Point<2>(1.5, 1.5)));
        assert(!s.intersect(Point<2>(0.5, 1.5)));
        assert(!s.intersect(Point<2>(-0.5, 1.5)));
        assert(!s.intersect(Point<2>(-0.5, 0.5)));
        assert(!s.intersect(Point<2>(-0.5, -0.5)));
        assert(!s.intersect(Point<2>(0.5, -0.5)));
        assert(!s.intersect(Point<2>(1.5, -0.5)));
    }

    {
        MapType m;

        MathShape<AxisBox, 2> s(AxisBox<2>{});
        s.fromAtlas(m);

        assert(!s.isValid());

    }

    {
        MathShape<AxisBox, 2> s(AxisBox<2>{});
        s.fromAtlas(ListType(2, 1.));

        assert(s.isValid());

    }

    {
        MathShape<AxisBox, 2> s(AxisBox<2>{});
        s.fromAtlas(ListType(2, 1.));

        assert(s.isValid());

        MapType dest;
        s.toAtlas(dest);
    }

    {
        MathShape<AxisBox, 2> s(AxisBox<2>{});
        s.fromAtlas(std::string("bad_string_value"));

        assert(!s.isValid());
    }

    {
        MathShape<AxisBox, 2> s(AxisBox<2>{});
        s.fromAtlas(ListType(2, 1.));

        assert(s.isValid());

        Point<2> low = s.lowCorner();
        Point<2> high = s.highCorner();
        assert(low.isValid());
        assert(high.isValid());
    }

    {
        MathShape<AxisBox, 2> s(AxisBox<2>(Point<2>(0,0),
                                                        Point<2>(2,2)));

        assert(s.isValid());
  
        Point<2> centre = s.centre();
        assert(Equal(centre, Point<2>(1,1)));
    }

    {
        MathShape<AxisBox, 2> s(AxisBox<2>(Point<2>(1,1),
                                                        Point<2>(2,2)));

        assert(s.isValid());
  
        test_conversion(s);
    }

    // Point
    {
        MathShape<Point, 2> s;

        assert(!s.isValid());
    }

    {
        MathShape<Point, 2> s(Point<2>(1,2));

        assert(s.isValid());
    }

    {
        MapType m;
        m["pos"] = ListType(2, 2.0);

        MathShape<Point, 2> s;

        assert(!s.isValid());

        s.fromAtlas(m);
        assert(s.isValid());
    }

    {
        ListType l(2, 2.0);

        MathShape<Point, 2> s;

        assert(!s.isValid());

        s.fromAtlas(l);
        assert(s.isValid());
    }

    {
        ListType l(1, 2.0); // Wrong length

        MathShape<Point, 2> s;

        assert(!s.isValid());

        s.fromAtlas(l);
        assert(!s.isValid());
    }

    {
        ListType l(2, "bad_string"); // Wrong type

        MathShape<Point, 2> s;

        assert(!s.isValid());

        s.fromAtlas(l);
        assert(!s.isValid());
    }

    {
        MathShape<Point, 2> s(Point<2>(1,2));

        assert(s.isValid());

        test_conversion(s);
    }

    {
        MathShape<Point, 2> s(Point<2>(1,2));

        assert(s.isValid());

        assert(s.intersect(Point<2>(1, 2)));
        assert(!s.intersect(Point<2>(0.75, 0.25)));
    }

    // The Polygon conversion functions throw if there isn't complete valid
    // polygon data
    {
        MathShape<Polygon, 2> s(Polygon<2>{});

        assert(s.isValid());
    }

    {
        Polygon<2> p;
        p.addCorner(0, Point<2>(1,1));
        p.addCorner(0, Point<2>(1,0));
        p.addCorner(0, Point<2>(0,0));
        MathShape<Polygon, 2> s(p);

        assert(s.isValid());
    }

    {
        Polygon<2> p;
        p.addCorner(0, Point<2>(1,1));
        p.addCorner(0, Point<2>(1,0));
        p.addCorner(0, Point<2>(0,0));
        // Make sure the underlying Intersect works
        assert(Intersect(p, Point<2>(0.75, 0.25), true));

        MathShape<Polygon, 2> s(p);

        assert(s.isValid());
        assert(s.intersect(Point<2>(0.75, 0.25)));
        assert(!s.intersect(Point<2>(1.5, 0.5)));
        assert(!s.intersect(Point<2>(1.5, 1.5)));
        assert(!s.intersect(Point<2>(0.5, 1.5)));
        assert(!s.intersect(Point<2>(-0.5, 1.5)));
        assert(!s.intersect(Point<2>(-0.5, 0.5)));
        assert(!s.intersect(Point<2>(-0.5, -0.5)));
        assert(!s.intersect(Point<2>(0.5, -0.5)));
        assert(!s.intersect(Point<2>(1.5, -0.5)));
    }

    {
        Polygon<2> p;
        p.addCorner(0, Point<2>(1,1));
        p.addCorner(0, Point<2>(1,0));
        p.addCorner(0, Point<2>(0,0));
        // Make sure the underlying Intersect works

        MathShape<Polygon, 2> s(p);

        assert(s.isValid());
        test_conversion(s);
    }

    {
        MapType m;
        m["type"] = "polygon";

        auto s = Shape::newFromAtlas(m);

        assert(s == 0);
    }

    {
        MapType m;
        m["type"] = "polygon";
        m["points"] = ListType(3, ListType(2, 1.f));

        auto s = Shape::newFromAtlas(m);

        assert(s != 0);
        assert(s->isValid());
    }

    {
        MapType m;
        m["type"] = "polygon";
        m["points"] = ListType(3, ListType(2, 1.f));

        auto s = Shape::newFromAtlas(m);

        assert(s != 0);
        assert(s->isValid());

        MapType dest;
        s->toAtlas(dest);
    }

    {
        MapType m;
        m["type"] = "polygon";
        ListType points;
        points.push_back(ListType(2, -1.));
        points.push_back(ListType(2, 1.));
        ListType point(1, 1.);
        point.push_back(-1.);
        points.push_back(point);
        m["points"] = points;

        auto s = Shape::newFromAtlas(m);

        assert(s != 0);
        assert(s->isValid());
        assert(s->area() > 1.9);
        assert(s->area() < 2.1);
    }

    {
        MapType m;
        m["type"] = "polygon";
        ListType points;
        points.push_back(ListType(2, -1.));
        points.push_back(ListType(2, 1.));
        ListType point(1, 1.);
        point.push_back(-1.);
        points.push_back(point);
        m["points"] = points;

        auto s = Shape::newFromAtlas(m);

        assert(s != 0);
        assert(s->isValid());
        double area = s->area();
        s->scale(2);
        assert(s->isValid());
        assert(area < s->area());
    }

    {
        MapType m;
        m["type"] = "polygon";
        ListType points;
        points.push_back(ListType(2, -1.));
        points.push_back(ListType(2, 1.));
        ListType point(1, 1.);
        point.push_back(-1.);
        points.push_back(point);
        m["points"] = points;

        auto s = Shape::newFromAtlas(m);

        assert(s != 0);
        assert(s->isValid());
        AxisBox<2> rect = s->footprint();
        std::cout << rect << std::endl;
        assert(rect.isValid());
    }

    {
        MapType m;
        m["type"] = "polygon";
        ListType points;
        points.push_back(ListType(2, -1.));
        points.push_back(ListType(2, 1.));
        ListType point(1, 1.);
        point.push_back(-1.);
        points.push_back(point);
        m["points"] = points;

        auto s = Shape::newFromAtlas(m);
        assert(s != 0);
        
        Area * a = dynamic_cast<Area *>(s.get());
        assert(a != 0);

        Point<2> low = a->lowCorner();
        Point<2> high = a->highCorner();
        assert(low.isValid());
        assert(high.isValid());
    }

    // The Ball conversion functions don't seem to require valid Atlas
    // data
    {
        MathShape<Ball, 2> s(Ball<2>{});

        assert(!s.isValid());
    }

    {
        MathShape<Ball, 2> s(
              Ball<2>(Point<2>(1,1), 23.f));

        assert(s.isValid());
    }

    {
        MapType m;
        m["type"] = "circle";

        auto s = Shape::newFromAtlas(m);

        assert(s != 0);
        assert(!s->isValid());
    }

    {
        MapType m;
        m["radius"] = 23.9;
        m["position"] = ListType(2, 1.f);

        MathShape<Ball, 2> s;
        s.fromAtlas(m);

        assert(s.isValid());
    }

    {
        MapType m;
        m["radius"] = 23.9;
        m["position"] = ListType(2, "bad_string"); // bad type here

        MathShape<Ball, 2> s;
        s.fromAtlas(m);

        assert(!s.isValid());
    }

    {
        ListType l(2, 1.f); // Wrong type

        MathShape<Ball, 2> s;
        s.fromAtlas(l);

        assert(!s.isValid());
    }

    {
        MapType m;
        m["type"] = "circle";
        m["radius"] = 23.9;
        m["position"] = ListType(2, 1.f);

        auto s = Shape::newFromAtlas(m);

        assert(s != 0);
        assert(s->isValid());
    }

    {
        MapType m;
        m["type"] = "circle";
        m["radius"] = 23.9;
        m["position"] = ListType(2, 1.f);

        auto s = Shape::newFromAtlas(m);

        assert(s != 0);

        MapType dest;
        s->toAtlas(dest);
    }

    {
        MapType m;
        m["type"] = "circle";
        m["radius"] = 23.9;
        m["position"] = ListType(2, 1.f);

        auto s = Shape::newFromAtlas(m);
        assert(s != 0);
        
        Area * a = dynamic_cast<Area *>(s.get());
        assert(a != 0);

        Point<2> low = a->lowCorner();
        Point<2> high = a->highCorner();
        assert(low.isValid());
        assert(high.isValid());
    }

    {
        MapType m;
        m["type"] = "circle";
        m["radius"] = 23.9;
        m["position"] = ListType(2, 1.f);

        auto s = Shape::newFromAtlas(m);
        assert(s != 0);
        
        test_conversion(*s);
    }

    // The RotBox conversion functions throw if there isn't complete valid
    // polygon data
    {
        MathShape<RotBox, 2> s((RotBox<2>()));

        assert(!s.isValid());
    }

    {
        MathShape<RotBox, 2> s(
            RotBox<2>(Point<2>(0,0),
                              Vector<2>(1,1),
                              RotMatrix<2>().identity()));

        assert(s.isValid());
    }

    {
        MapType m;
        m["type"] = "rotbox";

        auto s = Shape::newFromAtlas(m);

        assert(s == 0);
    }

    {
        MapType m;
        m["type"] = "rotbox";
        m["point"] = ListType(2, 1.f);
        m["size"] = ListType(2, 1.f);

        auto s = Shape::newFromAtlas(m);

        assert(s != 0);
        assert(s->isValid());
    }

    {
        MapType m;
        m["type"] = "rotbox";
        m["point"] = ListType(2, 1.f);
        m["size"] = ListType(2, 1.f);

        auto s = Shape::newFromAtlas(m);

        assert(s != 0);
        assert(s->isValid());

        MapType dest;
        s->toAtlas(dest);
    }

    {
        MapType m;
        m["type"] = "rotbox";
        m["point"] = ListType(2, 1.f);
        m["size"] = ListType(2, 1.f);

        auto s = Shape::newFromAtlas(m);
        assert(s != 0);
        
        Area * a = dynamic_cast<Area *>(s.get());
        assert(a != 0);

        Point<2> low = a->lowCorner();
        Point<2> high = a->highCorner();
        assert(low.isValid());
        assert(high.isValid());
    }

    {
        MapType m;
        m["type"] = "rotbox";
        m["point"] = ListType(2, 1.f);
        m["size"] = ListType(2, 1.f);

        auto s = Shape::newFromAtlas(m);
        assert(s != 0);
        
        // FIXME This doesn't work with an actual rotated box, as the underlying
        // wfmath functions don't support them yet
        test_conversion(*s);
    }

    // Line functions
    {
        MathShape<Line, 2> s;

        assert(!s.isValid());
    }

    {
        Line<2> p;
        p.addCorner(0, Point<2>(1,0));
        p.addCorner(0, Point<2>(0,0));
        MathShape<Line, 2> s(p);

        assert(s.isValid());
    }

    {
        Line<2> p;
        p.addCorner(0, Point<2>(1,1));
        p.addCorner(0, Point<2>(1,0));
        p.addCorner(0, Point<2>(0,0));

        MathShape<Line, 2> s(p);

        assert(s.isValid());
        assert(!s.intersect(Point<2>(0.75, 0.25)));
        assert(!s.intersect(Point<2>(1.5, 0.5)));
        assert(!s.intersect(Point<2>(1.5, 1.5)));
        assert(!s.intersect(Point<2>(0.5, 1.5)));
        assert(!s.intersect(Point<2>(-0.5, 1.5)));
        assert(!s.intersect(Point<2>(-0.5, 0.5)));
        assert(!s.intersect(Point<2>(-0.5, -0.5)));
        assert(!s.intersect(Point<2>(0.5, -0.5)));
        assert(!s.intersect(Point<2>(1.5, -0.5)));
    }

    {
        Line<2> p;
        p.addCorner(0, Point<2>(1,1));
        p.addCorner(0, Point<2>(1,0));
        p.addCorner(0, Point<2>(0,0));

        MathShape<Line, 2> s(p);

        assert(s.isValid());

        test_conversion(s);
    }

    {
        MapType m;
        m["type"] = "line";

        auto s = Shape::newFromAtlas(m);

        assert(s == 0);
    }

    {
        MapType m;
        m["type"] = "line";
        m["points"] = ListType(3, ListType(2, 1.f));

        auto s = Shape::newFromAtlas(m);

        assert(s != 0);
        assert(s->isValid());
    }

    {
        MapType m;
        m["type"] = "line";
        m["points"] = ListType(3, ListType(2, 1.f));

        auto s = Shape::newFromAtlas(m);

        assert(s != 0);
        assert(s->isValid());

        MapType dest;
        s->toAtlas(dest);
    }

    {
        MapType m;
        m["type"] = "line";
        ListType points;
        points.push_back(ListType(2, -1.));
        points.push_back(ListType(2, 1.));
        ListType point(1, 1.);
        point.push_back(-1.);
        points.push_back(point);
        m["points"] = points;

        auto s = Shape::newFromAtlas(m);

        assert(s != 0);
        assert(s->isValid());
        s->area();
    }

    {
        MapType m;
        m["type"] = "line";
        ListType points;
        points.push_back(ListType(2, -1.));
        points.push_back(ListType(2, 1.));
        ListType point(1, 1.);
        point.push_back(-1.);
        points.push_back(point);
        m["points"] = points;

        auto s = Shape::newFromAtlas(m);

        s->scale(2);
    }

    {
        MapType m;
        m["type"] = "line";
        ListType points;
        points.push_back(ListType(2, -1.));
        points.push_back(ListType(2, 1.));
        ListType point(1, 1.);
        point.push_back(-1.);
        points.push_back(point);
        m["points"] = points;

        auto s = Shape::newFromAtlas(m);

        assert(s != 0);
        assert(s->isValid());
        AxisBox<2> rect = s->footprint();
        std::cout << rect << std::endl;
        assert(rect.isValid());
    }

    {
        MapType m;
        m["type"] = "line";
        ListType points;
        points.push_back(ListType(2, -1.));
        points.push_back(ListType(2, 1.));
        ListType point(1, 1.);
        point.push_back(-1.);
        points.push_back(point);
        m["points"] = points;

        auto s = Shape::newFromAtlas(m);
        assert(s != 0);
        
        Area * a = dynamic_cast<Area *>(s.get());
        assert(a != 0);

        Point<2> low = a->lowCorner();
        Point<2> high = a->highCorner();
        assert(low.isValid());
        assert(high.isValid());
    }

    // Course functions

    {
        MathShape<LinearCourse, 2> s;

        assert(!s.isValid());
    }
    return 0;
}

