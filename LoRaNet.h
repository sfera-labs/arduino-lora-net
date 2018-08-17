#ifndef LoRaNet_h
#define LoRaNet_h

#include <Arduino.h>
#include "AES.h"

/*******************************************************************************
/** Network layer
/******************************************************************************/

class Node {
  private:
    byte _unit_addr;

  public:
    Node();
    Node(byte unitAddr);

    byte _session[8];
    bool _session_set;
    uint16_t _counter_send;
    uint16_t _counter_recv;
    uint16_t _reset_trial;
    unsigned long _reset_last;
    long _reset_intvl;
    byte _reset_session[8];
    int _lora_rssi;
    float _lora_snr;

    byte getAddr();
    void setAddr(byte unitAddr);
    void send(byte msg_type, byte *data, int data_len);
    int loraRssi();
    float loraSnr();
    virtual void _on_session_reset() = 0;
    virtual void _process_message(byte msg_type, byte *data, int data_len) = 0;
};

class LoRaNetClass {
  public:
    LoRaNetClass();
    void init(byte *siteId, int siteIdLen, byte *cryptoKey);
    void process();
    void setLocalAddr(int address);
    void setNodes(Node *nodes, int numOfNodes);
    bool _send(Node &to, byte msg_type, byte *data, int data_len);
    bool _send_with_session(Node &to, byte *session, byte msg_type, byte *data, int data_len);

  private:
    AES _aes;
    byte _crypto_key[16];
    byte *_site_id;
    int _site_id_len;
    byte _unit_addr;
    Node *_nodes;
    int _nodes_size;
    unsigned long _reset_last;
    long _reset_intvl;

    void _recv();
    void _reset();
    void _process_message(Node &sender, byte msg_type, byte *sent_session, uint16_t sent_counter, byte *data, int data_len);
    void _process_reset(Node &sender, byte msg_type, byte *sent_session, uint16_t sent_counter, byte *data, int data_len);
};

extern LoRaNetClass LoRaNet;

/*******************************************************************************
/** Master-Slave layer
/******************************************************************************/

#define _MSG_UPD 10
#define _MSG_CMD 11
#define _MSG_ACK 12

class LocalUnit {
  public:
    LocalUnit();
    LocalUnit(byte unitAddr);
    void process();
};

class RemoteUnit : public Node {
  public:
    RemoteUnit();
    RemoteUnit(byte unitAddr);
};

/**** Master side ****/

class RemoteSlave : public RemoteUnit {
  private:
    bool _last_update_ts_valid;
    unsigned long _last_update_ts;
    unsigned long _last_cmd_ts;
    void _send_cmd();
    virtual bool _has_cmds() = 0;
    virtual byte* _get_cmd_data() = 0;
    virtual int _get_cmd_data_len() = 0;
    virtual void _update_state(byte *data, int data_len) = 0;
    virtual bool _check_cmd_success() = 0;

  public:
    RemoteSlave();
    RemoteSlave(byte unitAddr);
    void process();
    unsigned int stateAge();
    void _on_session_reset();
    void _process_message(byte msg_type, byte *data, int data_len);
};

class Master : public LocalUnit {
  private:
    RemoteSlave *_slaves;
    int _num_of_slaves;

  public:
    Master();
    void setSlaves(RemoteSlave *slaves, int numOfSlaves);
    void process();
};

/**** Slave side ****/

class RemoteMaster;

class LocalSlave : public LocalUnit {
  private:
    void _send_update();
    virtual bool _has_updates() = 0;
    virtual byte* _get_state_data() = 0;
    virtual void _process_ios() = 0;

  public:
    LocalSlave();
    LocalSlave(byte unitAddr);
    void process();
    virtual int _get_state_data_len() = 0;
    virtual void _set_state(byte *data, int data_len) = 0;
};

class RemoteMaster : public RemoteUnit {
  private:
    LocalSlave *_local_slave;
    byte *_last_update_data;
    unsigned long _last_update_ts;
    byte _last_update_ack[32]; // buffer for ack

  public:
    RemoteMaster();
    void _on_session_reset();
    void _process_message(byte msg_type, byte *data, int data_len);
    void _set_local_slave(LocalSlave *local_slave);
    bool _needs_repetition_or_heartbeat();
    void _send_update(byte *data, int data_len);
};

#endif
