#pragma once

#include "box2d/box2d.h"

class CollisionListener2D : public b2ContactListener {
public:
	void BeginContact(b2Contact* contact) override;
	void EndContact(b2Contact* contact) override;
	void ContinueContact();
};