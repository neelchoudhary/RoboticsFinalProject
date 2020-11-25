# 🤖 Microphone Plugin for mBot Ranger.
## Description
The [mBot ranger](https://www.makeblock.com/steam-kits/mbot-ranger) has a built in microphone that measures sound from the environment. This plugin mimics that microphone and is intended to be included in a model for the mBot ranger.
## Set up
0. The easiest way (and therefore the recommended set up process) is to clone [this repo](https://github.com/nbock/cs5335-nm/tree/plugins), make, and execute world.sh. If you do that and see "hello from MicControlPlugin" on Gazebo start up, skip to Setting up sound sources.
1. Make a world. This plugin has an [example world](https://github.com/nbock/cs5335-nm/blob/plugins/worlds/project.world) using the microphone and a tankbot. There is also a [startup script](https://github.com/nbock/cs5335-nm/blob/plugins/world.sh) for that world.\
    a. If you want to use the example world, it's probably easiest to clone the entire repo and make from the parent directory. Then you can call `./world.sh` and the world will load.

2. If you chose to make your own world, load your models for a car or tankbot or whatever vehicle into the world.

3. Now, attach the microphone model to the tankbot.\
    a. To do this, you'll need to include the microphone sensor model in the tankbot model.\
    b. The [microphone model](https://github.com/nbock/cs5335-nm/tree/plugins/models/microphone_sensor) is very simple, but will need to be added to your models directory. Add the entire microphone_sensor directory.\
    b. An example of this is done in the [example world](https://github.com/nbock/cs5335-nm/blob/plugins/worlds/project.world)\
    c. You should add the following lines into your vehicle model (not the world model):
    ```xml
    <include>
      <uri>model://microphone_sensor</uri> <!--change this to your model directory-->
      <pose>
        0.0 0.0 0.23
        0.0 0.0 0.0
      </pose>
    </include>

    <joint name="microphone_joint" type="fixed">
      <child>microphone_sensor::link</child>
      <parent>chassis</parent>
    </joint>
    ```
    **NOTE**: you'll want to edit the pose of your microphone_sensor to sit atop your vehicle model. The example pose is for the tankbot in world.sh.\
    **NOTE**: The naming convention for your models matters and will influence what the topic names you're using are. Mimic the examples as close as possible.

4. Make sure you followed the second note above, it's super important (this is just a reminder)
5. Edit your Makefile. Your Makefile should mimic the one included in [plugins](../).\
    a. You need to create a .so for your plugin. The important lines to have for the microphone plugin of [this makefile](https://github.com/nbock/cs5335-nm/blob/plugins/plugins/Makefile) are lines 4, 11-13, and 18.\
    b. You'll also need a Makefile for your parent directory, which can mimic [this one](https://github.com/nbock/cs5335-nm/blob/plugins/Makefile).

6. If you've done all this, you should be ready to initialize your world and see the microphone plugin.

## Setting up sound sources
1. Now we need to set up sound sources. That is done on lines 17-19 on [microphone_control.cc](https://github.com/nbock/cs5335-nm/blob/plugins/plugins/microphone_control/microphone_control.cc)
```cpp
const int NUM_SOURCES = 2;
int SOURCES[][NUM_SOURCES] = {{3, 3}, {-3, -3}};
int SOURCES_DECIBELS[NUM_SOURCES] = {60, 60};
```
2. NUM_SOURCES dicates how many sound sources you have in your world
3. SOURCES is a 2D array of the (x, y) location of your sound sources
4. SOURCES_DECIBELS is an array of the decibels of each sound source
5. Edit all 3 of the global variables to add source sources to your world


## Subscribing from robot.cc (or something like it)
0. Check out [robot.cc](https://github.com/nbock/cs5335-nm/blob/plugins/brain/robot.cc) and [robot.hh](https://github.com/nbock/cs5335-nm/blob/plugins/brain/robot.hh) for examples on how this is done.\
    a. [brain.cc](https://github.com/nbock/cs5335-nm/blob/plugins/brain/brain.cc) also has a reference to `robot->noise`
1. Add an integer `noise` value to robot.hh or your equivalent
2. Add a SubscriberPtr as a private variable to robot.hh or equivalent
3. Use your SubscriberPtr to subscribe to the mic topic `~/tankbot0/mic`:
```cpp
mic_sub = node->Subscribe(
    string("~/tankbot0/mic"),
    &Robot::on_sound,
    this,
    false
);
```
4. Now, `Robot->on_sound` will be called on each message that is published to the mic topic
5. Store the noise data from your on_sound function
```cpp
void
Robot::on_sound(ConstIntPtr &msg)
{

    this->noise = msg->data();
    this->on_update(this);
}
```
6. Now you can reference the noise from `Robot->noise`

## Running it
1. If you pull the entire cs5335-nm branch called "plugin" you can make and `./world.sh` will start the world
2. Then executing `./brain` will move the bot in a boring way, but you'll see noise change as you get closer to (3, 3) or (-3, 3)

## Implementation details
1. Sound intensity in the plugin degrades with distance according to the inverse square law. This is also how sound intensity degrades in the real world, but if you want to change that for some reason, the code responsible for the degredation is in make_mic_message:
```cpp
// decibel intensity degrade by the inverse square law
double distance = pow(dist, 2);
double denom = clamp(1.0, dist, 1000);
double intensity = 1 / denom;
double decibels_float = source_decibels * intensity;
```
2. The microphone plugin subscribes to world_stats and calls its OnStats each time a message is published to world_stats.
3. Distance is measured in meters.
4. The command below is useful for seeing what data is being published to the microphone topic. Substitute "tankbot" for whatever model your sound sensor is embedded within:
```terminal
gz topic -e /gazebo/default/tankbot0/mic
```
5. If you aren't sure what topic your mic is on, use the following to list all topics:
```terminal
gz topic -l
```
6. The "dominant" sound wins, so if you have two sounds, the louder one _at that point_ is what contributes to noise.

## Fitting this to your world
In the example world, each (x, y) grid space is treated as 1 meter by 1 meter by this version of the microphone plugin. This means a sound source at (3, 3) is barely audible at 60 decibels from the origin (0, 0). That makes sense if you consider that 60 decibels is a normal conversation volume and that conversation is 4.24 meters away (about 14 feet). In real life, you'd expect to hear the noises, but not that loudly.

Changing the the distance parameters is pretty easy. For example, if you want to make each (x, y) grid space 0.33 meters, just divide dist by 3 in the return statement on line 72 of [microphone_control](https://github.com/nbock/cs5335-nm/blob/plugins/plugins/microphone_control/microphone_control.cc):
```cpp  
return dist / 3.0;
```

You'll run into trouble with this if you set each grid value to be <= 1/6th of a meter in each direction. This is because intensity depends inversely on the distance squared, so the distance square must be at least 1:
```cpp
// decibel intensity degrade by the inverse square law
double distance = pow(dist, 2);
double denom = clamp(1.0, dist, 1000);
double intensity = 1 / denom; // if denom < 1, intensity increases, which makes no sense
double decibels_float = source_decibels * intensity; // if intensity > 1, decibels are louder than the output decibels at source (also makes no sense)
```

All this is to say, you can easily change how far away the microphone considers its sources, but the microphone plugin won't work well on worlds that are discretized into spaces with grid values (x, y) that represent a space less than 1/6th by 1/6th of a meter. I think this is a reasonable constraint given that that would be an (x, y) grid that is approx. 6 inches by 6 inches.
