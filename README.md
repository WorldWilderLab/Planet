# OrganicCinema

## About
Our focus for “Organic Cinema” was on how to get action & variation potential signals as our main source of data. The action and variation potentials present in plants can give us an insight into all the processes happening inside the plant. We can use these signals to see its reaction to its environment and witness certain actions as a response.  
In physiology, an action potential is a short-lasting event in which the electrical membrane potential of a cell rapidly rises and falls, following a consistent trajectory. In a graph these action potentials show up as small spikes. The scale of these potentials is in milliVolts so in order for us to be able to measure these we would need to amplify them.  
Our goal was to create such device so that anyone who is enthusiastic about it would be able to build, copy, improve and experiment with this device; it is easy to use and build; and it is affordable. In order to achieve this we decided to release every version of our device under an open hardware / open source licence, that will be well documented and easy to reproduce. We like to encourage everyone to play with it and possibly even improve upon it!  


## How to setup

### Arduino

* Using this arduino code requires you use an [ethernet shield](http://www.arduino.cc/en/Main/ArduinoEthernetShield).
* Copy the files in the `arduino/libraries` to `[path-to-sketchbook]/libraries`.
* Open `arduino/vocab_mqtt/vocab_mqtt.ino`.
* On line 32 it says: `const boolean USE_DHCP = false;`, if you want to connect the device directly to your computer or for any other reason use a manual ip address set this to `false`. If you want to connect the device to a network (router, hub, etc) you can set this `true`.
* On line 53 it says: `char server[]      = "192.168.1.2"; // "mqtt.lan";`, change that ip-address to where you have your server.
* On line 56 it says: `#define MQTTPREFIX   "/plant/10"`, this is where you set the mqtt address of this device.
* Now load the sketch to your arduino, stack the ethernet shield on the arduino and the organic cinema shield on the ethernet shield.
* install [mosquitto](http://mosquitto.org/) on your computer.


## License
This work by [World Wilder Lab](http://www.worldwilderlab.net/) is licensed under a [Creative Commons Attribution-ShareAlike 4.0 International License](http://creativecommons.org/licenses/by-sa/4.0/).
