# Federated Learning on Arduino Portenta H7: Training ML Models with Dual-Core architecture

In this project, the dual-core architecture of the Arduino Portenta H7 is used to capture samples using the M7 core and train the ML model with the M4 core. Then, a FL process can be triggered to merge all of the node's local models and update them.

## Dual-Core architecture
The preferred design was to use the M4 core for the audio capture, since the more powerful M7 core can better handle the ML tasks.

Unfortunately, when this repository was published, the audio capture library was designed to be used in the M7 core. An issue was created in the Arduino repository and it is now capable of doing so: https://github.com/arduino/ArduinoCore-mbed/issues/416#issuecomment-1065333895. This was successfully tested, but not implemented.

### Communication between cores
Both cores share a memory region (SRAM3), defined in DualCoreFL.ino. Through this shared memory the M7 core can instantly share the audio recording with the M4 core.

They also con communicate using the RPC (remote procedure call) protocol, being able to call a function from other core, even passing variables.

## Training
The training can be performed using a button to trigger the `record` function on the M7 core, but on automated experiments this won't generate reliable results.

Therefore, the preferred way to test this setup is using a Python server, which can be found on the fl_server.py.
This server can be used to automatically send the samples to the devices and perform FL between the nodes after the configured amount of samples.


## Set up
1. Copy the edge-impulse library for MFCC processing found in `libs/ei-kws` to the Arduino IDE libraries folder to install it.
2. To upload the code to the boards the Arduino IDE has to be used. Each core will require a different build to be uploaded.  
To upload the M4 core's code open the Arduino IDE, click on "Tools" / "Board: " / "Arduino MBed OS Portenta Boards / "Arduino Portenta H7 (M4 core)" and hit "Upload".  
Repeat this process selecting the "Arduino Portenta H7 (M7 core)" to upload the M7 core's code.
3. Start the Python server: `python ./fl_server.py`. Some Python packages may need to be installed.


## Future work
* The M4 core should be used to capture samples, and the M7 should handle all heavy-work ML tasks.
* The build process should be optimized, using Platform.io to upload the code for the M4 and the M7 cores.