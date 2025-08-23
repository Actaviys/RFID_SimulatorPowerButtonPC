### "RFID_SimulatorPowerButtonPC" - this is a power button simulator for a computer.
\
The project was created for the board `Arduino Nano328P`, and the module `RFID-RC522`. The demonstrated example simulates pressing the computer's power button as if it were pressed manually. \
The example uses a low-voltage relay `Axicom MT2 C93416` to close the corresponding contacts that control the computer's power. Don't forget to use a diode to protect the Arduino pins. \
Two buttons are used to save and delete RFID tag records. A dual LED with common anode is also used to indicate appropriate actions. \
Please note that my example uses a specific connection buttons and LED. This is made specifically for my specific mounting of components in the appropriate place. \
\
The following is an example for connecting `RFID-RC522` to `Arduino Nano328P`: \
RFID    -->       Arduino \
    SDA    ->          D10 \
    SCK    ->          D13 \
    MOSI   ->          D11 \
    MISO   ->          D12 \
    vcc    -x \
    GNG    ->          GND \
    RST    ->          D9 \
    VCC    ->          3.3v \

We connect the buttons and LED to their own pins, the example uses a specific connection!

Power for Arduino must be taken directly from the power supply. To do this, you need regular to find 5V on the main 24-pin connector of the power supply, and use it as a power supply for the board `Arduino`.