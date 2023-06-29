extends RigidBody2D

func _physics_process(delta):
	var dir = get_global_mouse_position() - position
	linear_velocity = dir.normalized() * delta * 10000
	
