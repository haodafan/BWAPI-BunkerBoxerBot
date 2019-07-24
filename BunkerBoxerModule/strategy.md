# Bunker Boxer Bot Overall Strategy 

The Bunker Boxer Bot is a Bot that attempts to imitate the famous bunker rushes by SlayerS_BoxeR. 

The main idea is to accumulate an army of SCVs and marines and rush the enemy early on. Upon arrival at the enemy base, an SCV will construct a bunker and attack/harass the enemy using it. 

Much of our strategy will be based on this guide: https://liquipedia.net/starcraft/Bunker_Rush_(vs._Zerg)

## Build Order 

A Bunker Rush is normally done with an 8 Rax build order: 

- 8/10 - Barracks 
- 9/10 - Supply Depot 
- 10/18 - Scout (more on that later)
- @Barracks 100% - Rally on Zerg's Natural and build marines 
- 15/18 - Supply Depot. 

## Scouting 

At 10 supply, an SCV will be designated as the 'Scout'. The Scout's main purpose is to find the location of the enemy base. 

This can be done with the help of the terrain analyzer tool. 

Once the enemy has been found, the Scout then looks for the base location closest to the enemy. This is designated as the 'Natural'.

At this point the Scout has completed its duties. 

## The Attack 

An attack will begin once we have accumulated 4 marines. The initial attacking force is composed of 4 marines and 8 SCVs. 

The attacking force will first move towards the Natural. If the Natural has been taken by the enemy, then we will designate that as the Bunker fight location. If it hasn't then, we will designate the main base as the Bunker Fight Location.

## Tactics and Micro 

There are two types of stances (i.e. AI scripts) that can take place.  

If the bunker has not yet been built, then we will adopt a **bunker defense** stance. 

If the bunker has been built, and we see that there are offensive enemy units, then we will adopt the **bunker defense** stance.

If the bunker has been built, and we see no offensive enemy units, then we will adopt the **aggressive** stance. 

### Bunker Defence Stance 

The Bunker Defence Stance is one where the objective of all units is to protect the bunker. 

- All units must stay a certain distance away from the bunker 

- As many marines as possible will garrison the bunker. 

- Four SCVs are designated as **repairmen** who will repair the bunker if its damaged. 

- Marines move back if zerglings get too close 

The Bunker Defence Stance will also have an **Idleness** timer. If the enemy chooses not to attack for a certain amount of time, all ungarrisonned marines will move forward to attack and then come back. 

### Aggressive Stance 

The Aggressive stance seeks to destroy the enemy buildings. Any marines garrissoned in the bunker will exit and everyone attacks. 

# Strategic Execution 

## Structure 

The structure of this program is loosely based on that of UAlbertaBot. It is vastly simplified as it only executes a single and simple rush strategy. 

## Logical Flow: 

BunkerBoxerModule.OnFrame(): 
- ProductionManager.update() 
- - Send idle workers to gather minerals
- - Re-allocate workers to new mineral patches if mined out 
- - Execute build order 
- - If build order is executed, then continuously build marines and supply depots. 
- Intelligence.update() 
- - Intelligence is automatically set to passive. If it is passive then it doesn't do anything.
- - If it is active, then it takes one unit which it designated as the 'Scout'. 
- - - If we know where the enemy base is, then we go towards it. 
- - - If the scout dies, then intelligence is set to passive again. 
- FieldCommander.update() 
- - Field commander has four main states: passive, travelling, bunker defence stance, and aggressive. 
