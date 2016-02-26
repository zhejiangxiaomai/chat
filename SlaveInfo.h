#include "string"
#include <unistd.h>
#include "caf/all.hpp"
#include "caf/io/all.hpp"
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/vector.hpp>
#include <pthread.h>

using namespace std;
using namespace caf;
typedef unsigned short UInt16;

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::vector;
using std::pair;

using caf::event_based_actor;
using caf::behavior;
using caf::on;
using caf::spawn;
using caf::io::publish;
using caf::actor;
using caf::others;

using GetList = caf::atom_constant<caf::atom("getList")>;
using Heartbeat = caf::atom_constant<caf::atom("Heartbeat")>;
using Updatelist = caf::atom_constant<caf::atom("Updatelist")>;
using OkAtom = caf::atom_constant<caf::atom("ok")>;

const auto kTimeout = 6;
const auto kMaxTryTimes = 3;

struct SlaveInfo
{
  friend class boost::serialization::access;
  //use this function to serialize and deserialize object
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version)
  {
    ar & username_  & slave_host_ & port_;
  }

  std::string username_, slave_host_;
  UInt16 port_;
};


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
