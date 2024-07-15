#pragma once

using namespace reactphysics3d;

class Object
{
public:
	Object(glm::vec3 position);

	void create(PhysicsCommon& common, PhysicsWorld* world, BodyType type, Vector3 size);
	glm::vec3 getPosition();
	Transform getTransform();
	RigidBody* getRigidBody();
	BoxShape* getShape();
	Vector3 toVector3Position();
	glm::mat4 getRotationMatrix();

private:
	glm::vec3 position;
	Transform transform;
	BoxShape* shape;
	Collider* collider;
	RigidBody* rigidBody;
};