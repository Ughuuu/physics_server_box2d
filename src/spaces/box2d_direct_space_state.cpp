#include "box2d_direct_space_state.h"

#include "../b2_user_settings.h"

#include "../bodies/box2d_collision_object.h"
#include "../box2d_type_conversions.h"
#include "../servers/physics_server_box2d.h"
#include "box2d_query_callback.h"
#include "box2d_query_point_callback.h"
#include "box2d_ray_cast_callback.h"

#include <box2d/b2_collision.h>
#include <box2d/b2_fixture.h>

#define POINT_SIZE 0.000000001f

PhysicsDirectSpaceState2D *Box2DDirectSpaceState::get_space_state() {
	ERR_FAIL_NULL_V(space, nullptr);
	return space->get_direct_state();
}

bool Box2DDirectSpaceState::_intersect_ray(const Vector2 &from, const Vector2 &to, uint32_t collision_mask, bool collide_with_bodies, bool collide_with_areas, bool hit_from_inside, PhysicsServer2DExtensionRayResult *result) {
	Box2DRayCastCallback callback(this, result, collision_mask, collide_with_bodies, collide_with_areas, hit_from_inside);
	space->get_b2World()->RayCast(&callback, godot_to_box2d(from), godot_to_box2d(to));
	if (callback.get_hit()) {
		return true;
	}
	// also try point intersect
	if (hit_from_inside) {
		// try raycasting from other direction
		Box2DRayCastCallback callback_other_dir(this, result, collision_mask, collide_with_bodies, collide_with_areas, hit_from_inside);
		space->get_b2World()->RayCast(&callback_other_dir, godot_to_box2d(to), godot_to_box2d(from));
		if (callback_other_dir.get_hit()) {
			return true;
		}
		// try only a point in case the ray is completely inside
		Box2DQueryPointCallback callback(this,
				collision_mask,
				collide_with_bodies,
				collide_with_areas,
				-1,
				false,
				godot_to_box2d(from),
				10);
		b2Vec2 pos(godot_to_box2d(from));
		b2AABB aabb;

		aabb.lowerBound.Set(pos.x - POINT_SIZE, pos.y - POINT_SIZE);
		aabb.upperBound.Set(pos.x + POINT_SIZE, pos.y + POINT_SIZE);
		space->get_b2World()->QueryAABB(&callback, aabb);
		if (callback.get_results().size() != 0) {
			b2Fixture *fixture = callback.get_results()[0];
			result->normal = (box2d_to_godot(fixture->GetBody()->GetPosition()) - from).normalized();
			result->position = box2d_to_godot(fixture->GetBody()->GetPosition());
			result->shape = fixture->GetUserData().shape_idx;
			Box2DCollisionObject *collision_object = fixture->GetBody()->GetUserData().collision_object;
			result->rid = collision_object->get_self();
			result->collider_id = collision_object->get_object_instance_id();
			result->collider = collision_object->get_object_unsafe();
			return true;
		}
	}
	return false;
}
int32_t Box2DDirectSpaceState::_intersect_point(const Vector2 &position, uint64_t canvas_instance_id, uint32_t collision_mask, bool collide_with_bodies, bool collide_with_areas, PhysicsServer2DExtensionShapeResult *results, int32_t max_results) {
	if (max_results == 0) {
		return 0;
	}
	Box2DQueryPointCallback callback(this,
			collision_mask,
			collide_with_bodies,
			collide_with_areas,
			canvas_instance_id,
			true,
			godot_to_box2d(position),
			max_results);
	b2Vec2 pos(godot_to_box2d(position));
	b2AABB aabb;

	aabb.lowerBound.Set(pos.x - POINT_SIZE, pos.y - POINT_SIZE);
	aabb.upperBound.Set(pos.x + POINT_SIZE, pos.y + POINT_SIZE);
	space->get_b2World()->QueryAABB(&callback, aabb);
	Vector<b2Fixture *> collision_results = callback.get_results();
	for (b2Fixture *fixture : collision_results) {
		Box2DCollisionObject *collision_object = fixture->GetBody()->GetUserData().collision_object;
		PhysicsServer2DExtensionShapeResult &result = *results++;

		result.shape = fixture->GetUserData().shape_idx;
		result.rid = collision_object->get_self();
		result.collider_id = collision_object->get_object_instance_id();
		result.collider = collision_object->get_object_unsafe();
	}
	return collision_results.size();
}

int32_t Box2DDirectSpaceState::_intersect_shape(const RID &shape_rid, const Transform2D &transform, const Vector2 &motion, double margin, uint32_t collision_mask, bool collide_with_bodies, bool collide_with_areas, PhysicsServer2DExtensionShapeResult *result, int32_t max_results) {
	const Box2DShape *const_shape = space->get_server()->shape_owner.get_or_null(shape_rid);
	ERR_FAIL_COND_V(!const_shape, 0);
	Box2DShape *shape = const_cast<Box2DShape *>(const_shape);
	Vector<b2Fixture *> query_result = Box2DSweepTest::query_aabb_motion(shape, transform, motion, margin, collision_mask, collide_with_bodies, collide_with_areas, this);
	Vector<SweepTestResult> sweep_test_results = Box2DSweepTest::multiple_shapes_cast(shape, transform, motion, margin, collision_mask, collide_with_bodies, collide_with_areas, max_results, query_result, this);
	SweepTestResult sweep_test_result = Box2DSweepTest::closest_result_in_cast(sweep_test_results);
	if (!sweep_test_result.collision) {
		return 0;
	}
	int count = 0;
	PhysicsServer2DExtensionShapeResult &result_instance = result[count++];
	b2FixtureUserData fixture_B_user_data = sweep_test_result.sweep_shape_B.fixture->GetUserData();
	result_instance.shape = fixture_B_user_data.shape_idx;
	result_instance.rid = fixture_B_user_data.shape->get_self();
	Box2DCollisionObject *body_B = fixture_B_user_data.shape->get_body();
	result_instance.collider_id = body_B->get_object_instance_id();
	result_instance.collider = body_B->get_object_unsafe();
	return count;
}
bool Box2DDirectSpaceState::_cast_motion(const RID &shape_rid, const Transform2D &transform, const Vector2 &motion, double margin, uint32_t collision_mask, bool collide_with_bodies, bool collide_with_areas, float *closest_safe, float *closest_unsafe) {
	*closest_unsafe = 1.0f;
	*closest_safe = 1.0f;
	const Box2DShape *const_shape = space->get_server()->shape_owner.get_or_null(shape_rid);
	ERR_FAIL_COND_V(!const_shape, 0);
	Box2DShape *shape = const_cast<Box2DShape *>(const_shape);
	Vector<b2Fixture *> query_result = Box2DSweepTest::query_aabb_motion(shape, transform, motion, margin, collision_mask, collide_with_bodies, collide_with_areas, this);
	Vector<SweepTestResult> sweep_test_results = Box2DSweepTest::multiple_shapes_cast(shape, transform, motion, margin, collision_mask, collide_with_bodies, collide_with_areas, 2048, query_result, this);
	SweepTestResult sweep_test_result = Box2DSweepTest::closest_result_in_cast(sweep_test_results);
	if (!sweep_test_result.collision) {
		return true;
	}
	*closest_unsafe = sweep_test_result.unsafe_fraction();
	*closest_safe = sweep_test_result.safe_fraction();
	return true;
}
bool Box2DDirectSpaceState::_collide_shape(const RID &shape_rid, const Transform2D &transform, const Vector2 &motion, double margin, uint32_t collision_mask, bool collide_with_bodies, bool collide_with_areas, void *results, int32_t max_results, int32_t *result_count) {
	const Box2DShape *const_shape = space->get_server()->shape_owner.get_or_null(shape_rid);
	ERR_FAIL_COND_V(!const_shape, 0);
	Box2DShape *shape = const_cast<Box2DShape *>(const_shape);
	Vector<b2Fixture *> query_result = Box2DSweepTest::query_aabb_motion(shape, transform, motion, margin, collision_mask, collide_with_bodies, collide_with_areas, this);
	Vector<SweepTestResult> sweep_test_results = Box2DSweepTest::multiple_shapes_cast(shape, transform, motion, margin, collision_mask, collide_with_bodies, collide_with_areas, max_results, query_result, this);
	SweepTestResult sweep_test_result = Box2DSweepTest::closest_result_in_cast(sweep_test_results);
	auto *result = static_cast<Vector2 *>(results);
	if (!sweep_test_result.collision) {
		*result_count = 0;
		return false;
	}
	// TODO is this global or local?
	*result++ = box2d_to_godot(sweep_test_result.sweep_shape_B.transform.p + sweep_test_result.distance_output.pointB);
	*result++ = box2d_to_godot(sweep_test_result.sweep_shape_B.transform.p + sweep_test_result.distance_output.pointB);
	*result_count = 2;
	return true;
}
bool Box2DDirectSpaceState::_rest_info(const RID &shape_rid, const Transform2D &transform, const Vector2 &motion, double margin, uint32_t collision_mask, bool collide_with_bodies, bool collide_with_areas, PhysicsServer2DExtensionShapeRestInfo *rest_info) {
	const Box2DShape *const_shape = space->get_server()->shape_owner.get_or_null(shape_rid);
	ERR_FAIL_COND_V(!const_shape, 0);
	Box2DShape *shape = const_cast<Box2DShape *>(const_shape);
	Vector<b2Fixture *> query_result = Box2DSweepTest::query_aabb_motion(shape, transform, Vector2(), margin, collision_mask, collide_with_bodies, collide_with_areas, this);
	Vector<SweepTestResult> sweep_test_results = Box2DSweepTest::multiple_shapes_cast(shape, transform, Vector2(), margin, collision_mask, collide_with_bodies, collide_with_areas, 2048, query_result, this);
	SweepTestResult sweep_test_result = Box2DSweepTest::closest_result_in_cast(sweep_test_results);
	if (!sweep_test_result.collision) {
		return false;
	}
	PhysicsServer2DExtensionShapeRestInfo &result_instance = *rest_info;
	result_instance.shape = sweep_test_result.sweep_shape_B.fixture->GetUserData().shape_idx;
	result_instance.rid = sweep_test_result.sweep_shape_B.fixture->GetUserData().shape->get_self();
	result_instance.collider_id = sweep_test_result.sweep_shape_B.fixture->GetUserData().shape->get_body()->get_object_instance_id();
	result_instance.point = transform.get_origin() + box2d_to_godot(sweep_test_result.manifold.points[0]);
	result_instance.normal = box2d_to_godot(sweep_test_result.manifold.normal);
	result_instance.linear_velocity = box2d_to_godot(sweep_test_result.sweep_shape_B.fixture->GetBody()->GetLinearVelocity());
	return true;
}
