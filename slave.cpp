#include "slave.h"

void Slave::showFriendList()
{
  if(friendlist.size() == 0)
  {
    cout<<"目前没有好友"<<endl;
  }else{
    cout<<"好友列表为"<<endl;
    int i = 0;
    for (auto iter = this->friendlist.begin(); iter != this->friendlist.end(); iter++ , ++i)
    {
      cout<<i+1<<"  :"<<(*iter).username_<<endl;
    }
  }
}

void Slave::sendMessage(const string msg , const int id)
{
  if(id <= this->friendlist.max_size()){
  scoped_actor self;
  auto people = caf::io::remote_actor(this->friendlist[id].slave_host_,
                                      this->friendlist[id].port_);
  self->send(people,msg,this->username_);
  }else{
    cout<<"目前没有这个好友ID"<<endl;
  }
}

void Slave::sendFile(const string filename , const int id)
{
  if(id <= this->friendlist.max_size()){
    scoped_actor self;
    auto people = caf::io::remote_actor(this->friendlist[id].slave_host_,
                                        this->friendlist[id].port_);
    ifstream in(filename);
    if(!in.is_open())
    {
        cout<<"file open failed"<<endl;
        return;
    }
    in.seekg(0,in.end);
    int length = in.tellg();
    in.seekg(0,in.beg);
    char *buffer = new char[length];
    in.read(buffer,length);
    string file = buffer;
    message_builder mb;

    self->send(people,File::value,file,filename,this->username_, length);
    }else{
      cout<<"目前没有这个好友ID"<<endl;
    }
}

void Slave::requestFriendList()
{
  scoped_actor self;
  auto master = caf::io::remote_actor(master_host_, master_port_);
  self->sync_send(master, GetList::value).await(
    [=](OkAtom, string friendlist){
      auto list = Derialize<vector<SlaveInfo>>(friendlist);
      this->friendlist = list;
      //cout<<"friendlist update success"<<endl;
    },
    caf::after(std::chrono::seconds(kTimeout)) >> [=]() {
      //cout << "friendlist update timeout" << endl;
    });
};

void Slave::mainBehaveior(event_based_actor* self, Slave *client_self)
{
  try {
     caf::io::publish(self, client_self->port_);
     //cout << "client publish success" << endl;
   } catch (caf::bind_failure &e) {
     //cout << "the specified port is already in use" << endl;
   } catch (caf::network_error &e) {
     //cout << "client socket related errors occur " << endl;
   }
   self->become(
       [=](GetList){
     client_self->requestFriendList();
     //30秒自动请求一次好友列表
     self->delayed_send(self,std::chrono::seconds(kTimeout/3),GetList::value);
   },
       [=](Heartbeat){
         auto master = caf::io::remote_actor(client_self->master_host_, client_self->master_port_);
         self->sync_send(master, Heartbeat::value, client_self->username_, client_self->host_, client_self->port_).then(
           [=](OkAtom){
           //cout<<"heartbeat nomally"<<endl;
       });
         self->delayed_send(self,std::chrono::seconds(kTimeout),Heartbeat::value);
   },
       [=](OkAtom,string friendlist){
          auto list = Derialize<vector<SlaveInfo>>(friendlist);
          client_self->friendlist = list;
          //cout<<"friendlist update success"<<endl;
   }
   ,
       [=](string message, string username){
         cout<<username<<":"<<message<<endl;
   },
       [=](File, string file, string filename, string username, int length)
       {
         cout<<username<<"发来一个文件."<<endl;\
         filename = filename+".bak";
         ofstream out(filename);
         if(!out.is_open())
         {
             cout<<"接受文件失败"<<endl;
         }
         char *buffer = const_cast<char *>(file.c_str());
         out.write(buffer, length);
         return caf::make_message(FileOK::value,filename);
       }
   ,
       [=](FileOK, string filename){
         cout<<filename<<"send finished\n";
   }
   ,
   others >> [=]() { cout << "受到未知消息" << endl; }
   );
   self->send(self, GetList::value);
   self->send(self, Heartbeat::value);
}
