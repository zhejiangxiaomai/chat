#include "caf/all.hpp"
#include "caf/io/all.hpp"
#include "string"
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/tuple/tuple.hpp>

using std::cout;
using std::cerr;
using std::endl;
using std::string;

using namespace caf;
using namespace std;

typedef unsigned short UInt16;

template<typename T>string Serialize(T obj);
template<typename T>T Derialize(string obj);



class nodeInfo{
  public:
    nodeInfo(string nodename,float ratio):nodename_(nodename),ratio_(ratio)
    {
    };
    nodeInfo(){};
    string nodename_;
    float ratio_;
  private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
           ar & nodename_;
           ar & ratio_;
    }
};


void reciever(event_based_actor* self , UInt16 port)
{
  auto p = io::publish(self, port);
	self->become(
		[self](const string& text) -> string {
	 // nodeInfo node1 = Derialize<nodeInfo>(text);
	//aout(self)<<"name:"<<node1.nodename_ <<"ratio :"<< node1.ratio_<<endl;
			return "a";
		}
		);
}

void sender(event_based_actor* self,const string& host, UInt16 port)
{
  auto receiver = caf::io::remote_actor(host,port);
  caf::scoped_actor scoped_self;
  string info = "hello";
  scoped_self ->sync_send(receiver,info).await(
	    [=](const std::string& response) {
	      cout<<"同步成功"<<response<<endl;
	});
	aout(self)<<"send success"<<endl;
}



int main(int argc, char** argv)
{
	//string host;
	//UInt16 port;
	UInt16 port = 8081;
//	auto res = message_builder(argv + 1, argv + argc).extract_opts({
//	    {"host,M", "set host", host},
//	    {"port,m", "set port", port}
//	  });
//	if (! res.error.empty()) {
//	cerr << res.error << endl;
//	return 1;
//	}
//	if (res.opts.count("help") > 0) {
//	cout << res.helptext << endl;
//	return 0;
//	}
//	if (! res.remainder.empty()) {
//	}
	auto reciever_actor = spawn(reciever,8081);
	auto send_actor = spawn(sender,"127.0.0.1",port);

	await_all_actors_done();
  //shutdown();
	//cout<<host<<"  "<<port;
	return 0;

}

/**
 *  Transform T type c++ object to string
 */
template<typename T>
string Serialize(T obj) {
  stringstream ss;
  boost::archive::text_oarchive oa(ss);
  oa << obj;
  return ss.str();
}

/**
 * Transform string to T type c++ object
 */
template<typename T>
T Derialize(string obj) {
  T ret;
  stringstream ss(obj);
  boost::archive::text_iarchive ia(ss);
  ia >> ret;
  return ret;
}
