typedef int bad_type; // Remove this to get unset type reporting

bad_type sc(bad_type x,bad_type min,bad_type max,bad_type s);

#include "CommClient.h"
#include "ClientConnection.h"

class ObserverClient : public CommClient {
  public:
    ObserverClient();
    void loadDefault();
    void idle();
};

