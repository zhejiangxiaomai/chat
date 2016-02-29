#include "slave.h"
//./main_slave -h 127.0.0.1 -p 10001 -H 127.0.0.1 -P 20000 -u zzh

void menu();

int main(int argc, char** argv)
{
  string host,master_host,username;
  UInt16 port,master_port;
  auto res = message_builder(argv + 1, argv + argc).extract_opts({
          {"host,h", "set host", host},
          {"port,p", "set port", port},
          {"master_host,H", "set master host", master_host},
          {"master_port,P", "set master port", master_port},
          {"username,u", "set username",username}
  });
  if (! res.error.empty()) {
      cerr << res.error << endl;
      return 1;
  }
  if (res.opts.count("help") > 0) {
    cout << res.helptext << endl;
    return 0;
  }
  Slave client(username, host, master_host, port, master_port);
  int choice;
  menu();
  while(cin>>choice)
  {
// for test
//    client.showFriendList();
//    sleep(3);
//    string s = "哈哈";
//    client.sendMessage(s,0);
    int id = 0;
    string msg;
    switch(choice)
    {
      case 1:
        client.showFriendList();
        break;
      case 2:
        cout<<"输入消息"<<endl;
        cin>>msg;
        cout<<"输入好友ID"<<endl;
        cin>>id;
        client.sendMessage(msg,id-1);
        break;
      case 3:
        cout<<"输入文件名"<<endl;
        cin>>msg;
        cout<<"输入好友ID"<<endl;
        cin>>id;
        client.sendFile(msg, id-1);
        //发送文件
        break;
      case 4:
       exit(0);
        break;
      default:
        menu();
        break;
    }
  }
  caf::await_all_actors_done();
  return 0;
}


void menu()
{
  cout<<"1. 查看在线好友"<<endl
      <<"2. 发送消息(格式为 消息 + 好友ID)"<<endl
      <<"3. 发送文件(格式为 文件名 +好友ID)"<<endl
      <<"4. 下线"<<endl;
}
