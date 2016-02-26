#include "master.h"


int main(int argc, char** argv)
{
  UInt16 port = 0;
  auto res = message_builder(argv + 1, argv + argc).extract_opts({
        {"port,p", "set port", port}
      });
  if (! res.error.empty()) {
    cerr << res.error << endl;
    return 1;
  }
  if (res.opts.count("help") > 0) {
    cout << res.helptext << endl;
    return 0;
  }
  if (! res.remainder.empty()) {
  }
  if(port != 0)
  {
    Master server(port);
    server.start();
  }
  else
  {
    cout<<"端口错误"<<endl;
  }
  return 0;
}
