#include <stdlib.h>
#include <string.h>

const long BaudRate = 115200;
const size_t CommandBufferSize = 128;
const size_t SpeakerCount = 20;
const size_t MaxActivePins = SpeakerCount;
const bool RelayActiveLow = true;
const bool SelfTestOnStartup = true;
const size_t SelfTestRelayCount = 4;
const unsigned long SelfTestStepMs = 250;
const int RelayPins[SpeakerCount] = {
    2, 3, 4, 5,
    22, 23, 24, 25,
    26, 27, 28, 29,
    30, 31, 32, 33,
    34, 35, 36, 37
};

char commandBuffer[CommandBufferSize];
size_t commandLength = 0;
int activePins[MaxActivePins];
size_t activePinCount = 0;
unsigned long activeStopAtMs = 0;

void setup()
{
    for (size_t index = 0; index < SpeakerCount; index++)
    {
        pinMode(RelayPins[index], OUTPUT);
        setRelay(RelayPins[index], false);
    }

    Serial.begin(BaudRate);
    Serial.println("READY");

    if (SelfTestOnStartup)
    {
        runRelaySelfTest(SelfTestRelayCount);
    }
}

void loop()
{
    while (Serial.available() > 0)
    {
        char character = (char)Serial.read();
        if (character == '\n' || character == '\r')
        {
            if (commandLength > 0)
            {
                commandBuffer[commandLength] = '\0';
                handleCommand(commandBuffer);
                commandLength = 0;
            }
        }
        else if (commandLength < CommandBufferSize - 1)
        {
            commandBuffer[commandLength] = character;
            commandLength++;
        }
        else
        {
            commandLength = 0;
            Serial.println("ERR OVERFLOW");
        }
    }

    updateActiveSpeaker();
}

void handleCommand(char *line)
{
    char *command = strtok(line, " ");
    if (command == NULL)
    {
        return;
    }

    if (strcmp(command, "PING") == 0)
    {
        Serial.println("PONG");
        return;
    }

    if (strcmp(command, "TEST") == 0)
    {
        runRelaySelfTest(SelfTestRelayCount);
        Serial.println("OK TEST");
        return;
    }

    if (strcmp(command, "STOP") == 0)
    {
        stopActiveSpeaker();
        Serial.println("OK STOP");
        return;
    }

    if (strcmp(command, "GROUP") != 0)
    {
        Serial.println("ERR UNKNOWN");
        return;
    }

    char *durationToken = strtok(NULL, " ");
    if (durationToken == NULL)
    {
        Serial.println("ERR FORMAT");
        return;
    }

    unsigned long durationMs = strtoul(durationToken, NULL, 10);
    if (durationMs == 0)
    {
        Serial.println("ERR VALUE");
        return;
    }

    int relayPins[MaxActivePins];
    size_t relayPinCount = 0;
    char *speakerToken = strtok(NULL, " ");
    while (speakerToken != NULL && relayPinCount < MaxActivePins)
    {
        int speakerNumber = atoi(speakerToken);
        if (speakerNumber < 1 || speakerNumber > (int)SpeakerCount)
        {
            Serial.println("ERR VALUE");
            return;
        }

        relayPins[relayPinCount] = RelayPins[speakerNumber - 1];
        relayPinCount++;
        speakerToken = strtok(NULL, " ");
    }

    if (relayPinCount == 0)
    {
        Serial.println("ERR FORMAT");
        return;
    }

    playSpeakerGroup(relayPins, relayPinCount, durationMs);
    Serial.print("OK GROUP ");
    Serial.println(relayPinCount);
}

void playSpeakerGroup(int pins[], size_t pinCount, unsigned long durationMs)
{
    stopActiveSpeaker();

    for (size_t index = 0; index < pinCount; index++)
    {
        int speakerPin = pins[index];
        pinMode(speakerPin, OUTPUT);
        setRelay(speakerPin, true);
        activePins[index] = speakerPin;
    }

    activePinCount = pinCount;
    activeStopAtMs = millis() + durationMs;
}

void updateActiveSpeaker()
{
    if (activePinCount > 0 && (long)(millis() - activeStopAtMs) >= 0)
    {
        stopActiveSpeaker();
    }
}

void stopActiveSpeaker()
{
    for (size_t index = 0; index < activePinCount; index++)
    {
        setRelay(activePins[index], false);
    }

    activePinCount = 0;
    activeStopAtMs = 0;
}

void setRelay(int relayPin, bool enabled)
{
    bool outputHigh = RelayActiveLow ? !enabled : enabled;
    digitalWrite(relayPin, outputHigh ? HIGH : LOW);
}

void runRelaySelfTest(size_t relayCount)
{
    if (relayCount > SpeakerCount)
    {
        relayCount = SpeakerCount;
    }

    stopActiveSpeaker();
    for (size_t index = 0; index < relayCount; index++)
    {
        setRelay(RelayPins[index], true);
        delay(SelfTestStepMs);
        setRelay(RelayPins[index], false);
        delay(SelfTestStepMs);
    }
}
