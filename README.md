# Stealth Game 
Hitman-inspired stealth game made with Unreal Engine requiring players to strategically use the environment in order to defeat enemies and achieve objectives.

Work in progress demo : https://www.youtube.com/watch?v=qFmdNBOlCWE   
Blueprints showcase : coming soon   

I used a combination of C++ and blueprints. Blueprints handle interactable actors, certain abilities and AI BT tasks/services, EQS contexts, event responses (e.g. damage reactions, ability activation responses), visual elements like UI widgets, and objects with simpler logic such as the falling lamp. They are also used for actions that frequently occur over time, for the convenience of latent nodes (Timeline, PlayMontage, AsyncTurnInPlace). The core logic is done in C++. Primarily, I start prototyping in blueprints for rapid iteration and transition logic to C++ as complexity increases and my objectives become clearer.
## IMPLEMENTED FEATURES

- AI Stealth Behaviors
  - Distraction
  - Stealth Kills from triggering Falling objects
  - Investigation
  - Suspicion meter
 
- AI Combat
  - Dash Attacking, ragdoll + death when smashing into wall while dashing
  - Aggression Token System
  - Realistic chasing, strafing and tactical positioning using EQS. Allies take each other's influence into account.

- GAS Inspired ability system supporting blocking, cancelling and requiring abilities via gameplaytags. Abilities can be used for both AI and Player.
  - Gameplay Tag event system
  - Ability input binding with enhanced input
  - Implemented abilities: Throw Distraction, Draw distraction projectile path, Dashing , Pickup Interaction, Motion warp interaction, Charge & Shoot Projectile barrage
 
- Interaction System integrated with abilities
  - Synced animation interactions using motion warping including levers, switches and doors
  - unlocking, opening doors
  - Selection modes - Proximity, Camera
  - Interaction Filters - Dot, Object type (used for AI interactable noise searching)
  - Interactables inject their own behaviors into AI using dynamic subtrees (similar to smart objects)

- Character Turn In Place system for any angle without foot sliding, using generated animation curves from root motion
- Health, Death, Damage system
- Projectiles
- Object pooling for actors (used with projectiles)
- Mesh occlusion system for hiding object that block the camera view to character

## TO BE IMPLEMENTED

- Ability Cooldowns
- More AI reactions : reacting to dead bodies, darkness, falling objects, seeing allies in combat
- Disguises/Changing identity for not being detected
- More traps, light switch interaction, scan ability that reveals certain actors
- Shooting and Dash intercept point prediction based on target historical velocity
- Game objectives: kill target, steal thing, escape
- Hacking other characters (cyberpunk style), security cameras
- Item system & Hitman style inventory and equipment
