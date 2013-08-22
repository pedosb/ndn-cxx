/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#include <cstdlib>
#include <sstream>
#include <iostream>
#include <ndn-cpp/interest.hpp>
#include <ndn-cpp/data.hpp>
#include <ndn-cpp/face.hpp>

using namespace std;
using namespace ndn;
using namespace ptr_lib;

class MyClosure : public Closure {
public:
  MyClosure()
  : gotCallbackCount_(0)
  {  
  }
  
  virtual UpcallResult upcall(UpcallKind kind, const UpcallInfo &upcallInfo)
  {
    if (kind == UPCALL_DATA || kind == UPCALL_DATA_UNVERIFIED) {
      ++gotCallbackCount_;
      cout << "Got data packet with name " << upcallInfo.getData()->getName().to_uri() << endl;
      for (unsigned int i = 0; i < upcallInfo.getData()->getContent().size(); ++i)
        cout << upcallInfo.getData()->getContent()[i];
      cout << endl;
      
      return CLOSURE_RESULT_OK;
    }
    else if (kind == UPCALL_INTEREST_TIMED_OUT) {
      ++gotCallbackCount_;
      cout << "Time out for interest  " << upcallInfo.getInterest()->getName().toUri() << endl;    
      return CLOSURE_RESULT_OK;
    }
    else
      return CLOSURE_RESULT_OK;
  }
  
  int gotCallbackCount_;
};

int main(int argc, char** argv)
{
  try {
    MyClosure closure;
    Face face("E.hub.ndn.ucla.edu");
    
    Name name1("/ndn/ucla.edu/apps/ndn-js-test/hello.txt/level2/%FD%05%0B%16%7D%95%0E");    
    cout << "Express name  " << name1.toUri() << endl;
    face.expressInterest(name1, &closure);
    Name name2("/ndn/ucla.edu/apps/lwndn-test/howdy.txt/%FD%05%05%E8%0C%CE%1D");
    cout << "Express name  " << name2.toUri() << endl;
    face.expressInterest(name2, &closure);

    // The main event loop.
    while (closure.gotCallbackCount_ < 2) {
      face.processEvents();
      // We need to sleep for a few milliseconds so we don't use 100% of the CPU.
      usleep(10000);
    }
  } catch (std::exception &e) {
    cout << "exception: " << e.what() << endl;
  }
  return 0;
}
