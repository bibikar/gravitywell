## EE319K - Introduction to Embedded Systems - Lab 10

Find the lab description at link below :

https://docs.google.com/document/d/1s3uQowkEjTSlR0_45svptQb7hqS1_2NCD9HRzErFIx0/edit

# Features implemented:

* *Interrupts* - Edge-triggered GPIO interrupts for input during game, SysTick interrupts for timekeeping, Timer0A and Timer0B interrupts for sound.
* *Double buffering* - Extra buffer in RAM with one byte per pixel. The game is updated every tick, when all the entities are updated.
* *Buttons* - The on-board buttons PF0 and PF4 are used.
* *Sprites* - The ship, asteroids and items are rendered as sprites.
* *Potentiometer* - The potentiometer is sampled each tick to calculate the force by the ship's thrusters.
* *Sound*
* *Score display* - When the level ends, the player's score will be shown.
