# Gravity Well

An asteroid field game for the TM4C123 microcontroller (ARM Cortex-M4).
Written as a final project for EE 319K (Intro to Embedded Systems) at UT.

# Features implemented:

* *Interrupts* - Edge-triggered GPIO interrupts for input during game, SysTick interrupts for timekeeping, Timer0A and Timer0B interrupts for sound.
* *Double buffering* - Extra buffer in RAM with one byte per pixel. The game is updated every tick, when all the entities are updated.
* *Buttons* - The on-board buttons PF0 and PF4 are used.
* *Sprites* - The ship, asteroids and items are rendered as sprites.
* *Potentiometer* - The potentiometer is sampled each tick to calculate the force by the ship's thrusters.
* *Sound*
* *Score display* - When the level ends, the player's score will be shown.
