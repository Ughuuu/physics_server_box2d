#ifndef BOX2D_SHAPE_H
#define BOX2D_SHAPE_H

#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/variant/rid.hpp>
#include <godot_cpp/variant/variant.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/templates/vector.hpp>

#include <box2d/b2_shape.h>

using namespace godot;

class Box2DShape {
	RID self;

protected:
	b2Shape *shape = nullptr;
	bool configured = false;

public:
	_FORCE_INLINE_ void set_self(const RID &p_self) { self = p_self; }
	_FORCE_INLINE_ RID get_self() const { return self; }

	b2Shape* get_b2Shape() { return shape; }

	_FORCE_INLINE_ bool is_configured() const { return configured; }

	virtual void set_data(const Variant &p_data) = 0;
	virtual Variant get_data() const = 0;

	Box2DShape() {}
	virtual ~Box2DShape() {};
};

class Box2DShapeCircle: public Box2DShape {
	real_t radius;

public:
	virtual void set_data(const Variant &p_data) override;
	virtual Variant get_data() const override;

	Box2DShapeCircle();
	~Box2DShapeCircle();
};

class Box2DShapeRectangle: public Box2DShape {
	Vector2 half_extents;

public:
	virtual void set_data(const Variant &p_data) override;
	virtual Variant get_data() const override;

	Box2DShapeRectangle();
	~Box2DShapeRectangle();
};

class Box2DShapeConvexPolygon: public Box2DShape {
	Vector<Vector2> points;

public:
	virtual void set_data(const Variant &p_data) override;
	virtual Variant get_data() const override;

	Box2DShapeConvexPolygon();
	~Box2DShapeConvexPolygon();
};

#endif // BOX2D_SHAPE_H
