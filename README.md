# StealthGame
Unreal Engine game with common stealth mechanics inspired by Hitman. You play as a fragile character tasked with stealing a valuable item without getting caught. Traps are your only means of defeating enemies, so you must cleverly lure them into your traps and outsmart them using the environment.
Work in Progress video 1 :https://www.youtube.com/watch?v=kWLQR2_Irmw  
Work in Progress video 1 :https://youtu.be/6uISkR-HPmQ  

Implemented mechanics:

- Suspicion System
- Target detection with AI Perception. Faster detection if closer to enemy.
- Suspicion world space UI widget
- Behaviors for Chasing, Patrolling, Searching around suspicious location
- Distraction, noise stimuli
- Distraction projectile path prediction and visualization


Currently in progress:
 - distraction animation integration
 - improve distraction/noise with some sort of ticketing system to prevent all characters from investigating a single location simultaneously.

To be implemented:

- Disguise System (Change player mesh + AI Reactions)
- Crouching for slower detection
- Improve chasing and searching using EQS
- Traps (Like falling chandeliers, wet floor, electrocuting, etc)
- Win State: steal item and escape to location
- Lose State: getting caught


