# 20 Speaker Relay Wiring

This setup uses the computer to play the audio and Arduino Mega to switch each amplified speaker module on or off.

## Audio signal

Do not send the AUX audio through the relay.

Use the computer headphone output or a USB sound card:

```text
Computer AUX L -- 1k resistor --+
                                +-- all speaker module IN pins
Computer AUX R -- 1k resistor --+

Computer AUX GND ------------------ all speaker module GND pins
```

## Speaker module power

The relays switch each speaker module VCC.

```text
5V power supply + -> relay COM
relay NO        -> speaker module VCC
5V power supply - -> speaker module GND
```

All grounds should be connected together:

```text
5V power supply -
Arduino GND
relay GND
speaker module GND
computer AUX GND
```

Use a separate 5V power supply for the speaker modules. For 20 small amplified speaker modules, start with 5V 10A or higher.

## Arduino Mega relay mapping

Unity sends speaker numbers 1-20. Arduino maps them to Mega pins. The first four are set for your current test wiring:

```text
Speaker 01 -> Mega D2
Speaker 02 -> Mega D3
Speaker 03 -> Mega D4
Speaker 04 -> Mega D5
Speaker 05 -> Mega D22
Speaker 06 -> Mega D23
Speaker 07 -> Mega D24
Speaker 08 -> Mega D25
Speaker 09 -> Mega D26
Speaker 10 -> Mega D27
Speaker 11 -> Mega D28
Speaker 12 -> Mega D29
Speaker 13 -> Mega D30
Speaker 14 -> Mega D31
Speaker 15 -> Mega D32
Speaker 16 -> Mega D33
Speaker 17 -> Mega D34
Speaker 18 -> Mega D35
Speaker 19 -> Mega D36
Speaker 20 -> Mega D37
```

If your relay module turns on when the control pin is LOW, keep this in `SpeakerSequencer.ino`:

```cpp
const bool RelayActiveLow = true;
```

If it turns on when the control pin is HIGH, change it to:

```cpp
const bool RelayActiveLow = false;
```
