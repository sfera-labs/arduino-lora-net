#ifndef LoRaNet_h
#define LoRaNet_h

#include <Arduino.h>
#include "AES.h"

class Node {
  public:
    Node();
    Node(byte unitAddr);

    unsigned long _last_update;
    byte _session[8];
    uint16_t _counter_send;
    uint16_t _counter_recv;
    uint16_t _reset_trial;
    unsigned long _reset_last;
    long _reset_intvl;
    byte _reset_session[8];

    byte getAddr();

  private:
    byte _unit_addr;

};

class LoRaNetClass {
  public:
    LoRaNetClass();
    void begin(byte *siteId, int siteIdLen, byte *cryptoKey);
    void initNode(byte unitAddr);

    // TODO move to private
    bool _send(Node &to, byte *session, byte msg_type, byte *data, int data_len);
    void _recv();

  private:
    AES _aes;
    byte _crypto_key[16];
    byte *_site_id;
    int _site_id_len;
    byte _unit_addr;
    Node *_nodes;
    int _nodes_size;

    void _reset();
    void _process_message(Node &sender, byte msg_type, byte *sent_session, uint16_t sent_counter, byte *data, int data_len);
    void _process_reset(Node &sender, byte msg_type, byte *sent_session, uint16_t sent_counter, byte *data, int data_len);
};

extern LoRaNetClass LoRaNet;

#endif
