# Remote Controlled EDF Pinewood Derby Car
![Alt text](media/car.jpg?raw=true "Car")

I made an "outlaw" pinewood derby car for fun but also was hoping to spark an interest in engineering for a few young minds as well.

It's not much to look at admittedly but speed was the primary goal. I wanted the car to turn on and off by a remote because local rules don't allow a fork on the front of the car that would be required for an infared beam switch. The EDF makes enough thrust at full power to take off vertically, so I slightly angled the fan downward. Depending on the speed settings in the code, it can be as fast as possible without getting into CO2. The build cost around $50 with parts sourced from China. I used cheapish hobby components with reserve capacity to hopefully mitigate failures.

This code is designed for the newer [Adafruit Trinket M0](https://learn.adafruit.com/adafruit-trinket-m0-circuitpython-arduino/overview), which uses a different processor from the original [Adafruit Trinket](https://learn.adafruit.com/introducing-trinket/introduction) used by the forked repository.

My goal here is to provide enough instructions and pictures to build a similar car yourself.

## Video
Running at power level 100 (max is 180, so 56% power) with a burst of 1.1 sec, it acheived a speed of 1.995 sec on a track where the top non-outlaw cars reached times of 3.2 sec.
[![EDF POWERED PINEWOOD DERBY CAR](media/video_thumbnail.png?raw=true)](https://youtu.be/WiaqjgjA2JI "YouTube video")

## Controls Overview
The remote control has 4 buttons: A, B, C, & D. When A is pressed, the car goes into "staged" mode where the fan runs at a low speed appropriate for in the starting block. Then as soon as the race starts, you can press either B, C, or D to execute a burst of the fan at a low, medium, or high speed respectively. The car will turn off after the burst. Alternatively, staged mode can be turned off by pressing A again.

Note that the cheap remote and 433 MHz RF receiver used here are not individually paired, so multiple remotes or multiple receivers can interfere with each other. I chose an RF receiver instead of an infrared so that it doesn't need line of sight.

## Parts List
Here are the essential parts:
| Description                                    | Price  |
| ---------------------------------------------- | ------:|
| XFly Galaxy X4 40mm EDF (fan)                  | $20 |
| 30A ESC 5V/3A BEC with XT60 plug               | $4 |
| Turnigy NanoTech Plus 1300mAh 4S 70C/140C LIPO | $16 |
| QIACHIP RX480E with antenna and remote         | $5 |
| Adafruit Trinket M0                            | $7 |
| **Total**                                      | **$52** |

Other models are available and compatible. These are just what I used. In particular, there are other receivers that aren't programmable/versatile but should work; you want one that supports "Momentary" mode, sometimes called M4 receivers. The [XFly fan](http://www.xfly-model.com/productinfo/1068559.html) supports both 3S and 4S (3 or 4 cell) batteries with 3S giving up to 360g of thrust and the 4S giving 500g.

The battery is larger than I need (physically, and at 1300mAh), but that allowed me to do many test runs between charges and probably helps with inertia and potential energy during the race.

Technically, according to the EDF specifications you only need a 20A ESC, but I went with 30A for no good reason.

There are some other components that you may need as well if you don't already own them:
| Description                         | Price  |
| ----------------------------------- | ------:|
| iMAX B6 80W Charger with XT60 plug  | $19 |
| Soldering iron                      | $25 |
| Solder for electronics 60/40 0.03in | $15 |
| Dupont wires 10cm F-F and M-F       | $8 |

The Dupont wires allow you to avoid soldering the wires onto the pins of the RF receiver and Trinket M0 if you don't trust your soldering abilities. Alternatively, you can use 7 colors of 22 or 24 gauge wire and solder them.

## Code
The code can be found in [ducted-fan-pinewood-derby.ino](ducted-fan-pinewood-derby.ino). The key parameters to experiment with are `STAGED_SPEED`, `LOW_SPEED` (button B), `MEDIUM_SPEED` (button C), `HIGH_SPEED` (button D) that range from 0-180 so you can adjust them depending on your car design and desired speed. (A 4S battery may provide too much thrust at max speed depending on your car design.) Also experiment with `RACE_DURATION` that specifies the length of time (milliseconds) of the speed burst before the fan turns off.

Library dependencies:
* [Adafruit SoftServo](https://github.com/adafruit/Adafruit_SoftServo) version 1.0.4 (latest as of time of writing)
* [Adafruit ZeroTimer](https://github.com/adafruit/Adafruit_ZeroTimer) version 2.2.4 (latest as of time of writing)

These can both be installed through the Arduino IDE library.

## Wiring Diagram
The wiring diagram is below. Some notes:
* To make it easier to build and test, I [soldered the header pins](https://youtu.be/6rmErwU5E-k?t=316) that come with the Trinket M0 onto the chip. That allowed me to put it into the breadboard for prototyping, but it makes it bigger than absolutely necessary.
* The buttons A, B, C, and D on the remote trigger D0 through D3 on the receiver, which are wired to pins 1-4 on the Trinket M0 using F-F Dupont wires.
* Trinket M0 pin 0 is wired to the white cable to the ESC's female connector with a F-M Dupont wire.
* Both the receiver and Trinket need to connect to the 5V source and ground from the ESC. To support this, I cut the ends off of two Dupont wires leaving a female connection on each. I then stripped the cut ends, twisted them together, [tinned them](https://youtu.be/6rmErwU5E-k?t=409), and inserted them into the female connector coming from the ESC.
* The ESC and battery are connected with XT60 connections that they came with.
* The ESC and EDF (fan) are connected using the 3 wires and the connections they come with. If the fan blows in the wrong direction, switch any two.

![Wiring Diagram](media/wiring_diagram.png?raw=true "Wiring Diagram")

The QIACHIP RX480E receiver has multiple modes that are selected by pushing the button on the reciever while it has power. You want the "Momentary" mode. Refer to the receiver instructions for how to set the mode.

I used a 4S (4-cell) battery so when the power is connected, the fan beeps 4 times and then gives a long beep.

## Additional Pictures
The two chips after I soldered the header pins on the Trinket M0:

![Chips](media/chips.jpg?raw=true "Chips")

I prototyped the circuit on a breadboard to check the connections and programming. This included adding some LEDs, resistors (to not burn out the LEDs), and ground connections to each of the receiver output pins so I could confirm that the receiver was receiving each button press. The LEDs, resistors, and extra ground wire were not included in the built car.

![Breadboard1](media/breadboard1.jpg?raw=true "Breadboard 1")
![Breadboard2](media/breadboard2.jpg?raw=true "Breadboard 2")

The car body includes a pocket in the back for the Trinket M0. A pedestal on the front is carved out to hold the fan, is slightly angled down in the front to help keep the car on the track, the very front has a lip carved out for the fan's rim, and a hole drilled through will enable a string to ensure the fan stays with the car if damaged.

![Wood](media/wood.jpg?raw=true "Wood")

I attached the electronics with duct tape. The Dupont wires can be seen here and really saved me soldering time and frustration. The Trinket M0 is positioned so that I can access the USB port from the back for easy reprogramming as necessary. The receiver is positioned with the antenna rotated out a bit to get a clear signal. A string attaches the fan to the car for safety. Be sure not to tie the string too tight or apply the duct tape too tight to the fan so that the outside of the fan does not squeeze in to touch the fan blades. You can see the slight downward angle of the fan on the side image.

![With Electronics](media/with_electronics_top.jpg?raw=true "With Electronics, Top")
![With Electronics](media/with_electronics_side.jpg?raw=true "With Electronics, Side")

After screwing on the wood on the back, some more duct tape, and adding the wheels, the car is finished. It's not pretty, but it's fast.

![Finished1](media/finished_car1.jpg?raw=true "Finished Car")
![Finished2](media/finished_car2.jpg?raw=true "Finished Car")
![Finished3](media/car.jpg?raw=true "Finished Car")

Racing the car down my hallway, it had a tendancy to turn right at high speeds. However, it stayed on the track when the rails were there to guide it.

After racing the car as shown in the video, it broke in two parts at the front axel slot. The side image above shows a line down the length of the car at the top of the axel slots, which is where it broke. I wonder if the wood was partially defective from the start.

![Aftermath](media/aftermath.jpg?raw=true "Aftermath")

I would definitely change two things in the future: I would make the bottom of the car thicker and I would catch it with a pillow instead of my hands. I may also redesign the car so that the battery sits lower and the electronics are on top, for better weight distribution and aerodynamics so the fan does not blow on the battery.