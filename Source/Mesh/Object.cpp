#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

#include <reactphysics3d/reactphysics3d.h>

#include "Object.h"

using namespace reactphysics3d;

Object::Object(glm::vec3 position)
{
	this->position = position;
	transform = Transform(Vector3(position.x, position.y, position.z), Quaternion::identity());
}

void Object::create(PhysicsCommon& common, PhysicsWorld* world, BodyType type, Vector3 size)
{
	shape = common.createBoxShape(size);
	rigidBody = world->createRigidBody(transform);
	collider = rigidBody->addCollider(shape, Transform::identity());
	collider->setIsSimulationCollider(true);
	rigidBody->setType(type);
	rigidBody->setIsDebugEnabled(true);
}

glm::vec3 Object::getPosition()
{
	// Display the position of the body
	// std::cout << "Body Position: (" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;

	Vector3 localRigidBodyPosition = rigidBody->getTransform().getPosition();
	/*Transform newTransform = rigidBody->getTransform();
	rp3d::Transform interpolatedTransform = rp3d::Transform::interpolateTransforms(transform, newTransform, 1.0f);
	transform = newTransform;

	Vector3 localRigidBodyPosition = interpolatedTransform.getPosition();*/

	return glm::vec3(localRigidBodyPosition.x, localRigidBodyPosition.y, localRigidBodyPosition.z);
}

Vector3 Object::toVector3Position()
{
	return Vector3(position.x, position.x, position.x);
}

RigidBody* Object::getRigidBody()
{
	return rigidBody;
}

Transform Object::getTransform()
{
	return rigidBody->getTransform();
}

BoxShape* Object::getShape()
{
	return shape;
}

glm::mat4 Object::getRotationMatrix()
{
	Matrix3x3 transformMatrix = getTransform().getOrientation().getMatrix();
	// Matrix3x3 transformMatrix = getTransform().getInverse().getOrientation().getMatrix();
	Vector3 row0 = transformMatrix.getColumn(0);
	Vector3 row1 = transformMatrix.getColumn(1);
	Vector3 row2 = transformMatrix.getColumn(2);

	return glm::mat4(
		glm::mat3(
			(float)row0.x, (float)row0.y, (float)row0.z,
			(float)row1.x, (float)row1.y, (float)row1.z,
			(float)row2.x, (float)row2.y, (float)row2.z
		)
	);
}
