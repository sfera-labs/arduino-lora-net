#include "LoRaNet.h"
#include <LoRa.h>
#include "CRC.h"

/*******************************************************************************
/** Network layer
/******************************************************************************/

#define _MSG_RST_1 0
#define _MSG_RST_2 1
#define _MSG_RST_3 2
#define _MSG_RST_4 3

LoRaNetClass::LoRaNetClass() {
}

void LoRaNetClass::init(byte *siteId, int siteIdLen, byte *cryptoKey) {
  _site_id = (byte*) malloc(siteIdLen);
  _site_id_len = siteIdLen;
  memcpy(_site_id, siteId, siteIdLen);
  memcpy(_crypto_key, cryptoKey, 16);
  _reset_last = millis();
  _reset_intvl = 0;

  // init random
  LoRa.parsePacket();
  uint32_t seed = 0;
  for (int i = 0; i < 32; i++) {
    seed <<= 1;
    seed |= LoRa.random() & 1;
  }
  randomSeed(seed);
}

void LoRaNetClass::setLocalAddr(int address) {
  _unit_addr = address;
}

void LoRaNetClass::setNodes(Node *nodes, int numOfNodes) {
  _nodes = nodes;
  _nodes_size = numOfNodes;
}

void LoRaNetClass::process() {
  _reset();
  _recv();
}

void LoRaNetClass::_reset() {
  unsigned long now = millis();

  if (now - _reset_last >= _reset_intvl) {
    _reset_last = now;

    for (int i = 0; i < _nodes_size; i++) {
      Node &node = _nodes[i];
      if (node._reset_intvl >= 0 && now - node._reset_last >= node._reset_intvl) {
        Serial.print("_reset ");
        Serial.println(node.getAddr());
        node._reset_last = now;
        node._reset_intvl = node._reset_trial * 5000 + random(5000);
        if (node._reset_trial < 30) {
          node._reset_trial++;
        }

        _reset_intvl = 5000;

        for (int i = 0; i < 8; i++) {
          node._reset_session[i] = random(0x100);
        }

        _send_with_session(node, node._reset_session, _MSG_RST_1, NULL, 0);
        return;
      }
    }

    _reset_intvl = 2000;
  }
}

bool LoRaNetClass::_send(Node &to, byte msg_type, byte *data, int data_len) {
  if (!to._session_set) {
    Serial.println("Send error: no session");
    return false;
  }
  return _send_with_session(to, to._session, msg_type, data, data_len);
}

bool LoRaNetClass::_send_with_session(Node &to, byte *session, byte msg_type, byte *data, int data_len) {
  Serial.println("_send ====");

  Serial.print("to: ");
  Serial.print(to.getAddr());
  Serial.println();

  Serial.print("msg_type: ");
  Serial.print(msg_type);
  Serial.println();

  Serial.print("session: ");
  for (int i = 0; i < 8; i++) {
    Serial.print(session[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  int plain_len = data_len + 16;
  byte plain[plain_len];
  plain[0] = to.getAddr();
  plain[1] = _unit_addr;
  plain[2] = msg_type;
  memcpy(plain + 3, session, 8);
  plain[11] = (byte) ((to._counter_send >> 8) & 0xff);
  plain[12] = (byte) (to._counter_send & 0xff);
  if (data != NULL) {
    plain[13] = (byte) data_len;
    memcpy(plain + 14, data, data_len);
  } else {
    plain[13] = 0;
  }
  CRC.crc16(plain, plain_len - 2, plain + plain_len - 2);

  byte iv2[2];
  byte iv16[16];
  iv2[0] = random(0x100);
  iv2[1] = random(0x100);
  for (int i = 0; i < 8; i++) {
    memcpy(iv16 + (i * 2), iv2, 2);
  }

  int paded_len = plain_len + N_BLOCK - plain_len % N_BLOCK;
  byte cipher[paded_len];
  _aes.do_aes_encrypt(plain, plain_len, cipher, _crypto_key, 128, iv16);
  LoRa.beginPacket();
  LoRa.write(_site_id, _site_id_len);
  LoRa.write(iv2, 2);
  LoRa.write(cipher, paded_len);
  LoRa.endPacket();

  to._counter_send = (to._counter_send + 1) % 0x10000;
  if (to._counter_send == 0) {
    Serial.println("Reset after counter overflow");
    to._reset_trial = 0;
    to._reset_last = millis();
    to._reset_intvl = 0;
  }

  return true;
}

void LoRaNetClass::_recv() {
  if (LoRa.parsePacket() == 0) {
    return;
  }

  byte p[128];
  int bytes = 0;
  while (LoRa.available()) {
    p[bytes++] = LoRa.read();
  }

  int cipher_len = bytes - _site_id_len - 2;

  if (cipher_len < 15) {
    return;
  }

  for (int i = 0; i < _site_id_len; i++) {
    if (p[i] != _site_id[i]) {
      return;
    }
  }

  byte iv[16];
  for (int i = 0; i < 8; i++) {
    memcpy(iv + (i * 2), p + _site_id_len, 2);
  }

  byte plain[cipher_len];
  _aes.do_aes_decrypt(p + _site_id_len + 2, cipher_len, plain, _crypto_key, 128, iv);

  Serial.println("========");
  for (int i = 0; i < cipher_len; i++) {
    Serial.print(plain[i], HEX);
    Serial.print("(");
    Serial.print((char) plain[i]);
    Serial.print(") ");
  }
  Serial.println();

  byte to_addr = plain[0];
  byte from_addr = plain[1];

  Serial.print("to_addr: ");
  Serial.print(to_addr);
  Serial.println();

  Serial.print("from_addr: ");
  Serial.print(from_addr);
  Serial.println();

  if (to_addr == from_addr) {
    Serial.println("Error: from == to");
    return;
  }

  byte msg_type = plain[2];
  byte sent_session[8];
  memcpy(sent_session, plain + 3, 8);
  uint16_t sent_counter = ((plain[11] & 0xff) << 8) + (plain[12] & 0xff);
  int data_len = plain[13] & 0xff;
  byte *data;
  if (data_len > 0) {
    data = plain + 14;
  } else {
    data = NULL;
  }

  byte crc[2];
  CRC.crc16(plain, 14 + data_len, crc);
  if (plain[14 + data_len] != crc[0] || plain[14 + data_len + 1] != crc[1]) {
    Serial.println("Error: crc");
    return;
  }

  if (_unit_addr == to_addr) {
    Node *sender = NULL;
    for (int i = 0; i < _nodes_size; i++) {
      if (_nodes[i].getAddr() == from_addr) {
        sender = &_nodes[i];
        break;
      }
    }
    if (sender != NULL) {
      sender->_lora_rssi = LoRa.packetRssi();
      sender->_lora_snr = LoRa.packetSnr();
      _process_message(*sender, msg_type, sent_session, sent_counter, data, data_len);
    }
  }
}

void LoRaNetClass::_process_message(Node &sender, byte msg_type, byte *sent_session, uint16_t sent_counter, byte *data, int data_len) {
  Serial.println("_process_message ====");

  Serial.print("sender: ");
  Serial.print(sender.getAddr());
  Serial.println();

  Serial.print("msg_type: ");
  Serial.print(msg_type);
  Serial.println();

  Serial.print("sent_session: ");
  for (int i = 0; i < 8; i++) {
    Serial.print(sent_session[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  Serial.print("sent_counter: ");
  Serial.print(sent_counter);
  Serial.println();

  Serial.print("data: ");
  for (int i = 0; i < data_len; i++) {
    Serial.print(data[i], HEX);
    Serial.print("(");
    Serial.print((char) data[i]);
    Serial.print(") ");
  }
  Serial.println();

  if (msg_type <= _MSG_RST_4) {
    _process_reset(sender, msg_type, sent_session, sent_counter, data, data_len);

  } else if (memcmp(sender._session, sent_session, 8) == 0
              && sender._counter_recv < sent_counter) {
    sender._counter_recv = sent_counter;
    sender._process_message(msg_type, data, data_len);
  }
}

void LoRaNetClass::_process_reset(Node &sender, byte msg_type, byte *sent_session, uint16_t sent_counter, byte *data, int data_len) {
  if (msg_type == _MSG_RST_1) {
    Serial.println("RST 1");

    uint16_t counter_challenge = (sender._counter_recv + 1) % 0x10000;
    if (counter_challenge > 0xfffa) {
      counter_challenge = 0;
    }
    byte counter_challenge_bytes[2];
    counter_challenge_bytes[0] = (byte) ((counter_challenge >> 8) & 0xff);
    counter_challenge_bytes[1] = (byte) (counter_challenge & 0xff);

    memcpy(sender._reset_session, sent_session, 8);

    _send_with_session(sender, sender._reset_session, _MSG_RST_2, counter_challenge_bytes, 2);

  } else if (msg_type == _MSG_RST_2) {
    Serial.println("RST 2");

    if (memcmp(sender._reset_session, sent_session, 8) != 0) {
      Serial.println("RST 2 error: session");
      return;
    }

    if ((uint16_t) (sender._counter_recv + 1) > sent_counter) {
      Serial.println("RST 2 error: counter");
      return;
    }

    uint16_t counter_challenge = ((data[0] & 0xff) << 8) + (data[1] & 0xff);
    sender._counter_send = counter_challenge;

    _send_with_session(sender, sender._reset_session, _MSG_RST_3, NULL, 0);

    memcpy(sender._session, sender._reset_session, 8);
    sender._session_set = true;
    sender._counter_recv = sent_counter;

  } else if (msg_type == _MSG_RST_3) {
    Serial.println("RST 3");

    if (memcmp(sender._reset_session, sent_session, 8) != 0) {
      Serial.println("RST 3 error: session");
      return;
    }

    uint16_t counter_challenge = (sender._counter_recv + 1) % 0x10000;
    if (counter_challenge > 0xfffa) {
      counter_challenge = 0;
    }

    if (sent_counter != counter_challenge) {
      Serial.println("RST 3 error: counter");
      return;
    }

    _send_with_session(sender, sender._reset_session, _MSG_RST_4, NULL, 0);

    memcpy(sender._session, sender._reset_session, 8);
    sender._session_set = true;
    sender._counter_recv = sent_counter;

    memset(sender._reset_session, 0, 8);
    sender._reset_intvl = -1;

    sender._on_session_reset();

    Serial.println("RST DONE!");

  } else if (msg_type == _MSG_RST_4) {
    Serial.println("RST 4");

    if (memcmp(sender._reset_session, sent_session, 8) != 0) {
      Serial.println("RST 4 error: session");
      return;
    }

    if (sender._counter_recv >= sent_counter) {
      Serial.println("RST 4 error: counter");
      return;
    }

    sender._counter_recv = sent_counter;

    memset(sender._reset_session, 0, 8);
    sender._reset_intvl = -1;

    sender._on_session_reset();

    Serial.println("RST DONE!");

    _reset_last = millis();
    _reset_intvl = 1000;
  }
}

LoRaNetClass LoRaNet;
