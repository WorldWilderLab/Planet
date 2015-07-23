# PlanEt

## About
Through PlanEt we can detect the electrical discharge from ions present in the water intake by plants, distributed in the apoplast - that is the space between plants cell walls. That water is not absorbed by the cells themselves, and its presence in-between cells is a result of plant’s self-regulating how much water and nutrients it needs. The apoplast itself plays a major role in a diverse range of processes, including intercellular signalling, plant–microbe interactions and both water and nutrient transport. Signals acquired with PlanEt are a result of small changes in conductance through the apoplast and that reflects changes in ion concentrations in the apoplast. There are any number of reasons why such changes might occur, including (but not limited to) pathogen defense, water and/or mineral stress, normal developmental events, cell death, circadian and/or diurnal rhythms which could be connected to photosynthesis.
The signals - data which we get from ions in the apoplast - can inform us about various plant’s reactions to its environment. For example the leaf apoplast connects the plant with the atmosphere. Together with the atmospheric conditions and conditions in the leaf apoplast, it can determine the exchange processes in either direction, informing the plant about its environment.

Our goal was to create such device so that anyone who is enthusiastic about it would be able to build, copy, improve and experiment with this device; it is easy to use and build; and it is affordable. In order to achieve this we decided to release every version of our device under an open hardware / open source licence, that will be well documented and easy to reproduce. We like to encourage everyone to play with it and possibly even improve upon it!

## How to setup

### Shield

* In the `kicad` folder are all the files needed for you to reproduce the shield.
* You can find the schematic [here](https://github.com/WorldWilderLab/Planet/blob/master/kicad/Vocab.pdf).
* You can find the partslist [here](https://github.com/WorldWilderLab/Planet/blob/master/kicad/partslist.csv).

### Arduino

* Using this arduino code requires you use an [ethernet shield](http://www.arduino.cc/en/Main/ArduinoEthernetShield).
* Copy the files in `arduino/libraries` to `[path-to-sketchbook]/libraries`.
* Open `arduino/vocab_mqtt/vocab_mqtt.ino`.
* On line 32 it says: `const boolean USE_DHCP = false;`, if you want to connect the device directly to your computer or for any other reason use a manual ip address set this to `false`. If you want to connect the device to a network (router, hub, etc) you can set this to `true`.
* On line 53 it says: `char server[]      = "192.168.1.2"; // "mqtt.lan";`, change that ip-address to where you have your [MQTT](http://mqtt.org/) broker.
* On line 56 it says: `#define MQTTPREFIX   "/plant/10"`, this is where you set the MQTT address of this device.
* Now load the sketch to your arduino, stack the ethernet shield on the arduino and the organic cinema shield on the ethernet shield.
* install [mosquitto](http://mosquitto.org/) on your computer.

### Scripts

* In the `scripts` folder you will find scripts we use for logging data.
* Please read the readme in the `scripts` folder for more info on usage.

## License
This work by [World Wilder Lab](http://www.worldwilderlab.net/) is licensed under a [Creative Commons Attribution-ShareAlike 4.0 International License](http://creativecommons.org/licenses/by-sa/4.0/).
