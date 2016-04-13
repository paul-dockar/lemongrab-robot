# README #

### How do I get set up? ###
* Click clone, use sourcetree and choose a directory. MP-LAB X should be able to open the project.

### Who do I talk to? ###
* Talk to Paul for help with git/sourcetree :)

### Requirements ###
You will be required to write source code for the PIC 16F877A micro-controller to:

* Flash a heartbeat LED at 1Hz
* Continuously read the IR sensor and display the range in cm on the LCD
* On a button press, scan the IR sensor CW through 360 degrees taking readings at each half step. At the conclusion of the scan, rotate CCW to point the sensor towards the closest obstacle.
* On a button press, drive iRobot Create 4m in a straight line (straight line manoeuvre) while continuously displaying the total linear distance travelled on the LCD since the button press.
* On a button press, drive the iRobot Create in a “square” shaped trajectory (square-shaped manoeuvre) with 1m each side (taking either left or right turns) while continuously
displaying the total linear distance travelled on the LCD since the button press.
* On a button press, scan the IR sensor CCW to find the wall closest to it. Drive the iRobot Create along the wall maintaining a constant distance (without colliding with the wall) and stopping when a bump sensor or cliff sensor is triggered. The iRobot needs to frequently check and adjust its distance from the wall. At the start of the wall following