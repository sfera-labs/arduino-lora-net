# LoRaNet - Arduino

LoRaNet is a framework for implementing a master-slave architecture using a secure communication protocol on top of LoRa-MAC.

This repo contains the implementation of LoRaNet for Arduino MKR-based modules.

## Architecture

The LoRaNet network comprises one master unit and several slave units. Each slave unit has a unique address ranging from 1 to 250; the master has address 0.

Slave units send periodical state updates to the master, which, in turn, sends commands to the slave units.

Messages are encrypted using a pre-shared key. An initial handshake sequence between slave units and master is used to exchange a session key and a sequence counter to protect against replay attacks.

Messages between nodes of the same LoRaNet network are prepended with a pre-shared site-ID. This reduces the possibility of conflicts between separate LoRaNet networks, using the same LoRa radio frequency, in the same area.

## Usage

This library depends on the [arduino-LoRa library](https://github.com/sfera-labs/arduino-LoRa).

Implementations of LoRaNet shall extend the classes `LoRaLocalSlave` and `LoRaRemoteSlave`.

For an example of implementation and usage see [IonoLoRaNet](https://github.com/sfera-labs/iono/tree/master/IonoLoRaNet) which implements LoRaNet for [Iono MKR](https://www.sferalabs.cc/iono-mkr/).

## Protocol details

Nodes of the same LoRaNet network are initialized with the same key (for encrypting messages' content) and the same site-ID.

The site-ID is added to the beginning of each message, not encrypted, so that received messages which do not start with this ID can be promptly discarded.

Messages' content is encrypted using AES with a 128-bits key.

Each exchanged message contains a session-key and an incremental counter.

Communication between master and slaves begins with a handshake sequence to agree on session-key and counters. At startup each node (slaves or master) attempts to initiate the sequence.

In the following, the node successfully initiating the sequence is referred to as N1 and the one receiving the initial message as N2.

N1 sends a message containing a random session-key.

N2 responds with a message containing the received session-key and a challenge counter.     
This counter starts from zero if no previous session was established between the two nodes, otherwise it is set to the last counter value received by that node, increased by one.     
This avoids replay attacks by guaranteeing the uniqueness of the encrypted content.     

N1 checks that the received session-key corresponds to the sent one, if so, sets its counter to the value of the received challenge counter and responds.

N2 checks session-key and counter and, if correct, responds to signal the acceptance of these parameters, incrementing its counter.

N1 checks again session-key and counter and saves them.

From this point the agreed session-key is used for subsequent messages (state updates from slaves, commands from master, and acknowledgments). Each time a node sends a new message increments the counter and the receiver checks that the counter is greater than the previously received one.

The counter is a 2-bytes value, when the maximum value is reached, the handshake sequence is repeated to agree on a new session-key.

The content of state updates and commands is implementation-specific.

## Duty cycle constraints

Each LoRa frequency allows for a maximum duty cycle.

The method `LoRaNet.setDutyCycle(unsigned long windowSeconds, int permillage)` lets you set the time period over which the duty cycle is calculated (max 1 hour) and the maximum permillage (1/1000) value you want the device to use.

Make sure to set a duty cycle no higher than the allowed one for the used frequency.

When the specified duty cycle is exceeded the node will stop sending LoRa messages until the end of the current duty cycle window.
