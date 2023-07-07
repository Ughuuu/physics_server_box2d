#pragma once

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
	Box2DCollisionObject *body;
	Vector<b2Shape *> shapes;

protected:
	bool configured = false;
	PhysicsServer2D::ShapeType type;

public:
	_FORCE_INLINE_ PhysicsServer2D::ShapeType get_type() const { return type; }

	void set_body(Box2DCollisionObject *p_body);
	Box2DCollisionObject *get_body() const;

	void add_b2_shape(b2Shape *p_shape);
	Vector<b2Shape *> get_b2_shapes() const;

	_FORCE_INLINE_ void set_self(const RID &p_self) { self = p_self; }
	_FORCE_INLINE_ RID get_self() const { return self; }

	_FORCE_INLINE_ bool is_configured() const { return configured; }
	virtual void set_data(const Variant &p_data) = 0;
	virtual Variant get_data() const = 0;

	virtual int get_b2Shape_count(bool is_static) const = 0;
	virtual b2Shape *get_transformed_b2Shape(int p_index, const Transform2D &p_transform, bool one_way, bool is_static) = 0;

	Box2DShape() { type = PhysicsServer2D::SHAPE_CUSTOM; }
	virtual ~Box2DShape(){};
};
