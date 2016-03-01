#include "master.h"

void Master::mainBehavior(event_based_actor* self , Master * server_self)
{
  try{
    aout(self)<<"port :"<<server_self->port_<<endl;
    auto p = io::publish(self, server_self->port_);
    aout(self)<<"server create successfully"<<endl;

  }catch (caf::bind_failure &e) {
    cout << "the specified port is already in use" << endl;
  } catch (caf::network_error &e) {
    cout << "master socket related errors occur " << endl;
  }
    self->become(
        //收到心跳
        [=](Heartbeat, string username, string slave_host , UInt16 port ) -> caf::message {
            bool isExist = false;
            if(server_self->alivelist.size() > 0){
              for (auto iter = server_self->alivelist.begin(); iter != server_self->alivelist.end(); iter++)
              {
                if((*iter).second.username_ == username &&
                   (*iter).second.slave_host_ == slave_host &&
                   (*iter).second.port_ == port){
                  //相同计数清零
                  isExist = true;
                  (*iter).first = 0;
                  }
              }
            }
            if(!isExist){
              SlaveInfo sif;
              sif.username_ = username;
              sif.slave_host_ = slave_host;
              sif.port_= port;
              auto sif_pair = std::make_pair(0,sif);
              server_self->alivelist.push_back(sif_pair);
            }
            return caf::make_message(OkAtom::value);
        },
        //请求列表
        [=](GetList) -> caf::message{
          vector<SlaveInfo> friendlist;
          for (auto iter = server_self->alivelist.begin(); iter != server_self->alivelist.end(); iter++)
          {
            friendlist.push_back((*iter).second);
          }
          string list = Serialize<vector<SlaveInfo>>(friendlist);
          return caf::make_message(OkAtom::value,list);
        },
        //更新存活列表mainBehavior,port_
        [=](Updatelist){
          if(server_self->alivelist.size() > 0)
          {
            for (auto iter = server_self->alivelist.begin(); iter != server_self->alivelist.end(); iter++)
            {
              (*iter).first++;
              //cout<<(*iter).first<<endl;
              if((*iter).first >= kMaxTryTimes)
              {
                server_self->alivelist.erase(iter);
              }
            }
          }
          //3秒更新一次存活列表
          self->delayed_send(self,std::chrono::seconds(kTimeout/2),Updatelist::value);
        },
        others >> [=]() { cout << "unknown message" << endl; }
        );
    self->send(self,make_message(Updatelist::value));
}
void T(event_based_actor * self){}
void Master::start()
{
    auto master_actor = caf::spawn(mainBehavior,this);

    caf::await_all_actors_done();
}
