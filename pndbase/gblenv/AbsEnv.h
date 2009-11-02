#ifndef AbsEnv_HH
#define AbsEnv_HH

class IfdProxyDict;

class AbsEnv {
public:
  // Constructors
  AbsEnv();
  
  // Destructor
  virtual ~AbsEnv();

protected:

 private:
  
};

extern IfdProxyDict* gblEvtDict; //dictionary for event livetime
// extern IfdProxyDict* gblJobDict; //dictionary for run livetime

#endif //AbsEnv_HH
