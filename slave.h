#include "SlaveInfo.h"

class Slave{
 public:
  Slave(){};
  Slave(string username, string host, string master_host, UInt16 port, UInt16 master_port )
  :username_(username),host_(host),master_host_(master_host),
   port_(port),master_port_(master_port)
  {
    SlaveInfo self{username_, host_, port_};
    friendlist.push_back(self);
    //一开始先发送一个心跳
    auto master = caf::io::remote_actor(master_host_, master_port_);
    caf::scoped_actor scoped_self;
    try{
    scoped_self ->sync_send(master, Heartbeat::value, username_, host_, port_).await(
          [=](OkAtom){ cout<<username+"log in successfully"<<endl;
    });
    }catch(caf::network_error & e) {
        cout << "cannot't connect to master <"<< master_host_ <<","<< master_port_<<">" <<endl;
    }
    auto client = spawn(mainBehaveior, this);
  };
  //向某个slave发送消息
  void sendMessage( const string msg , const int id);
  void sendFile(const string filename , const int id);
  void showFriendList();
  void requestFriendList();
  private:
  string username_, host_,master_host_;
  UInt16 port_,master_port_;
  actor client;
  //向master发送心跳，获取存活列表，接受消息。
  static void mainBehaveior(event_based_actor* self,Slave *client_self);
  vector<SlaveInfo> friendlist;
};
