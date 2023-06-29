#ifndef BOX2D_SHAPE_H
#define BOX2D_SHAPE_H

#include <godot_cpp/classes/physics_server2d.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/variant/rid.hpp>
#include <godot_cpp/variant/variant.hpp>
#include <godot_cpp/variant/vector2.hpp>

#include <box2d/b2_shape.h>

using namespace godot;

class Box2DCollisionObject;

class Box2DShape {
	RID self;

protected:
	Box2DCollisionObject *body = nullptr;
	bool configured = false;
	PhysicsServer2D::ShapeType type;
	b2Shape *shape;

public:
	_FORCE_INLINE_ PhysicsServer2D::ShapeType get_type() const { return type; }

	_FORCE_INLINE_ void set_body(Box2DCollisionObject *p_body) { body = p_body; }
	_FORCE_INLINE_ void remove_body(const Box2DCollisionObject *p_body) { body = nullptr; }
	_FORCE_INLINE_ Box2DCollisionObject *get_body() const { return body; }

	_FORCE_INLINE_ void set_self(const RID &p_self) { self = p_self; }
	_FORCE_INLINE_ RID get_self() const { return self; }

	_FORCE_INLINE_ bool is_configured() const { return configured; }
	void recreate_shape();
	virtual void set_data(const Variant &p_data) = 0;
	virtual Variant get_data() const = 0;

	virtual int get_b2Shape_count() const = 0;
	virtual void set_b2Shape(b2Shape *p_shape) { shape = p_shape; }
	virtual b2Shape *get_b2Shape() const { return shape; }
	virtual b2Shape *create_transformed_b2Shape(int p_index, const Transform2D &p_transform, bool one_way) = 0;

	Box2DShape() { type = PhysicsServer2D::SHAPE_CUSTOM; }
	virtual ~Box2DShape(){};
};

class Box2DShapeCircle : public Box2DShape {
	real_t radius = 0.0;

public:
	virtual void set_data(const Variant &p_data) override;
	virtual Variant get_data() const override;
	virtual int get_b2Shape_count() const override { return 1; };
	virtual b2Shape *create_transformed_b2Shape(int p_index, const Transform2D &p_transform, bool one_way) override;

	Box2DShapeCircle() { type = PhysicsServer2D::SHAPE_CIRCLE; }
	~Box2DShapeCircle(){};
};

class Box2DShapeRectangle : public Box2DShape {
protected:
	Vector2 half_extents;

public:
	virtual void set_data(const Variant &p_data) override;
	virtual Variant get_data() const override;
	virtual int get_b2Shape_count() const override { return 1; }
	virtual b2Shape *create_transformed_b2Shape(int p_index, const Transform2D &p_transform, bool one_way) override;

	Box2DShapeRectangle() { type = PhysicsServer2D::SHAPE_RECTANGLE; }
	~Box2DShapeRectangle() {}
};

class Box2DShapeCapsule : public Box2DShape {
	real_t height = 0.0;
	real_t radius = 0.0;

public:
	virtual void set_data(const Variant &p_data) override;
	virtual Variant get_data() const override;
	virtual int get_b2Shape_count() const override;
	virtual b2Shape *create_transformed_b2Shape(int p_index, const Transform2D &p_transform, bool one_way) override;

	Box2DShapeCapsule() { type = PhysicsServer2D::SHAPE_CAPSULE; }
	~Box2DShapeCapsule() {}
};

class Box2DShapeConvexPolygon : public Box2DShape {
	Vector<Vector2> points;

public:
	virtual void set_data(const Variant &p_data) override;
	virtual Variant get_data() const override;
	virtual int get_b2Shape_count() const override { return 1; }
	virtual b2Shape *create_transformed_b2Shape(int p_index, const Transform2D &p_transform, bool one_way) override;

	Box2DShapeConvexPolygon() { type = PhysicsServer2D::SHAPE_CONVEX_POLYGON; }
	~Box2DShapeConvexPolygon() {}
};

class Box2DShapeConcavePolygon : public Box2DShape {
	Vector<Vector2> points;

public:
	virtual void set_data(const Variant &p_data) override;
	virtual Variant get_data() const override;
	virtual int get_b2Shape_count() const override { return 1; }
	virtual b2Shape *create_transformed_b2Shape(int p_index, const Transform2D &p_transform, bool one_way) override;

	Box2DShapeConcavePolygon() { type = PhysicsServer2D::SHAPE_CONCAVE_POLYGON; }
	~Box2DShapeConcavePolygon() {}
};

class Box2DShapeSegment : public Box2DShapeRectangle {
	Vector2 a;
	Vector2 b;

public:
	virtual void set_data(const Variant &p_data) override;
	virtual Variant get_data() const override;
	virtual int get_b2Shape_count() const override { return 1; };
	virtual b2Shape *create_transformed_b2Shape(int p_index, const Transform2D &p_transform, bool one_way) override;

	Box2DShapeSegment() { type = PhysicsServer2D::SHAPE_SEGMENT; }
	~Box2DShapeSegment() {}
};

class Box2DShapeWorldBoundary : public Box2DShape {
	Vector2 normal;
	real_t distance;

public:
	virtual void set_data(const Variant &p_data) override;
	virtual Variant get_data() const override;
	virtual int get_b2Shape_count() const override { return 1; };
	virtual b2Shape *create_transformed_b2Shape(int p_index, const Transform2D &p_transform, bool one_way) override;

	Box2DShapeWorldBoundary() { type = PhysicsServer2D::SHAPE_WORLD_BOUNDARY; }
	~Box2DShapeWorldBoundary() {}
};

class Box2DShapeSeparationRay : public Box2DShape {
	Vector2 a;
	Vector2 b;

public:
	virtual void set_data(const Variant &p_data) override;
	virtual Variant get_data() const override;
	virtual int get_b2Shape_count() const override { return 1; };
	virtual b2Shape *create_transformed_b2Shape(int p_index, const Transform2D &p_transform, bool one_way) override;

	Box2DShapeSeparationRay() { type = PhysicsServer2D::SHAPE_SEPARATION_RAY; }
	~Box2DShapeSeparationRay() {}
};

#endif // BOX2D_SHAPE_H
