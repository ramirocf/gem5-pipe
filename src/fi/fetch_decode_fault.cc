#include "base/types.hh"
#include "fi/faultq.hh"
#include "fi/fetch_decode_fault.hh"
#include "fi/fi_system.hh"

using namespace std;

Fetch_DecodeFault::Fetch_DecodeFault(std::ifstream &os,bool inherited)
  : Base_PipelineFault(os)
{
  if(!inherited){
   string file_field,file_bundleNum;

   setFaultType(InjectedFault::Fetch_DecodeFault);
   fi_system->fetch_DecodeFaultQueue.insert(this);
   os>>file_field;
   os>>file_bundleNum;
   cout<<"File field: "<<file_field<<endl;
   cout<<"File bundle num: "<<file_bundleNum<<endl;
   set_field(file_field);
   set_bundleNum(file_bundleNum);
   if(DTRACE(FaultInjection)){
     cout<<"Fetch-Decode Fault::field : "<<field<<endl;
     cout<<"Fetch-Decode Fault::bundleNum : "<<bundle_num<<endl;
   }   
  }
}

Fetch_DecodeFault::~Fetch_DecodeFault()
{
}

void Fetch_DecodeFault::set_field(string file_field)
{
  if(file_field.compare("MachInst")==0){
    field = machInst;
    field_type = machInst_type;
  }
  else if(file_field.compare("PC")==0){
    field = pC;
    field_type = dynInst;
  }
  else if(file_field.compare("PredPC")==0){
    field = predPC;
    field_type = dynInst;
  }
  else if(file_field.compare("Taken")==0){
    field = taken;
    field_type = dynInst;
  }
  else{
    if(DTRACE(FaultInjection)){
      cout<<"No such field: "<<file_field<<endl;
    }
  }
}

const char *
Fetch_DecodeFault::description() const
{
    return "Fetch_DecodeFault";
}

void Fetch_DecodeFault::dump() const
{
  if (DTRACE(FaultInjection)) {
    std::cout << "===Fetch_DecodeFault::dump()===\n";
    O3CPUInjectedFault::dump();
    cout<<"        Field : "<<field<<endl;
    cout<<"        Field Type : "<<field_type<<endl;
    cout<<"        BundleNum : "<<bundle_num<<endl;
    std::cout << "~==Fetch_DecodeFault::dump()===\n";
  }
}

///Process functions
///Injecting a fault after reading from Icache
TheISA::MachInst Fetch_DecodeFault::process(TheISA::MachInst curInst)
{
  DPRINTF(FaultInjection,"===MachInst::process()===");
  dump();
  curInst = manifest(curInst,getValue(),getValueType());
  check4reschedule();
  DPRINTF(FaultInjection,"===MachInst::process()===");
  return curInst;
}

//Injecting a fault in fetch's primary fields
Base_PipelineFault::dynInstPtr Fetch_DecodeFault::process(dynInstPtr curInst)
{
  if(field==pC){
    DPRINTF(FaultInjection,"===PC::process===");
    dump();
    pC_process(curInst);
    DPRINTF(FaultInjection,"===PC::process===");
  }
  else if(field==predPC){
    DPRINTF(FaultInjection,"===PredPC::process()===");
    dump();
    predPC_process(curInst);
    DPRINTF(FaultInjection,"===PredPC::process()===");
  } 
  else if(field==taken){
    DPRINTF(FaultInjection,"===Taken::process()===");
    dump();
    taken_process(curInst);
    DPRINTF(FaultInjection,"===Taken::process()===");
  }
  else{
    if(DTRACE(FaultInjection)){
      cout<<"Invalid field.This should not happen"<<endl;
    }
  }
  return curInst;
}

//Process for PC field
void Fetch_DecodeFault::pC_process(dynInstPtr &curInst)
{
  TheISA::PCState curState = curInst->pcState();
  Addr curPC = curState.pc();
  curPC = manifest(curPC,getValue(),getValueType());
  curState.pc(curPC);
  curInst->pcState(curState);
  check4reschedule();
}

//Process for PredPC field
void Fetch_DecodeFault::predPC_process(dynInstPtr &curInst)
{
  TheISA::PCState curState = curInst->pcState();
  Addr curNPC = curState.npc();
  curNPC = manifest(curNPC,getValue(),getValueType());
  curState.npc(curNPC);
  curInst->pcState(curState);
  check4reschedule();
}

//Process for taken flag
void Fetch_DecodeFault::taken_process(dynInstPtr &curInst)
{
  bool taken_flag = curInst->readPredTaken();
  taken_flag = manifest_bool(taken_flag,getValue(),getValueType());
  curInst->setPredTaken(taken_flag);
  check4reschedule();
}

//Class Fetch_Decode Fault Queue
Fetch_DecodeFault *Fetch_DecodeFaultQueue::scan(string s, ThreadEnabledFault &thisThread, Addr vaddr, int curBundle, int fieldType)
{
  static uint64_t called=0;
  Fetch_DecodeFault *p;
  int i;
  uint64_t exec_time = 0;
  uint64_t exec_instr = 0;
  unsigned char flag = 1;
  p = reinterpret_cast<Fetch_DecodeFault *> (head);
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
    p = reinterpret_cast<Fetch_DecodeFault*> (p->nxt);
  }
  return(NULL);

}

