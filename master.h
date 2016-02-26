#include <utility>
#include <vector>

#include "SlaveInfo.h"

class Master {
public:
  Master(){};
  Master(UInt16 port):port_(port){};
  void start();
private:
  UInt16 port_;
  //存活列表 int表示心跳失去计数,SlaveInfo包含客户端信息
  vector<pair<int,SlaveInfo>> alivelist;

  //自动累加心跳数，维护存活列表
  //void *refresh();
  //主要行为包括1.返回存活列表 2.接受心跳 3.更新列表。
  static void mainBehavior(event_based_actor* self , Master * server_self);
};
