#ifndef __WB_COMMITFAULT_HH__
#define __WB_COMMITFAULT_HH__

#include "config/the_isa.hh"
#include "base/types.hh"
#include "fi/issue_execute_fault.hh"
#include "fi/faultq.hh"

/*
 * Inject a fault into the wb to commit stage pipeline register
 */


class WB_CommitFault : public Issue_ExecuteFault 
{
private:

protected:
 //Fields
 short int misPredAddr = 24;
 short int redPC = 25;
 short int squashSeqNum = 26;
 short int numDest = 27;
 short int load = 28;
 short int squash = 29;
 short int branchMisPred = 30;
 short int destReg = 31;
 short int pC = 32;
 short int phyDestReg = 33;
 short int prevDestReg = 34;
 short int brTaken = 35;
 short int memRef = 36;
public:
  //field types
  static const int misPredAddr_type = 8;
  static const int redPC_type = 9;
  static const int squashSeqNum_type = 10;  
  static const int squash_type = 11;
  static const int branchMisPred_type = 12;
  static const int brTaken_type = 13;

  WB_CommitFault(std::ifstream &os, bool inherited);
  ~WB_CommitFault();

  virtual const char *description() const;
  
  virtual void dump() const;

  virtual void set_field(std::string file_field);

  //Process functions
  bool process(bool val);
  Base_PipelineFault::dynInstPtr process(dynInstPtr curInst);
  template<class T> inline T process(T curVal)
  {
    if(DTRACE(FaultInjection)){
      if(field==misPredAddr){
        cout<<"===MispredictionAddress::process()==="<<endl;
    }
    else if(field==squashSeqNum){
      cout<<"===SquashSeqNum::process()==="<<endl;
      }
    }

   curVal = manifest(curVal,getValue(),getValueType());
   check4reschedule();

   if(DTRACE(FaultInjection)){
     if(field==misPredAddr){
       cout<<"===MispredictionAddress::process()==="<<endl;
     }
   else if(field==squashSeqNum){
     cout<<"===SquashSeqNum::process()==="<<endl;
     }
   }

  return curVal;
  } 

  PCState process(PCState pC);

};

/**WriteBack Commit Fault Queue
 *Its purpouse is very straightforward
 */

class WB_CommitFaultQueue : public InjectedFaultQueue
{
  private:

  protected:

  public:
    using InjectedFaultQueue::scan;
    virtual WB_CommitFault *scan(std::string s, ThreadEnabledFault &thisThread, Addr vaddr, int curBundle, int fieldType);
};
#endif // __WB_COMMIT_FAULT_HH__
