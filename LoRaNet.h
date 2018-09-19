#ifndef LoRaNet_h
#define LoRaNet_h

#include <Arduino.h>
#include "AES.h"

// #define __SL_DEBUG 1

#ifdef __SL_DEBUG
#define __DEBUGprint(...) Serial.print(__VA_ARGS__)
#define __DEBUGprintln(...) Serial.println(__VA_ARGS__)
#else
#define __DEBUGprint(...)
#define __DEBUGprintln(...)
#endif

/*******************************************************************************
/** Network layer
/******************************************************************************/

class LoRaNode {
  private:
    byte _unit_addr;

  public:
    LoRaNode();
    LoRaNode(byte unitAddr);

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
    bool send(byte msg_type, byte *data, int data_len);
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
    void setLocalAddr(byte address);
    byte getLocalAddr();
    void setNodes(LoRaNode **nodes, int numOfNodes);
    void enableDiscovery(LoRaNode **buff, int maxSize);
    void setDutyCycle(unsigned long windowSeconds, int permillage);
    bool _send(LoRaNode &to, byte msg_type, byte *data, int data_len);
    bool _send_with_session(LoRaNode &to, byte *session, byte msg_type, byte *data, int data_len);

  private:
    AES _aes;
    byte _crypto_key[16];
    byte *_site_id;
    int _site_id_len;
    byte _unit_addr;
    LoRaNode **_nodes;
    int _nodes_size;
    unsigned long _reset_last;
    long _reset_intvl;
    int _reset_idx;
    int _nodes_disc_buff_size;
    unsigned long _dc_window;
    unsigned long _dc_window_start;
    unsigned long _dc_tx_time_max;
    unsigned long _dc_tx_time;
    unsigned long _dc_tx_start;
    bool _dc_tx_on;
    bool _dc_exceeded;

    void _recv();
    void _reset();
    void _duty_cycle();
    void _process_message(LoRaNode &sender, byte msg_type, byte *sent_session, uint16_t sent_counter, byte *data, int data_len);
    void _process_reset(LoRaNode &sender, byte msg_type, byte *sent_session, uint16_t sent_counter, byte *data, int data_len);
};

extern LoRaNetClass LoRaNet;

/*******************************************************************************
/** Master-Slave layer
/******************************************************************************/

#define _MSG_UPD 10
#define _MSG_CMD 11
#define _MSG_ACK 12

class LoRaLocalUnit {
  public:
    LoRaLocalUnit();
    LoRaLocalUnit(byte unitAddr);
    void setAddr(byte unitAddr);
    byte getAddr();
    void process();
};

class LoRaRemoteUnit : public LoRaNode {
  public:
    LoRaRemoteUnit();
    LoRaRemoteUnit(byte unitAddr);
};

/**** Master side ****/

class LoRaRemoteSlave : public LoRaRemoteUnit {
  private:
    bool _last_update_ts_valid;
    unsigned long _last_update_ts;
    unsigned long _last_cmd_ts;
    int _last_cmd_attempts;
    bool _send_cmd();
    virtual bool _has_cmds() = 0;
    virtual byte* _get_cmd_data() = 0;
    virtual int _get_cmd_data_len() = 0;
    virtual void _update_state(byte *data, int data_len) = 0;
    virtual bool _check_cmd_success() = 0;

  public:
    LoRaRemoteSlave();
    LoRaRemoteSlave(byte unitAddr);
    bool process();
    unsigned int stateAge();
    void _on_session_reset();
    void _process_message(byte msg_type, byte *data, int data_len);
};

class LoRaLocalMaster : public LoRaLocalUnit {
  private:
    LoRaRemoteSlave **_slaves;
    int _num_of_slaves;
    int _process_idx;

  public:
    LoRaLocalMaster();
    void setSlaves(LoRaRemoteSlave **slaves, int numOfSlaves);
    void enableDiscovery(LoRaRemoteSlave **buff, int maxSize);
    void process();
};

/**** Slave side ****/

class LoRaRemoteMaster;

class LoRaLocalSlave : public LoRaLocalUnit {
  private:
    virtual void _process_ios() = 0;

  public:
    LoRaLocalSlave();
    LoRaLocalSlave(byte unitAddr);
    void process();
    virtual bool _has_updates() = 0;
    virtual byte* _get_state_data() = 0;
    virtual int _get_state_data_len() = 0;
    virtual void _set_state(byte *data, int data_len) = 0;
};

class LoRaRemoteMaster : public LoRaRemoteUnit {
  private:
    LoRaLocalSlave *_local_slave;
    byte *_last_update_data;
    unsigned long _last_update_ts;
    int _last_update_attempts;
    byte _last_update_ack[32]; // buffer for ack

  public:
    LoRaRemoteMaster();
    void _on_session_reset();
    void _process_message(byte msg_type, byte *data, int data_len);
    void _set_local_slave(LoRaLocalSlave *local_slave);
    bool _send_update();
    bool _send_update(byte *data, int data_len);
    void _process();
};

#endif
