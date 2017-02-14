#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include "../ext/dmtcp_2.4/include/dmtcp.h"
#include "cpu/o3/cpu.hh"
#include "cpu/base.hh"

#include "fi/faultq.hh"
#include "fi/cpu_threadInfo.hh"
#include "fi/fi_system.hh"
#include "fi/o3cpu_injfault.hh"
#include "fi/cpu_injfault.hh"
#include "fi/genfetch_injfault.hh"
#include "fi/iew_injfault.hh"
// #include "fi/mem_injfault.hh"
#include "fi/opcode_injfault.hh"
#include "fi/pc_injfault.hh"
#include "fi/regdec_injfault.hh"
#include "fi/reg_injfault.hh"
#include "fi/loadstore_injfault.hh"
#include "sim/pseudo_inst.hh"
#include "arch/vtophys.hh"

#include "config/the_isa.hh"
#include "base/types.hh"
#include "arch/types.hh"
#include "arch/registers.hh"
#include "base/trace.hh"

#include "mem/mem_object.hh"


#include <sys/time.h>  

// #include <dmtcpaware.h>

//SPEC_FI
#include "debug/Golden_Run.hh"
#include "debug/SPEC_FI.hh"
#include "debug/IVM_FI.hh"
//SPEC_FI


using namespace std;

unsigned fi_IntRegs;
unsigned fi_FloatRegs;
unsigned fi_CCRegs;

Fi_System *fi_system;

  Fi_System::Fi_System(Params *p)
:MemObject(p)
{

  std:: stringstream s1;
  in_name = p->input_fi;
  meta_file=p->meta_file;
  text_start=p->text_start;
  setcheck(p->check_before_init);
  setCheckBeforeFI(p->checkBeforeFI);
  setswitchcpu(p->fi_switch);
  fi_enable = 0;
  fi_system = this;
  setmaincheckpoint(false);
  mainInjectedFaultQueue.setName("MainFaultQueue");
  mainInjectedFaultQueue.setHead(NULL);
  mainInjectedFaultQueue.setTail(NULL);
  fetchStageInjectedFaultQueue.setName("FetchStageFaultQueue");
  fetchStageInjectedFaultQueue.setHead(NULL);
  fetchStageInjectedFaultQueue.setTail(NULL);
  decodeStageInjectedFaultQueue.setName("DecodeStageFaultQueue");
  decodeStageInjectedFaultQueue.setHead(NULL);
  decodeStageInjectedFaultQueue.setTail(NULL);
  iewStageInjectedFaultQueue.setName("IEWStageFaultQueue");
  iewStageInjectedFaultQueue.setHead(NULL);
  iewStageInjectedFaultQueue.setTail(NULL);
  LoadStoreInjectedFaultQueue.setName("LoadStoreFaultQueue");
  LoadStoreInjectedFaultQueue.setHead(NULL);
  LoadStoreInjectedFaultQueue.setTail(NULL);
  //Pipeline registers
  fetch_DecodeFaultQueue.setName("Fetch_DecodeFaultQueue");
  fetch_DecodeFaultQueue.setHead(NULL);
  fetch_DecodeFaultQueue.setTail(NULL);
  decode_RenameFaultQueue.setName("Decode_RenameFaultQueue");
  decode_RenameFaultQueue.setHead(NULL);
  decode_RenameFaultQueue.setTail(NULL);
  rename_IssueFaultQueue.setName("Rename_IssueFaultQueue");
  rename_IssueFaultQueue.setHead(NULL);
  rename_IssueFaultQueue.setTail(NULL);
  issue_ExecuteFaultQueue.setName("Issue_ExecuteFaultQueue");
  issue_ExecuteFaultQueue.setHead(NULL);
  issue_ExecuteFaultQueue.setTail(NULL);
  wb_CommitFaultQueue.setName("WB_CommitFaultQueue");
  wb_CommitFaultQueue.setHead(NULL);
  wb_CommitFaultQueue.setTail(NULL);
  execute_WBFaultQueue.setName("Execute_WBFaultQueue");
  execute_WBFaultQueue.setHead(NULL);
  execute_WBFaultQueue.setTail(NULL);
  //End of Pipeline registers
  fi_execute = false;
  fi_decode= false; 
  fi_fetch= false;
  fi_loadstore=false;
  fi_main= false;
  //Pipeline registers
  fi_fetch_decode= false;
  fi_decode_rename = false;
  fi_rename_issue = false;
  fi_issue_execute = false;
  fi_wb_commit = false;
  fi_execute_wb = false;
  //End of Pipeline registers
  
  allthreads = NULL;

  if(in_name.size() > 1){
    input.open (in_name.c_str(), ifstream::in);
    getFromFile(input);
    input.close();

  //SPEC_FI
  enable_trace = false;
  //SPEC_FI
  }




}
Fi_System::~Fi_System(){

}

  void
Fi_System::init()
{

  if (DTRACE(FaultInjection)) {
    std::cout << "Fi_System:init()\n";
  }
}


void 
Fi_System:: dump(){
  InjectedFault *p;

  if (DTRACE(FaultInjection)) {
    std::cout <<"===Fi_System::dump()===\n";
    std::cout << "Input: " << in_name << "\n";
    std::cout<<  "MetaData Input " << meta_file << "Is EMTPY : "<<meta_file.empty()<<"\n";
    for(fi_activation_iter = fi_activation.begin(); fi_activation_iter != fi_activation.end(); ++fi_activation_iter){
      fi_activation_iter->second->dump();
    }


    p=mainInjectedFaultQueue.head;
    while(p){

      p->dump();
      p=p->nxt;
    }

    p=fetchStageInjectedFaultQueue.head;
    while(p){
      p->dump();
      p=p->nxt;
    }

    p=decodeStageInjectedFaultQueue.head;
    while(p){
      p->dump();
      p=p->nxt;
    }

    p=iewStageInjectedFaultQueue.head;
    while(p){
      p->dump();
      p=p->nxt;
    }

    p=LoadStoreInjectedFaultQueue.head;
    while(p){
      p->dump();
      p=p->nxt;
    }
    //Pipeline Registers
    p=fetch_DecodeFaultQueue.head;
    while(p){
      p->dump();
      p=p->nxt;
    } 

    p=decode_RenameFaultQueue.head;
    while(p){
      p->dump();
      p=p->nxt;
    }  
         
    p=rename_IssueFaultQueue.head;
    while(p){
      p->dump();
      p=p->nxt;
    }  

    p=issue_ExecuteFaultQueue.head;
    while(p){
      p->dump();
      p=p->nxt;
    }

    p=wb_CommitFaultQueue.head;
    while(p){
      p->dump();
      p=p->nxt;
    }

    p=execute_WBFaultQueue.head;
    while(p){
      p->dump();
      p=p->nxt;
    }  
    //Pipeline Registers    
    std::cout <<"~===Fi_System::dump()===\n"; 

  }
}

  void
Fi_System::startup()
{
  if (DTRACE(FaultInjection)) {
    std::cout << "Fi_System:startup()\n";
  }
  dump();
}

  Fi_System *
Fi_SystemParams::create()
{
  if (DTRACE(FaultInjection)) {
    std::cout << "Fi_System:create()\n";
  }
  return new Fi_System(this);
}

  Port *
Fi_System::getPort(const string &if_name, int idx)
{
  std::cout << "Fi_System:getPort() " << "if_name: " << if_name << " idx: " << idx <<  "\n";
  panic("No Such Port\n");
}

//Initialize faults from a file
//Note that the conditions of how the faults are
//stored in a file are very strict.

void
Fi_System:: getFromFile(std::ifstream &os){
  string check;

  while(os.good()){
    os>>check;		
    if(check.compare("CPUInjectedFault") ==0){
      new CPUInjectedFault(os);
    }
    else if(check.compare("InjectedFault") ==0){
      new InjectedFault(os);
    }
    else if(check.compare("GeneralFetchInjectedFault") ==0){
      new GeneralFetchInjectedFault(os);
    }
    else if(check.compare("IEWStageInjectedFault") ==0){
      new IEWStageInjectedFault(os);
    }
    // 		else if(check.compare("MemoryInjectedFault") ==0){
    // 			new MemoryInjectedFault(os);
    // 		}
    else if(check.compare("O3CPUInjectedFault") ==0){
      new O3CPUInjectedFault(os);
    }
    else if(check.compare("OpCodeInjectedFault") ==0){
      new OpCodeInjectedFault(os);
    }
    else if(check.compare("PCInjectedFault") ==0){
      new PCInjectedFault(os);
    }
    else if(check.compare("RegisterInjectedFault") ==0){
      new RegisterInjectedFault(os);
    }
    else if(check.compare("RegisterDecodingInjectedFault") ==0){
      new RegisterDecodingInjectedFault(os);
    }
    else if(check.compare("LoadStoreInjectedFault")==0){
      new LoadStoreInjectedFault(os);
    }
    //Pipeline Registers
    //Recognition of added faults
    else if(check.compare("Fetch_DecodeFault")==0){
      new Fetch_DecodeFault(os,false);
    }
    else if(check.compare("Decode_RenameFault")==0){
      new Decode_RenameFault(os,false);
    }
    else if(check.compare("Rename_IssueFault")==0){
      new Rename_IssueFault(os,false);
    }
    else if(check.compare("Issue_ExecuteFault")==0){
      new Issue_ExecuteFault(os,false);
    }
    else if(check.compare("WB_CommitFault")==0){
      new WB_CommitFault(os,false);
    }
    else if(check.compare("Execute_WBFault")==0){
      new Execute_WBFault(os,false);
    }
    //End of Pipeline registers
    else{
      if (DTRACE(FaultInjection)) {
        std::cout << "No such Object: "<<check<<"\n";
      }
    }
  }
}




//delete all info and restart from the begining

void 
Fi_System::delete_faults(){

  while(!mainInjectedFaultQueue.empty())
    mainInjectedFaultQueue.remove(mainInjectedFaultQueue.head);

  while(!fetchStageInjectedFaultQueue.empty())
    fetchStageInjectedFaultQueue.remove(fetchStageInjectedFaultQueue.head);

  while(!decodeStageInjectedFaultQueue.empty())
    decodeStageInjectedFaultQueue.remove(decodeStageInjectedFaultQueue.head);

  while(!iewStageInjectedFaultQueue.empty())
    iewStageInjectedFaultQueue.remove(iewStageInjectedFaultQueue.head);

  while(!LoadStoreInjectedFaultQueue.empty())
    LoadStoreInjectedFaultQueue.remove(LoadStoreInjectedFaultQueue.head);
  //Pipeline Registers
  while(!fetch_DecodeFaultQueue.empty())
    fetch_DecodeFaultQueue.remove(fetch_DecodeFaultQueue.head);
  while(!decode_RenameFaultQueue.empty()){
    decode_RenameFaultQueue.remove(decode_RenameFaultQueue.head);
  }
  while(!rename_IssueFaultQueue.empty()){
    rename_IssueFaultQueue.remove(rename_IssueFaultQueue.head);  
  }
  while(!issue_ExecuteFaultQueue.empty()){
    issue_ExecuteFaultQueue.remove(issue_ExecuteFaultQueue.head);
  }
  while(!wb_CommitFaultQueue.empty()){
    wb_CommitFaultQueue.remove(wb_CommitFaultQueue.head);
  }
  while(!execute_WBFaultQueue.empty()){
    execute_WBFaultQueue.remove(execute_WBFaultQueue.head);
  }
  //End of Pipeline Registers
  mainInjectedFaultQueue.setName("MainFaultQueue");
  mainInjectedFaultQueue.setHead(NULL);
  mainInjectedFaultQueue.setTail(NULL);
  fetchStageInjectedFaultQueue.setName("FetchStageFaultQueue");
  fetchStageInjectedFaultQueue.setHead(NULL);
  fetchStageInjectedFaultQueue.setTail(NULL);
  decodeStageInjectedFaultQueue.setName("DecodeStageFaultQueue");
  decodeStageInjectedFaultQueue.setHead(NULL);
  decodeStageInjectedFaultQueue.setTail(NULL);
  iewStageInjectedFaultQueue.setName("IEWStageFaultQueue");
  iewStageInjectedFaultQueue.setHead(NULL);
  iewStageInjectedFaultQueue.setTail(NULL);
  LoadStoreInjectedFaultQueue.setName("LoadStoreFaultQueue");
  LoadStoreInjectedFaultQueue.setHead(NULL);
  LoadStoreInjectedFaultQueue.setTail(NULL);
  
  //Pipeline Registers
  fetch_DecodeFaultQueue.setName("Fetch_DecodeFaultQueue");
  fetch_DecodeFaultQueue.setHead(NULL);
  fetch_DecodeFaultQueue.setTail(NULL);
  decode_RenameFaultQueue.setName("Decode_RenameFaultQueue");
  decode_RenameFaultQueue.setHead(NULL);
  decode_RenameFaultQueue.setTail(NULL);
  rename_IssueFaultQueue.setName("Rename_IssueFaultQueue");
  rename_IssueFaultQueue.setHead(NULL);
  rename_IssueFaultQueue.setTail(NULL);
  issue_ExecuteFaultQueue.setName("Issue_ExecuteFaultQueue");
  issue_ExecuteFaultQueue.setHead(NULL);
  issue_ExecuteFaultQueue.setTail(NULL);
  wb_CommitFaultQueue.setName("WB_CommitFaultQueue");
  wb_CommitFaultQueue.setHead(NULL);
  wb_CommitFaultQueue.setTail(NULL);
  execute_WBFaultQueue.setName("Execute_WBFaultQueue");
  execute_WBFaultQueue.setHead(NULL);
  execute_WBFaultQueue.setTail(NULL);
  //End of Pipeline Registers
}


  void
Fi_System:: reset()
{

  std:: stringstream s1;

  delete_faults();

  if(in_name.size() > 1){
    if (DTRACE(FaultInjection)) {
      std::cout << "Fi_System::Reading New Faults \n";
    }
    input.open (in_name.c_str(), ifstream::in);
    getFromFile(input);
    input.close();

    if (DTRACE(FaultInjection)) {
      std::cout << "~Fi_System::Reading New Faults \n";
    }
    dump();
  }
}



int 
Fi_System:: get_fi_fetch_counters( InjectedFault *p , ThreadEnabledFault &thread,std::string curCpu , uint64_t *fetch_instr , uint64_t *fetch_time ){

  *fetch_time=0;
  *fetch_instr=0;

  // Case :: specific cpu ---- specific thread
  if((p->getWhere().compare(curCpu))==0 && (p->getThread()).compare("all") != 0 && thread.getThreadId() == atoi( (p->getThread()).c_str() ) ){ // case thread_id - cpu_id
    thread.CalculateFetchedTime(curCpu,fetch_instr,fetch_time);
  }//Case :: ALL cores --- specific Thread
  else if((p->getWhere().compare("all") == 0) && (p->getThread()).compare("all") != 0 && thread.getThreadId() == atoi( (p->getThread()).c_str() )){// case thread_id - all
    thread.CalculateFetchedTime("all",fetch_instr,fetch_time);
  }//Case :: Specific Cpu --- All threads
  else if( ( (p->getWhere().compare(curCpu)) == 0  ) && (((p->getThread()).compare("all")) == 0)  ){ //case cpu_id - all
    allthreads->CalculateFetchedTime(curCpu,fetch_instr,fetch_time);
  }//Case:: All cores --- All threads
  else if( ((p->getThread()).compare("all") == 0) && ((p->getWhere().compare("all")) == 0) ){ //case all - all
    allthreads->CalculateFetchedTime("all",fetch_instr,fetch_time);
  }

  if(*fetch_time|*fetch_instr){
    if(p->getFaultType() == p->RegisterInjectedFault || p->getFaultType() == p->PCInjectedFault || p->getFaultType() == p->MemoryInjectedFault  ){
      p->setCPU(reinterpret_cast<BaseCPU *>(find(curCpu.c_str())));// I may manifest during this cycle so se the core.
      return 1;
    }//Pipeline Registers:Addition of comparison for new faults during CPU setting
    else if(p->getFaultType() == p->GeneralFetchInjectedFault || p->getFaultType() == p->OpCodeInjectedFault ||
        p->getFaultType() == p->RegisterDecodingInjectedFault || p->getFaultType() == p->ExecutionInjectedFault ||
        p->getFaultType() == p->Fetch_DecodeFault ||
        p->getFaultType() == p->Decode_RenameFault ||
        p->getFaultType() == p->Rename_IssueFault ||
        p->getFaultType() == p->Issue_ExecuteFault ||
        p->getFaultType() == p->WB_CommitFault ||
        p->getFaultType() == p->Execute_WBFault){
      O3CPUInjectedFault *k = reinterpret_cast<O3CPUInjectedFault*> (p);
      k->setCPU(reinterpret_cast<BaseO3CPU *>(find(curCpu.c_str())));// I may manifest during this cycle so se the core.
      return 2;
    }
  }
  return 0;
}


int 
Fi_System:: get_fi_decode_counters( InjectedFault *p , ThreadEnabledFault &thread,std::string curCpu , uint64_t *fetch_instr , uint64_t *fetch_time ){

  *fetch_time=0;
  *fetch_instr=0;
  int val = 0;
  // Case :: specific cpu ---- specific thread
  if((p->getWhere().compare(curCpu))==0 && (p->getThread()).compare("all") != 0 && thread.getThreadId() == atoi( (p->getThread()).c_str() ) ){ // case thread_id - cpu_id
    thread.CalculateDecodedTime(curCpu,fetch_instr,fetch_time);
    val=1;
  }//Case :: ALL cores --- specific Thread
  else if((p->getWhere().compare("all") == 0) && (p->getThread()).compare("all") != 0 && thread.getThreadId() == atoi( (p->getThread()).c_str() )){// case thread_id - all
    thread.CalculateDecodedTime("all",fetch_instr,fetch_time);
    val =2;
  }//Case :: Specific Cpu --- All threads
  else if( ( (p->getWhere().compare(curCpu)) == 0  ) && (((p->getThread()).compare("all")) == 0)  ){ //case cpu_id - all
    allthreads->CalculateDecodedTime(curCpu,fetch_instr,fetch_time);
    val=3;
  }//Case:: All cores --- All threads
  else if( ((p->getThread()).compare("all") == 0) && ((p->getWhere().compare("all")) == 0) ){ //case all - all
    allthreads->CalculateDecodedTime("all",fetch_instr,fetch_time);
    val = 4;
  }
  if(*fetch_time | *fetch_instr){
    if(p->getFaultType() == p->RegisterInjectedFault || p->getFaultType() == p->PCInjectedFault || p->getFaultType() == p->MemoryInjectedFault){
      p->setCPU(reinterpret_cast<BaseCPU *>(find(curCpu.c_str()))); // I may manifest during this cycle so se the core.
      return val;
    }
    else if(p->getFaultType() == p->GeneralFetchInjectedFault || p->getFaultType() == p->OpCodeInjectedFault ||
        p->getFaultType() == p->RegisterDecodingInjectedFault || p->getFaultType() == p->ExecutionInjectedFault){
      O3CPUInjectedFault *k = reinterpret_cast<O3CPUInjectedFault*> (p);
      BaseO3CPU *v = reinterpret_cast<BaseO3CPU *>(find(curCpu.c_str())); // I may manifest during this cycle so se the core.
      k->setCPU(v);
      return val;
    }
  }
  return val;
}


int 
Fi_System:: get_fi_exec_counters( InjectedFault *p , ThreadEnabledFault &thread,std::string curCpu , uint64_t *exec_instr, uint64_t *exec_time  ){

  *exec_time=0;
  *exec_instr=0;
  if((p->getWhere().compare(curCpu))==0 && (p->getThread()).compare("all") != 0 && thread.getThreadId() == atoi( (p->getThread()).c_str() ) ){ // case thread_id - cpu_id
    thread.CalculateExecutedTime(curCpu,exec_instr,exec_time);
  }
  else if((p->getWhere().compare("all") == 0) && (p->getThread()).compare("all") != 0 && thread.getThreadId() == atoi( (p->getThread()).c_str() )){// case thread_id - all
    thread.CalculateExecutedTime("all",exec_instr,exec_time);
  }
  else if( ( (p->getWhere().compare(curCpu)) == 0  ) && (((p->getThread()).compare("all")) == 0)  ){ //case cpu_id - all
    allthreads->CalculateExecutedTime(curCpu,exec_instr,exec_time);
  }
  else if( ((p->getThread()).compare("all") == 0) && ((p->getWhere().compare("all")) == 0) ){ //case all - all
    allthreads->CalculateExecutedTime("all",exec_instr,exec_time);
  }
  if(*exec_time | *exec_instr){
    if(p->getFaultType() == p->RegisterInjectedFault || p->getFaultType() == p->PCInjectedFault || p->getFaultType() == p->MemoryInjectedFault){
      p->setCPU(reinterpret_cast<BaseCPU *>(find(curCpu.c_str()))); // I may manifest during this cycle so se the core.
      return 1;
    }
    else if(p->getFaultType() == p->GeneralFetchInjectedFault || p->getFaultType() == p->OpCodeInjectedFault ||
        p->getFaultType() == p->RegisterDecodingInjectedFault || p->getFaultType() == p->ExecutionInjectedFault){
      O3CPUInjectedFault *k = reinterpret_cast<O3CPUInjectedFault*> (p);
      BaseO3CPU *v = reinterpret_cast<BaseO3CPU *>(find(curCpu.c_str())); // I may manifest during this cycle so se the core.
      k->setCPU(v);
      return 2;
    }
  }
  return 0;
}

int 
Fi_System:: get_fi_loadstore_counters( InjectedFault *p , ThreadEnabledFault &thread,std::string curCpu , uint64_t *exec_instr, uint64_t *exec_time  ){

  *exec_time=0;
  *exec_instr=0;
  if((p->getWhere().compare(curCpu))==0 && (p->getThread()).compare("all") != 0 && thread.getThreadId() == atoi( (p->getThread()).c_str() ) ){ // case thread_id - cpu_id
    thread.CalculateLoadStoreTime(curCpu,exec_instr,exec_time);
  }
  else if((p->getWhere().compare("all") == 0) && (p->getThread()).compare("all") != 0 && thread.getThreadId() == atoi( (p->getThread()).c_str() )){// case thread_id - all
    thread.CalculateLoadStoreTime("all",exec_instr,exec_time);
  }
  else if( ( (p->getWhere().compare(curCpu)) == 0  ) && (((p->getThread()).compare("all")) == 0)  ){ //case cpu_id - all
    allthreads->CalculateLoadStoreTime(curCpu,exec_instr,exec_time);
  }
  else if( ((p->getThread()).compare("all") == 0) && ((p->getWhere().compare("all")) == 0) ){ //case all - all
    allthreads->CalculateLoadStoreTime("all",exec_instr,exec_time);
  }
  if(*exec_time | *exec_instr){
    if(p->getFaultType() == p->RegisterInjectedFault || p->getFaultType() == p->PCInjectedFault || p->getFaultType() == p->MemoryInjectedFault){
      p->setCPU(reinterpret_cast<BaseCPU *>(find(curCpu.c_str()))); // I may manifest during this cycle so se the core.
      return 1;
    }
    else if(p->getFaultType() == p->GeneralFetchInjectedFault || p->getFaultType() == p->OpCodeInjectedFault ||
        p->getFaultType() == p->RegisterDecodingInjectedFault || p->getFaultType() == p->ExecutionInjectedFault ||
        p->getFaultType() == p->LoadStoreInjectedFault){
      O3CPUInjectedFault *k = reinterpret_cast<O3CPUInjectedFault*> (p);
      BaseO3CPU *v = reinterpret_cast<BaseO3CPU *>(find(curCpu.c_str())); // I may manifest during this cycle so se the core.
      k->setCPU(v);
      return 2;
    }
  }
  return 0;
}

int 
Fi_System::increaseTicks(std :: string curCpu , ThreadEnabledFault *curThread , uint64_t ticks){
  curThread->increaseTicks(curCpu, ticks);
  allthreads->increaseTicks(curCpu, ticks);
  return 1;
}



void
Fi_System::scheduleswitch(ThreadContext *tc){
  if(getswitchcpu()){
    Fi_SystemEvent *switching = new Fi_SystemEvent(tc);
    switching->setticks(curTick());
    Tick when = curTick() + 10000 * SimClock::Int::ns;
    mainEventQueue[0]->schedule(switching,when,true);
    DPRINTF(FaultInjection,"EVENT IS scheduled for %i\n",when);
    setswitchcpu(false);
  }
}


void Fi_System::start_fi(ThreadContext *tc,  uint64_t threadid){
  Addr _tmpAddr  = TheISA::getFiThread(tc);
  fi_activation_iter = fi_activation.find(_tmpAddr);
  if (fi_activation_iter == fi_activation.end()  ) {
    std::string _name = tc->getCpuPtr()->name();
    if(allthreads == NULL){
      allthreads = new ThreadEnabledFault(-1, _name);
    }
    DPRINTF(FaultInjection,"==Fault Injection Activation Instruction===\n");
    DPRINTF(FaultInjection,"== Addr %llx   =============================\n",_tmpAddr);
    ThreadEnabledFault *thread = new ThreadEnabledFault(threadid,_name);
    fi_activation[_tmpAddr] = thread;
    tc->setEnabledFIThread( thread );
    thread->setMode(START);
    tc->setEnabledFI(true);
    fi_enable++;
    fi_execute = fi_enable & (!iewStageInjectedFaultQueue.empty());
    fi_decode= fi_enable & (!decodeStageInjectedFaultQueue.empty());
    fi_fetch= fi_enable & (!fetchStageInjectedFaultQueue.empty());
    fi_loadstore= fi_enable & (!LoadStoreInjectedFaultQueue.empty());
    fi_main= fi_enable & (!mainInjectedFaultQueue.empty() );
    //Pipeline Registers
    //How to enable fi flags?
    fi_fetch_decode= fi_enable &(!fetch_DecodeFaultQueue.empty());
    fi_decode_rename= fi_enable &(!decode_RenameFaultQueue.empty());
    fi_rename_issue= fi_enable &(!rename_IssueFaultQueue.empty());
    fi_issue_execute = fi_enable &(!issue_ExecuteFaultQueue.empty());
    fi_wb_commit = fi_enable &(!wb_CommitFaultQueue.empty());
    fi_execute_wb = fi_enable &(!execute_WBFaultQueue.empty());
    //Pipeline Registers
    DPRINTF(FaultInjection,"~==Fault Injection Activation Instruction===\n");
  }
  else{
    if	(fi_activation_iter->second->getMode() == PAUSE ){
      fi_enable++;
      fi_activation_iter->second->setMode(START);
      fi_activation_iter->second->setfaulty(0);
      tc->setEnabledFIThread(fi_activation_iter->second);
      tc->setEnabledFI(true);
      fi_activation_iter->second->setThreadId(threadid);
    }
    else{
      DPRINTF(FaultInjection,"I have already enabled fault injection I am going to ignore this request\n");
    }
  }
}


void Fi_System::pause_fi(ThreadContext *tc,uint64_t threadid)
{
  static int number_of_pauses=0;
  Addr _tmpAddr  = TheISA::getFiThread(tc);
  fi_activation_iter = fi_activation.find(_tmpAddr);
  if (fi_activation_iter == fi_activation.end()) {
    DPRINTF(FaultInjection,"I have not enabled fault injection going to ignore stop request\n");
  }
  else{ 
    fi_activation_iter->second->setMode(PAUSE);
    tc->setEnabledFI(false);
    fi_enable--;
    tc->setEnabledFIThread(NULL);
  }
  if( (number_of_pauses++)%100 == 99){
    DPRINTF(FaultInjection,"Paused one more time %d\n",number_of_pauses);
  }
}

void Fi_System:: stop_fi(ThreadContext *tc, uint64_t req){
  if(DTRACE(FaultInjection))
  {
    std::cout<<"==Fault Injection Deactivation Instruction===\n";
  }

  Addr _tmpAddr  = TheISA::getFiThread(tc);
  DPRINTF(FaultInjection, "\t Process Control Block(PCB) Addressx: %llx ####%d#####\n",_tmpAddr,threadid);  
  fi_activation_iter = fi_activation.find(_tmpAddr);

  if (fi_activation_iter != fi_activation.end()) {
    fi_activation[_tmpAddr]->print_time();
    tc->setEnabledFI(false);
    tc->setEnabledFIThread(NULL);
    fi_activation.erase(fi_activation_iter);
    DPRINTF(FaultInjection,"~===Fault Injection Deactivation Instruction===\n");
    fi_enable--;
  }
}


void Fi_System::dump_fi(ThreadContext *tc){
  DPRINTF(FaultInjection, "Dumping number of instructions\n");
  for( fi_activation_iter = fi_activation.begin(); fi_activation_iter!=fi_activation.end() ; ++fi_activation_iter){
    DPRINTF(FaultInjection, " Thread ID : %llx\n",fi_activation_iter->first);
    fi_activation_iter->second->print_time();
  }
  fi_activation.clear();
  tc->setEnabledFIThread(NULL);
  tc->setEnabledFI(false);
  fi_enable = 0;

  if(getswitchcpu())
    scheduleswitch(tc);
}


void Fi_System::rename_ckpt(const char* new_name){
  int num_checkpoints,num_restored;
  dmtcp_get_local_status(&num_checkpoints,&num_restored);
  const char *path = dmtcp_get_ckpt_filename();
  if (path == NULL)
    DPRINTF(FaultInjection,"PATH IS NULL\n");
  std::string new_path(path);
  int count = new_path.find_last_of("/");

  if(!(rename(new_path.substr(count+1).c_str(),new_name)))
    DPRINTF(FaultInjection," Checkpoint created (%s)\n",new_name);
  else
    cout<<"Error \n"; 
}


int Fi_System::checkpointOnFault(){
  if ( getCheckBeforeFI () )
    return dmtcp_checkpoint();
  return 1;
}

//SPEC_FI
int 
Fi_System::increaseTicks(std :: string curCpu , ThreadEnabledFault *curThread , uint64_t ticks,ThreadContext *tc){
  curThread->increaseTicks(curCpu, ticks);
  allthreads->increaseTicks(curCpu, ticks);
  SPECFI_timing(tc,curThread,curCpu);
  return 1;
}

void Fi_System::SPECFI_timing(ThreadContext *tc,ThreadEnabledFault *curThread,std::string curCpu)
{
  if(Debug::Golden_Run){
    if(curThread->getTicks("all")==10000){
      golden_trace.open("m5out/golden_trace.txt");
      enable_trace = true;
    }
    if(curThread->getTicks("all")==35000){
      enable_trace = false;
      golden_trace.close();
      if(Debug::IVM_FI){
        PseudoInst::m5exit(tc,4000);
      }
      PseudoInst::switchcpu(tc);
      stop_fi(tc,0);
    }
  }

  if(Debug::SPEC_FI){
    if(curThread->getTicks(curCpu)==10000){
      fi_trace.open("m5out/fi_trace.txt");
      enable_trace = true;
    }
    if(curThread->getTicks(curCpu)==35000){
      fi_trace.close();
      int arch_res = check_arch();
      std::ofstream arch_check;
      arch_check.open("m5out/arch_check.txt");
      if(arch_res==0){
        arch_check<<"SDC";
        arch_check.close();
        if(Debug::IVM_FI){
          PseudoInst::m5exit(tc,4000);
        }
        PseudoInst::switchcpu(tc);
        stop_fi(tc,0);
      }
      if(arch_res==1){
        arch_check<<"Timing";
        arch_check.close();
        if(Debug::IVM_FI){
          PseudoInst::m5exit(tc,4000);
        }
        PseudoInst::switchcpu(tc);
        stop_fi(tc,0);
      }
      if(arch_res==2){
        arch_check<<"Masked:10000";
        arch_check.close();
        PseudoInst::m5exit(tc,0);
      }
    }
  }
}

int Fi_System::check_arch()
{
  std::ifstream bench_input;
  bench_input.open("SPEC_FI/bench2check.txt");
  if(bench_input.good()){
    if(bench_input.peek()!=bench_input.eof()){
      std::string bench2check,golden_line,fi_line;
      getline(bench_input,bench2check);
      bench_input.close();
      std::ifstream golden_check,fi_check;
      golden_check.open("SPEC_FI/"+bench2check+".GT");
      fi_check.open("m5out/fi_trace.txt");

      while(getline(golden_check,golden_line)){
        if(!getline(fi_check,fi_line)){
          return 0;
        }
        std::string fi_time,golden_time;
        fi_time = fi_line.substr(0,13);
        golden_time = golden_line.substr(0,13);
        fi_line = fi_line.substr(15);
        golden_line = golden_line.substr(15);
        if(golden_line != fi_line){
          return 0;
        }
        else if(golden_time != fi_time){
          return 1;
        }
        return 2;
      }
    }
    else{
       if(Debug::FaultInjection){
        cout<<"Empty File.Any benchmark indicated"<<endl;
       }
    }
  }
  else{
    cout<<"Corrupted Input benchmark File"<<endl;
  }
  return 0;
}
//SPEC_FI
