#include "CollisionListener.h"
#include "Components/Collider2D.h"
#include "ConsoleLogger.h"
#include <deque>
#include <algorithm>

std::deque<b2Contact*> continuedContact; // Todo: A deque might not be the best for this since elements may often be removed

void CollisionListener::BeginContact(b2Contact* contact)
{
    Collider2D* colliderA = reinterpret_cast<Collider2D*>(contact->GetFixtureA()->GetUserData().pointer);
    Collider2D* colliderB = reinterpret_cast<Collider2D*>(contact->GetFixtureB()->GetUserData().pointer);

    if (colliderA && colliderB)
    {
        for (Component* component : colliderA->gameObject->GetComponents())
            component->OnCollisionEnter2D(colliderB);
        for (Component* component : colliderB->gameObject->GetComponents())
            component->OnCollisionEnter2D(colliderA);
    }

    continuedContact.push_back(contact);
}

void CollisionListener::EndContact(b2Contact* contact)
{
    Collider2D* colliderA = reinterpret_cast<Collider2D*>(contact->GetFixtureA()->GetUserData().pointer);
    Collider2D* colliderB = reinterpret_cast<Collider2D*>(contact->GetFixtureB()->GetUserData().pointer);

    if (colliderA && colliderB)
    {
        for (Component* component : colliderA->gameObject->GetComponents())
            component->OnCollisionExit2D(colliderB);
        for (Component* component : colliderB->gameObject->GetComponents())
            component->OnCollisionExit2D(colliderA);
    }

    auto it = std::find_if(continuedContact.begin(), continuedContact.end(),
        [contact](b2Contact* ptr) { return ptr == contact; });
    if (it != continuedContact.end())
        continuedContact.erase(it);
}

void CollisionListener::ContinueContact()
{
    for (b2Contact* contact : continuedContact)
    {
        Collider2D* colliderA = reinterpret_cast<Collider2D*>(contact->GetFixtureA()->GetUserData().pointer);
        Collider2D* colliderB = reinterpret_cast<Collider2D*>(contact->GetFixtureB()->GetUserData().pointer);

        if (colliderA && colliderB)
        {
            for (Component* component : colliderA->gameObject->GetComponents())
                component->OnCollisionStay2D(colliderB);
            for (Component* component : colliderB->gameObject->GetComponents())
                component->OnCollisionStay2D(colliderA);
        }                           
    }
}
