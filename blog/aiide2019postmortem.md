# Blog 1: AIIDE 2019 PostMortem 
September 15th 2019 


## Initial Thoughts 

Let me begin by saying that I did not put nearly as much time into this as I would have liked. When I registered for AIIDE back in August, my bot could not even properly attack enemy units. By the 14th of September, one day before the competition, my bot was still having trouble building a bunker (the one thing my bot was advertised to do). Progress for much of the month had been at a snail's pace due to a combination of exams, other commitments, as well as my own lack of self-discipline. Being that this was a project done purely out of my own interest and engagement, it was unfortunately a bit low on my priorities list. 

If you'll excuse the negativity, I fully expect this bot to be the worst bot in AIIDE. Hell, I would be surprised if it even won one match. The bot itself is barely put together, barely functional, and only tested on one map - ICCUP Bluestorm, which I'm positive isn't even in the map pool. 

That being said, my main goal from the very beginning was to submit a bot in AIIDE, and to that end, I succeeded. Good job, Haoda. 

## The State of the Bot : Version 0.1 



## Lessons learned, Anticipated Improvements for Version 0.2 

One of the biggest lessons I learned is that there is a very big difference between playing Starcraft myself (as a human), and programming a bot through BWAPI. A big issue I had was purely in the way I organized my code. 

### Unit Control
The way I had programmed unit control in general was intuitive to the way I played Starcraft. 

When humans play Starcraft, we categorize and group certain units together and think of them as a single entity. This is sometimes done ingame using control groups, and sometimes its purely mental. For example, when I perform a bunker rush, I have my marines and my SCVs. It is the marine's job to do damage and stay alive, while it is the SCV's job to build and repair the bunker. 

This way of thinking is reflected in the way I coded BunkerBoxeR. BunkerBoxeR 0.1 is sub-divided into different modules which controls different aspects of the game. Each module contains one or more "groups" (i.e. `std::vector<Unit>`) of units, through which it issues commands.

This presents a set of problems. 

1. Dead Units 

One of the most frustrating problems I ran into were the memory errors caused whenever one of my units died. When a unit dies in BWAPI, it essentially becomes "null-ified" and stops existing. But of course, my groups of units are not aware that one of its members has died, and will attempt to access it. This of course, causes a memory error and crashes the bot. 

My ad-hoc solution to this was to create a function which takes in a list of `std::vector<Unit>`s and removes any dead units from the list. This function needed to be added to the beginning of essentially every function in every module which could potentially be the first function which accesses a certain list of units. It's a very inelegant and annoying solution I deemed acceptable due to the time constraints I had.  

2. Bad cohesiveness between modules 

A lot of times, I need to transition units into different roles. For example, when I rush out, I need SCVs to help with the attack. To do this requires (in my opinion) a needlessly complicated process of going into my Production module, finding an SCV from its list, removing it from the list, returning it to the main module, then adding it to the Command module which then adds it into its corresponding internal group(s). 

3. Inefficiency 

This implementation is of course, very inefficient. I constantly cycle through list after list, many of which contain the same units. Although this fact doesn't actually affect anything competition-wise, it still irks me on the inside. 

*Solution* 

Of course, there is still the basic need to differentiate between the different roles in your set of units. This, however, can be done instead by applying categories and tags to the units themselves. 

I plan on creating a new container class for every controllable unit, and this class will have a set of categories and tags which will identify the unit. 

The bot itself can still be modular, however the modules themselves will not own any units. Instead, each module will interact with units by having a function which can be passed in a unit container. The function will first identify if it has the correct tags before deciding to interact with it. 

That way, there should only be one big list of units which greatly simplifies and streamlines the unit control process. 

### Other issues to be solved

