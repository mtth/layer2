#ifndef LAYER2_DECODE_RADIOTAP_HPP
#define LAYER2_DECODE_RADIOTAP_HPP

#include <nan.h>
#include <node.h>

using v8::Boolean;
using v8::FunctionTemplate;
using v8::Integer;
using v8::Number;
using v8::Handle;
using v8::Local;
using v8::Persistent;
using v8::Object;
using v8::String;

class RadiotapDecoder : public node::ObjectWrap {

public:
  static NAN_METHOD(init);
  static NAN_METHOD(decode);

private:
  void add_chaninfo(Local<Object> obj, int freq, int flags);
  int add_field(
    Local<Object> obj,
    struct cpack_state *s,
    uint32_t bit,
    uint8_t *flags,
    struct radiotap_state *state,
    uint32_t presentflags
  );

  Persistent<String> RTAP_ANTENNA_KEY;
  Persistent<String> RTAP_BAD_FCS_KEY;
  Persistent<String> RTAP_CFP_KEY;
  Persistent<String> RTAP_CHANNEL_FLAGS_KEY;
  Persistent<String> RTAP_CHANNEL_FREQ_KEY;
  Persistent<String> RTAP_ENCRYPTED_KEY;
  Persistent<String> RTAP_FH_PAT_KEY;
  Persistent<String> RTAP_FH_SET_KEY;
  Persistent<String> RTAP_FRAG_KEY;
  Persistent<String> RTAP_MCS_KEY;
  Persistent<String> RTAP_MCS_BW_KEY;
  Persistent<String> RTAP_MCS_FEC_TYPE_KEY;
  Persistent<String> RTAP_MCS_GI_KEY;
  Persistent<String> RTAP_MCS_HT_FORMAT_KEY;
  Persistent<String> RTAP_MCS_RX_STBC_KEY;
  Persistent<String> RTAP_NOISE_DB_KEY;
  Persistent<String> RTAP_RATE_KEY;
  Persistent<String> RTAP_SHORT_KEY;
  Persistent<String> RTAP_SIGNAL_DB_KEY;
  Persistent<String> RTAP_SQ_KEY;
  Persistent<String> RTAP_TSFT_KEY;
  Persistent<String> RTAP_TX_POWER_KEY;
  Persistent<String> RTAP_TX_POWER_DB_KEY;
  Persistent<String> RTAP_TX_POWER_DBM_KEY;

};

#endif
