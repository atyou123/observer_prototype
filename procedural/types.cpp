#include "../util/Serialization.h"

class ExternalMessage {
  public:
    enum Sender {
        A,
        B,
        C,
        D,
    };

  private:
    Sender _sender;
    u8 byte;
};