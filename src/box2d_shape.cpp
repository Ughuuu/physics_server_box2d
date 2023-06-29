#include "box2d_shape.h"
#include "box2d_collision_object.h"
#include "box2d_type_conversions.h"

#include <godot_cpp/core/memory.hpp>
#include "b2_user_settings.h"

#include <box2d/b2_chain_shape.h>
#include <box2d/b2_circle_shape.h>
#include <box2d/b2_edge_shape.h>
#include <box2d/b2_polygon_shape.h>

#define SHAPE_SMALLEST_VALUE b2_linearSlop * 2
#define SHAPE_EDGE_SMALLEST_VALUE 0.1

void Box2DShape::recreate_shape() {
	if (body) {
		body->recreate_shapes();
	}
}

/* CIRCLE SHAPE */

void Box2DShapeCircle::set_data(const Variant &p_data) {
	ERR_FAIL_COND(p_data.get_type() != Variant::FLOAT && p_data.get_type() != Variant::INT);
	radius = godot_to_box2d(p_data);
	if (radius < SHAPE_SMALLEST_VALUE) {
		ERR_PRINT("Radius is too small. Using " + rtos(SHAPE_SMALLEST_VALUE));
		radius = SHAPE_SMALLEST_VALUE;
	}
	configured = true;
}

Variant Box2DShapeCircle::get_data() const {
	return radius;
}

b2Shape *Box2DShapeCircle::create_transformed_b2Shape(int p_index, const Transform2D &p_transform, bool one_way) {
	ERR_FAIL_INDEX_V(p_index, 1, nullptr);
	b2CircleShape *shape = memnew(b2CircleShape);
	// for now we don't support elipsys
	Vector2 scale = p_transform.get_scale();
	if (scale.x != scale.y) {
		ERR_PRINT("Circles don't support non uniform scale.");
	}
	godot_to_box2d(radius * scale.x, shape->m_radius);
	godot_to_box2d(p_transform.get_origin(), shape->m_p);
	return shape;
}

/* RECTANGLE SHAPE */

void Box2DShapeRectangle::set_data(const Variant &p_data) {
	ERR_FAIL_COND(p_data.get_type() != Variant::VECTOR2);
	half_extents = p_data;
	if (half_extents.x < SHAPE_SMALLEST_VALUE) {
		ERR_PRINT("Width is too small. Using " + rtos(SHAPE_SMALLEST_VALUE));
		half_extents.x = SHAPE_SMALLEST_VALUE;
	}
	if (half_extents.y < SHAPE_SMALLEST_VALUE) {
		ERR_PRINT("Width is too small. Using " + rtos(SHAPE_SMALLEST_VALUE));
		half_extents.y = SHAPE_SMALLEST_VALUE;
	}
	configured = true;
}

Variant Box2DShapeRectangle::get_data() const {
	return half_extents;
}

b2Shape *Box2DShapeRectangle::create_transformed_b2Shape(int p_index, const Transform2D &p_transform, bool one_way) {
	ERR_FAIL_INDEX_V(p_index, 1, nullptr);
	b2PolygonShape *shape = memnew(b2PolygonShape);
	b2Vec2 box2d_half_extents = godot_to_box2d(half_extents);
	b2Vec2 *box2d_points = new b2Vec2[4];
	godot_to_box2d(p_transform.xform(Vector2(-half_extents.x, -half_extents.y)), box2d_points[0]);
	godot_to_box2d(p_transform.xform(Vector2(-half_extents.x, half_extents.y)), box2d_points[1]);
	godot_to_box2d(p_transform.xform(Vector2(half_extents.x, half_extents.y)), box2d_points[2]);
	godot_to_box2d(p_transform.xform(Vector2(half_extents.x, -half_extents.y)), box2d_points[3]);
	shape->Set(box2d_points, 4);
	delete[] box2d_points;
	return shape;
}

/* CAPSULE SHAPE */

void Box2DShapeCapsule::set_data(const Variant &p_data) {
	ERR_FAIL_COND(p_data.get_type() != Variant::ARRAY && p_data.get_type() != Variant::VECTOR2);
	if (p_data.get_type() == Variant::ARRAY) {
		Array arr = p_data;
		ERR_FAIL_COND(arr.size() != 2);
		height = arr[0];
		radius = arr[1];
	} else {
		Point2 p = p_data;
		radius = p.x;
		height = p.y;
	}
	if (radius < SHAPE_SMALLEST_VALUE) {
		ERR_PRINT("Radius is too small. Using " + rtos(SHAPE_SMALLEST_VALUE));
		radius = SHAPE_SMALLEST_VALUE;
	}
	if (height < SHAPE_SMALLEST_VALUE) {
		ERR_PRINT("Height is too small. Using " + rtos(SHAPE_SMALLEST_VALUE));
		height = SHAPE_SMALLEST_VALUE;
	}
	if (radius > height * 0.5 - SHAPE_SMALLEST_VALUE) {
		ERR_PRINT("Radius is bigger than half the height. Using " + rtos(height * 0.5 - SHAPE_SMALLEST_VALUE));
		radius = height * 0.5 - SHAPE_SMALLEST_VALUE;
	}
	configured = true;
}

Variant Box2DShapeCapsule::get_data() const {
	return Vector2(height, radius);
}

int Box2DShapeCapsule::get_b2Shape_count() const{
	// TODO: Better handle the degenerate case when the capsule is a sphere.
	return 3;
}

b2Shape *Box2DShapeCapsule::create_transformed_b2Shape(int p_index, const Transform2D &p_transform, bool one_way) {
	ERR_FAIL_INDEX_V(p_index, 3, nullptr);
	if (p_index == 0 || p_index == 1) {
		b2CircleShape *shape = memnew(b2CircleShape);
		godot_to_box2d(radius, shape->m_radius);
		real_t circle_height = (height * 0.5 - radius) * (p_index == 0 ? 1.0 : -1.0);
		godot_to_box2d(p_transform.xform(Vector2(0, circle_height)), shape->m_p);
		return shape;
	} else {
		b2PolygonShape *shape = memnew(b2PolygonShape);
		Vector2 half_extents(radius, height * 0.5 - radius);
		b2Vec2 box2d_half_extents = godot_to_box2d(half_extents);
		b2Vec2 box2d_origin = godot_to_box2d(p_transform.get_origin());
		shape->SetAsBox(box2d_half_extents.x, box2d_half_extents.y, box2d_origin, p_transform.get_rotation());
		return shape;
	}
	return nullptr; // This line is never reached, but it silences the compiler warning.
}

/* CONVEX POLYGON SHAPE */

void Box2DShapeConvexPolygon::set_data(const Variant &p_data) {
	ERR_FAIL_COND(p_data.get_type() != Variant::PACKED_VECTOR2_ARRAY);
	PackedVector2Array points_array = p_data;
	points.resize(points_array.size());
	for (int i = 0; i < points_array.size(); i++) {
		points.write[i] = points_array[i];
	}
	configured = true;
}

Variant Box2DShapeConvexPolygon::get_data() const {
	Array points_array;
	points_array.resize(points.size());
	for (int i = 0; i < points.size(); i++) {
		points_array[i] = points[i];
	}
	return points_array;
}

b2Shape *Box2DShapeConvexPolygon::create_transformed_b2Shape(int p_index, const Transform2D &p_transform, bool one_way) {
	ERR_FAIL_INDEX_V(p_index, 1, nullptr);
	if (points.size() >= b2_maxPolygonVertices) {
		// create a chain loop
		b2ChainShape *shape = memnew(b2ChainShape);
		b2Vec2 *box2d_points = new b2Vec2[points.size()];
		for (int i = 0; i < points.size(); i++) {
			godot_to_box2d(p_transform.xform(points[i]), box2d_points[i]);
		}
		shape->CreateLoop(box2d_points, (int)points.size());
		delete[] box2d_points;
		return shape;
	}
	b2PolygonShape *shape = memnew(b2PolygonShape);
	b2Vec2 *box2d_points = new b2Vec2[points.size()];
	for (int i = 0; i < points.size(); i++) {
		godot_to_box2d(p_transform.xform(points[i]), box2d_points[i]);
	}
	shape->Set(box2d_points, (int)points.size());
	delete[] box2d_points;
	return shape;
}

/* CONCAVE POLYGON SHAPE */

void Box2DShapeConcavePolygon::set_data(const Variant &p_data) {
	ERR_FAIL_COND(p_data.get_type() != Variant::PACKED_VECTOR2_ARRAY);
	PackedVector2Array points_array = p_data;
	points.resize(points_array.size());
	for (int i = 0; i < points_array.size(); i++) {
		points.write[i] = points_array[i];
	}
	configured = true;
}

Variant Box2DShapeConcavePolygon::get_data() const {
	Array points_array;
	points_array.resize(points.size());
	for (int i = 0; i < points.size(); i++) {
		points_array[i] = points[i];
	}
	return points_array;
}

b2Shape *Box2DShapeConcavePolygon::create_transformed_b2Shape(int p_index, const Transform2D &p_transform, bool one_way) {
	ERR_FAIL_INDEX_V(p_index, 1, nullptr);
	b2ChainShape *shape = memnew(b2ChainShape);
	b2Vec2 *box2d_points = new b2Vec2[points.size()];
	for (int i = 0; i < points.size(); i++) {
		godot_to_box2d(p_transform.xform(points[i]), box2d_points[i]);
	}
	shape->CreateChain(box2d_points, (int)points.size(), box2d_points[points.size() - 1], box2d_points[0]);
	delete[] box2d_points;
	return shape;
}

/* SEGMENT SHAPE */

void Box2DShapeSegment::set_data(const Variant &p_data) {
	ERR_FAIL_COND(p_data.get_type() != Variant::RECT2);
	Rect2 rect = p_data;
	a = rect.get_position();
	b = rect.get_size();
	half_extents = Vector2((a - b).length(), SHAPE_SMALLEST_VALUE);
	if (half_extents.x < SHAPE_SMALLEST_VALUE) {
		half_extents.x = SHAPE_SMALLEST_VALUE;
	}
	configured = true;
}

Variant Box2DShapeSegment::get_data() const {
	return Rect2(a, b);
}

b2Shape *Box2DShapeSegment::create_transformed_b2Shape(int p_index, const Transform2D &p_transform, bool one_way) {
	// make a line if it's static
	if (body && body->get_b2BodyDef()->type == b2_staticBody) {
		ERR_FAIL_INDEX_V(p_index, 1, nullptr);
		b2EdgeShape *shape = memnew(b2EdgeShape);
		b2Vec2 edge_endpoints[2];
		godot_to_box2d(p_transform.xform(a), edge_endpoints[0]);
		godot_to_box2d(p_transform.xform(b), edge_endpoints[1]);
		if (one_way) {
			b2Vec2 dirV0 = edge_endpoints[0] - edge_endpoints[1];
			shape->SetOneSided(edge_endpoints[1] + dirV0, edge_endpoints[0], edge_endpoints[1], edge_endpoints[0] - dirV0);
		} else {
			shape->SetTwoSided(edge_endpoints[0], edge_endpoints[1]);
		}
		return shape;
	}
	// make a square if not
	ERR_FAIL_INDEX_V(p_index, 1, nullptr);
	b2PolygonShape *shape = memnew(b2PolygonShape);
	b2Vec2 *box2d_points = new b2Vec2[4];
	Vector2 dir = (a - b).normalized();
	Vector2 right(dir.y, -dir.x);
	godot_to_box2d(p_transform.xform(a - right * SHAPE_EDGE_SMALLEST_VALUE), box2d_points[0]);
	godot_to_box2d(p_transform.xform(a + right * SHAPE_EDGE_SMALLEST_VALUE), box2d_points[1]);
	godot_to_box2d(p_transform.xform(b - right * SHAPE_EDGE_SMALLEST_VALUE), box2d_points[2]);
	godot_to_box2d(p_transform.xform(b + right * SHAPE_EDGE_SMALLEST_VALUE), box2d_points[3]);
	shape->Set(box2d_points, 4);
	delete[] box2d_points;
	return shape;
}

/* WORLD BOUNDARY SHAPE */

void Box2DShapeWorldBoundary::set_data(const Variant &p_data) {
	ERR_FAIL_COND(p_data.get_type() != Variant::ARRAY);
	Array arr = p_data;
	ERR_FAIL_COND(arr.size() != 2);
	normal = arr[0];
	distance = arr[1]; // no need to bring it to box2d here as we will do it later
	configured = true;
}

Variant Box2DShapeWorldBoundary::get_data() const {
	Array data;
	data.resize(2);
	data[0] = normal;
	data[1] = distance; // no need to bring it to godot here as we didn't cast this one
	return data;
}

b2Shape *Box2DShapeWorldBoundary::create_transformed_b2Shape(int p_index, const Transform2D &p_transform, bool one_way) {
	ERR_FAIL_INDEX_V(p_index, 1, nullptr);
	b2EdgeShape *shape = memnew(b2EdgeShape);
	b2Vec2 edge_endpoints[2];
	Vector2 right(normal.y, -normal.x);
	Vector2 left(-right);
	left *= 100000; // Multiply by large number
	right *= 100000;
	left = left + normal * distance;
	right = right + normal * distance;
	godot_to_box2d(p_transform.xform(left), edge_endpoints[0]);
	godot_to_box2d(p_transform.xform(right), edge_endpoints[1]);
	if (one_way) {
		b2Vec2 dirV0 = edge_endpoints[0] - edge_endpoints[1];
		shape->SetOneSided(edge_endpoints[1] + dirV0, edge_endpoints[0], edge_endpoints[1], edge_endpoints[0] - dirV0);
	} else {
		shape->SetTwoSided(edge_endpoints[0], edge_endpoints[1]);
	}
	return shape;
}

/* SEPARATION RAY SHAPE */

void Box2DShapeSeparationRay::set_data(const Variant &p_data) {
	// todo
	configured = true;
}

Variant Box2DShapeSeparationRay::get_data() const {
	// todo
	return Variant();
}

b2Shape *Box2DShapeSeparationRay::create_transformed_b2Shape(int p_index, const Transform2D &p_transform, bool one_way) {
	ERR_FAIL_INDEX_V(p_index, 1, nullptr);
	b2EdgeShape *shape = memnew(b2EdgeShape);
	b2Vec2 edge_endpoints[2];
	godot_to_box2d(p_transform.xform(a), edge_endpoints[0]);
	godot_to_box2d(p_transform.xform(b), edge_endpoints[1]);
	if (one_way) {
		b2Vec2 dirV0 = edge_endpoints[0] - edge_endpoints[1];
		shape->SetOneSided(edge_endpoints[1] + dirV0, edge_endpoints[0], edge_endpoints[1], edge_endpoints[0] - dirV0);
	} else {
		shape->SetTwoSided(edge_endpoints[0], edge_endpoints[1]);
	}
	return shape;
}