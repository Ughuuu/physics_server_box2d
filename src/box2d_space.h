#ifndef BOX2D_SPACE_H
#define BOX2D_SPACE_H

#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/templates/self_list.hpp>
#include <godot_cpp/variant/packed_vector2_array.hpp>
#include <godot_cpp/variant/rid.hpp>

#include <box2d/b2_world.h>

using namespace godot;

class Box2DCollisionObject;
class Box2DBody;
class Box2DDirectSpaceState;
class Box2DJoint;

class Box2DSpace {
private:
	RID self;

	int active_body_count = 0;
	b2World *world = nullptr;

	SelfList<Box2DBody>::List active_list;
	SelfList<Box2DBody>::List state_query_list;

	bool locked = false;

	double solver_iterations = 8;
	double step_interval = 0;

	Box2DDirectSpaceState *direct_state = nullptr;

public:
	double get_step();
	void set_solver_iterations(int32 iterations);
	int32 get_solver_iterations() const;

	_FORCE_INLINE_ void set_self(const RID &p_self) { self = p_self; }
	_FORCE_INLINE_ RID get_self() const { return self; }

	b2World *get_b2World() const { return world; }

	const SelfList<Box2DBody>::List &get_active_body_list() const;
	void body_add_to_active_list(SelfList<Box2DBody> *p_body);
	void body_remove_from_active_list(SelfList<Box2DBody> *p_body);

	void body_add_to_state_query_list(SelfList<Box2DBody> *p_body);
	void body_remove_from_state_query_list(SelfList<Box2DBody> *p_body);

	void add_object(Box2DCollisionObject *p_object);
	void remove_object(Box2DCollisionObject *p_object);

	void create_joint(Box2DJoint *joint);
	void remove_joint(Box2DJoint *joint);
	void step(float p_step);
	void call_queries();

	bool is_locked() const { return locked; }
	void lock() { locked = true; }
	void unlock() { locked = false; }

	int get_active_body_count();

	int32_t get_contact_count() const;
	PackedVector2Array get_contacts() const;
	void set_debug_contacts(int32_t max_contacts);

	Box2DDirectSpaceState *get_direct_state();

	Box2DSpace();
	~Box2DSpace();
};

#endif // BOX2D_SPACE_H
