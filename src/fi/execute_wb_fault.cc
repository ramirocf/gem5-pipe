#include "base/types.hh"
#include "fi/faultq.hh"
#include "fi/issue_execute_fault.hh"
#include "fi/fi_system.hh"
#include "arch/alpha/faults.hh"

using namespace std;

Execute_WBFault::Execute_WBFault(std::ifstream &os,bool inherited)
  : Issue_ExecuteFault(os,true)
{
  if(!inherited){
   string file_field,file_bundleNum;

   setFaultType(InjectedFault::Execute_WBFault);
   fi_system->execute_WBFaultQueue.insert(this);
   os>>file_field;
   os>>file_bundleNum;
   cout<<"File field: "<<file_field<<endl;
   cout<<"File bundle num: "<<file_bundleNum<<endl;
   set_field(file_field);
   set_bundleNum(file_bundleNum);
   if(DTRACE(FaultInjection)){
     cout<<"Execute-WB Fault::field : "<<field<<endl;
     cout<<"Execute-WB Fault::bundleNum : "<<bundle_num<<endl;
   }   
  }
}

Execute_WBFault::~Execute_WBFault()
{
}

const char *
Execute_WBFault::description() const
{
    return "Execute_WB Fault";
}

void Execute_WBFault::dump() const
{
  if (DTRACE(FaultInjection)) {
    std::cout << "===Execute_WBFault::dump()===\n";
    O3CPUInjectedFault::dump();
    cout<<"        Field : "<<field<<endl;
    cout<<"        Field Type : "<<field_type<<endl;
    cout<<"        BundleNum : "<<bundle_num<<endl;
    std::cout << "~==Execute_WBFault::dump()===\n";
  }
}

void Execute_WBFault::set_field(string file_field)
{
  //First try with fields added during this stage
  if(file_field.compare("BranchTaken")==0){
    field = branchTaken;
    field_type = branchTaken_type;
  }
  else if(file_field.compare("Exception")==0){
    field = exception;
    field_type = exception_type;
  }
  else if(file_field.compare("Value")==0){
    field = value;
    field_type = value_type;
  }
  else{
    //Look for fields in previous stages
    Issue_ExecuteFault::set_field(file_field);
    field_type = exception_type;
  }
}

//Process Functions
//Process for boolean values
bool Execute_WBFault::process (bool value)
{
  if(DTRACE(FaultInjection)){
    if(field == branchTaken){
      cout <<"===BranchTaken::process()==="<<endl;
    }
  }

  value = manifest_bool(value,getValue(),getValueType());
  check4reschedule();

  if(DTRACE(FaultInjection)){
    if(field == branchTaken){
      cout <<"===BranchTaken::process()==="<<endl;
    }
  }  

  return value;
}

//Process for faulty values(Exception Flag)
Fault Execute_WBFault::process(Fault value)
{
  if(DTRACE(FaultInjection)){
    cout<<"Exception::process()"<<endl;
  }

  value = (value == NoFault) ? new InterruptFault : NoFault;
  check4reschedule();

  if(DTRACE(FaultInjection)){
    cout<<"Exception::process()"<<endl;
  }

  return value;
}

//Class Execute WriteBack Fault Queue
Execute_WBFault *Execute_WBFaultQueue::scan(string s, ThreadEnabledFault &thisThread, Addr vaddr, int curBundle, int fieldType)
{
  static uint64_t called=0;
  Execute_WBFault *p;
  int i;
  uint64_t exec_time = 0;
  uint64_t exec_instr = 0;
  unsigned char flag = 1;
  p = reinterpret_cast<Execute_WBFault *> (head);
  //pass the list and check if a fault meets the conditions

  while(p && flag){
    if(!p->isManifested()){
      exec_time = 0;
      exec_instr = 0;
      //find how much time do I run.
      if(name().compare("DecodeStageFaultQueue") == 0 ){
        i = fi_system->get_fi_decode_counters( p , thisThread, s , &exec_instr , &exec_time );
        called++;
      }
      else if(name().compare("IEWStageFaultQueue") == 0 ){
        i = fi_system->get_fi_exec_counters( p , thisThread, s , &exec_instr , &exec_time );
      }
      else if (name().compare("LoadStoreFaultQueue") == 0 ){
        i = fi_system->get_fi_loadstore_counters( p , thisThread, s , &exec_instr , &exec_time );
      }
      else{
        i = fi_system->get_fi_fetch_counters( p , thisThread, s , &exec_instr , &exec_time );
      }
      if(i){
        switch( p->getTimingType() ){
          case (InjectedFault ::TickTiming):
            {  
              if((exec_time == p->getTiming()&&
                curBundle == p->get_bundleNum() &&
                fieldType == p->get_fieldType())){ //correct time so intend to manifest
                p->setServicedAt(exec_time);
                return(p);
              }
              else if(exec_time > p->getTiming())
                flag = 0;
            }
            break;
          case (InjectedFault ::InstructionTiming):
            {
              if(exec_instr == p->getTiming() &&
                curBundle == p->get_bundleNum() &&
                fieldType == p->get_fieldType()){
                p->setServicedAt(exec_instr);
                return(p);
              }
              else if(exec_instr > p->getTiming())
                flag = 0;
            }
            break;
          case (InjectedFault ::VirtualAddrTiming):
            {
              if(vaddr == p->getTiming() + thisThread.getMagicInstVirtualAddr() ){
                p->setServicedAt(vaddr);
                p->dump();
                return(p);
              }

            }
            break;
          default:
            {
              std::cout << "InjectedFaultQueue::scan() - getTimingType default type error\n";
              assert(0);
              break;
            }


        }
      }
    }
    p = reinterpret_cast<Execute_WBFault*> (p->nxt);
  }
  return(NULL);

}

