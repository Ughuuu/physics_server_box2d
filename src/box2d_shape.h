#ifndef BOX2D_SHAPE_H
#define BOX2D_SHAPE_H

#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/variant/rid.hpp>
#include <godot_cpp/variant/variant.hpp>
#include <godot_cpp/variant/vector2.hpp>

#include <box2d/b2_shape.h>

using namespace godot;

class Box2DShape {
	RID self;

protected:
	bool configured = false;

public:
	_FORCE_INLINE_ void set_self(const RID &p_self) { self = p_self; }
	_FORCE_INLINE_ RID get_self() const { return self; }

	_FORCE_INLINE_ bool is_configured() const { return configured; }

	virtual void set_data(const Variant &p_data) = 0;
	virtual Variant get_data() const = 0;

	virtual int get_b2Shape_count() = 0;
	virtual b2Shape *get_transformed_b2Shape(int p_index, const Transform2D &p_transform, bool one_way, double one_way_margin) = 0;

	Box2DShape() {}
	virtual ~Box2DShape(){};
};

class Box2DShapeCircle : public Box2DShape {
	real_t radius = 0.0;

public:
	virtual void set_data(const Variant &p_data) override;
	virtual Variant get_data() const override;
	virtual int get_b2Shape_count() override { return 1; };
	virtual b2Shape *get_transformed_b2Shape(int p_index, const Transform2D &p_transform, bool one_way, double one_way_margin) override;

	Box2DShapeCircle();
	~Box2DShapeCircle();
};

class Box2DShapeRectangle : public Box2DShape {
	Vector2 half_extents;

public:
	virtual void set_data(const Variant &p_data) override;
	virtual Variant get_data() const override;
	virtual int get_b2Shape_count() override { return 1; }
	virtual b2Shape *get_transformed_b2Shape(int p_index, const Transform2D &p_transform, bool one_way, double one_way_margin) override;

	Box2DShapeRectangle();
	~Box2DShapeRectangle();
};

class Box2DShapeCapsule : public Box2DShape {
	real_t height = 0.0;
	real_t radius = 0.0;

public:
	virtual void set_data(const Variant &p_data) override;
	virtual Variant get_data() const override;
	virtual int get_b2Shape_count() override;
	virtual b2Shape *get_transformed_b2Shape(int p_index, const Transform2D &p_transform, bool one_way, double one_way_margin) override;

	Box2DShapeCapsule();
	~Box2DShapeCapsule();
};

class Box2DShapeConvexPolygon : public Box2DShape {
	Vector<Vector2> points;

public:
	virtual void set_data(const Variant &p_data) override;
	virtual Variant get_data() const override;
	virtual int get_b2Shape_count() override { return 1; }
	virtual b2Shape *get_transformed_b2Shape(int p_index, const Transform2D &p_transform, bool one_way, double one_way_margin) override;

	Box2DShapeConvexPolygon();
	~Box2DShapeConvexPolygon();
};

class Box2DShapeConcavePolygon : public Box2DShape {
	Vector<Vector2> points;

public:
	virtual void set_data(const Variant &p_data) override;
	virtual Variant get_data() const override;
	virtual int get_b2Shape_count() override;
	virtual b2Shape *get_transformed_b2Shape(int p_index, const Transform2D &p_transform, bool one_way, double one_way_margin) override;

	Box2DShapeConcavePolygon();
	~Box2DShapeConcavePolygon();
};

class Box2DShapeSegment : public Box2DShape {
	Vector2 a;
	Vector2 b;

public:
	virtual void set_data(const Variant &p_data) override;
	virtual Variant get_data() const override;
	virtual int get_b2Shape_count() override { return 1; };
	virtual b2Shape *get_transformed_b2Shape(int p_index, const Transform2D &p_transform, bool one_way, double one_way_margin) override;

	Box2DShapeSegment();
	~Box2DShapeSegment();
};

#endif // BOX2D_SHAPE_H
