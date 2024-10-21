#pragma once

#include "../../api/CryonicAPI.h"

class ScriptPreset : public Component
{
public:
	// IMPORTANT: Do not modify or remove the constructor. You may use Awake() as an alternative.
	ScriptPreset(GameObject* obj, int id) : Component(obj, id) {};
	
	// Called when the component is first initialized
	void Awake() override;
	
	// Called after Awake() but before the first frame update
	void Start() override;
	
	// Called once per frame
	void Update() override;
	
    /* 
     * The following functions are optional and can be overridden to handle specific events.
     * Uncomment and implement them in the ScriptPreset.cpp to receive callbacks for the corresponding event.
	 * Documentation for each event can be found in the official documentation on our website.
	 * You can remove this comment, as well as any unused functions including Awake(), Start(), and Update().
     */
	
	// Called at fixed time intervals for physics-related logic
	// void FixedUpdate() override;
	
	// Called when the component or gameobject is enabled/activated
    // void Enable() override;

    // Called when the component or gameobject is disabled/deactivated
    // void Disable() override;
	
	// Called when the game object or component is destroyed, or the scene is unloaded
	// void Destroy() override;

    // Called when the object starts colliding with another 2D collider
    // void OnCollisionEnter2D(Collider2D* other) override;

    // Called when the object stops colliding with another 2D collider
    // void OnCollisionExit2D(Collider2D* other) override;

    // Called every frame while the object is colliding with another 2D collider
    // void OnCollisionStay2D(Collider2D* other) override;
};