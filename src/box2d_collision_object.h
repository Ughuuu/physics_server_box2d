#ifndef BOX2D_COLLISION_OBJECT_H
#define BOX2D_COLLISION_OBJECT_H

#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/variant/rid.hpp>

#include <box2d/b2_body.h>

#include "box2d_shape.h"
#include "box2d_space.h"
#include "box2d_type_conversions.h"

using namespace godot;

class Box2DCollisionObject {
public:
	enum Type {
		TYPE_AREA,
		TYPE_BODY
	};

protected:
	Type type;
	RID self;
	ObjectID instance_id;

	b2Body *body = nullptr;
	b2BodyDef *body_def = nullptr;
	Box2DSpace *space = nullptr;

	struct Shape {
		Transform2D xform;
		Box2DShape *shape = nullptr;
		Vector<b2Fixture *> fixtures;
		bool disabled = false;
		bool one_way_collision = false;
		double one_way_collision_margin = 0;
	};

	Vector<Shape> shapes;

	void _update_shapes();

protected:
	_FORCE_INLINE_ void _set_transform(const Transform2D &p_transform, bool p_update_shapes = true) {
		if (body) {
			Vector2 pos = p_transform.get_origin();
			b2Vec2 box2d_pos;
			godot_to_box2d(pos, box2d_pos);
			body->SetTransform(box2d_pos, p_transform.get_rotation());
		} else {
			godot_to_box2d(p_transform.get_origin(), body_def->position);
			body_def->angle = p_transform.get_rotation();
		}
		if (p_update_shapes) {
			_update_shapes();
		}
	}

	void _set_space(Box2DSpace *p_space);

	Box2DCollisionObject(Type p_type);

public:
	_FORCE_INLINE_ Type get_type() const { return type; }

	_FORCE_INLINE_ void set_self(const RID &p_self) { self = p_self; }
	_FORCE_INLINE_ RID get_self() const { return self; }

	_FORCE_INLINE_ void set_instance_id(const ObjectID &p_instance_id) { instance_id = p_instance_id; }
	_FORCE_INLINE_ ObjectID get_instance_id() const { return instance_id; }

	b2BodyDef *get_b2BodyDef() { return body_def; }
	void set_b2BodyDef(b2BodyDef *p_body_def) { body_def = p_body_def; }
	b2Body *get_b2Body() { return body; }
	void set_b2Body(b2Body *p_body) { body = p_body; }

	void add_shape(Box2DShape *p_shape, const Transform2D &p_transform = Transform2D(), bool p_disabled = false);
	void set_shape(int p_index, Box2DShape *p_shape);
	void set_shape_transform(int p_index, const Transform2D &p_transform);
	void set_shape_disabled(int p_index, bool p_disabled);
	void set_shape_as_one_way_collision(int p_index, bool enable, double margin);

	_FORCE_INLINE_ int get_shape_count() const { return shapes.size(); }
	_FORCE_INLINE_ Box2DShape *get_shape(int p_index) const {
		CRASH_BAD_INDEX(p_index, shapes.size());
		return shapes[p_index].shape;
	}

	_FORCE_INLINE_ const Transform2D &get_shape_transform(int p_index) const {
		CRASH_BAD_INDEX(p_index, shapes.size());
		return shapes[p_index].xform;
	}

	void remove_shape(Box2DShape *p_shape);
	void remove_shape(int p_index);

	_FORCE_INLINE_ const Transform2D get_transform() const {
		if (body) {
			Vector2 position;
			box2d_to_godot(body->GetPosition(), position);
			return Transform2D(body->GetAngle(), position);
		} else {
			Vector2 position;
			box2d_to_godot(body_def->position, position);
			return Transform2D(body_def->angle, position);
		}
	}

	virtual void set_space(Box2DSpace *p_space) = 0;
	_FORCE_INLINE_ Box2DSpace *get_space() const { return space; }

	Box2DCollisionObject();
	virtual ~Box2DCollisionObject();
};

#endif // BOX2D_COLLISION_OBJECT_H
