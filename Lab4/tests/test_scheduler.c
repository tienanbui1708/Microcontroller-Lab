#include <stdio.h>
#include <assert.h>
#include "scheduler.h"
static int log_id[1000], log_t[1000], n;
#define T(k) static void t##k(void){log_id[n]=k;log_t[n++]=SCH_Get_Time();}
T(1) T(2) T(3) T(4) T(5)
static void tick(int k){while(k--){SCH_Update();SCH_Dispatch_Tasks();}}
static int count(int id){int c=0;for(int i=0;i<n;i++)c+=log_id[i]==id;return c;}
int main(void){
  /* 5 periodic tasks */
  SCH_Init(); n=0;
  SCH_Add_Task(t1,500,500);SCH_Add_Task(t2,1000,1000);SCH_Add_Task(t3,1500,1500);
  SCH_Add_Task(t4,2000,2000);SCH_Add_Task(t5,2500,2500);
  tick(1500); /* 15 s */
  assert(count(1)==30&&count(2)==15&&count(3)==10&&count(4)==7&&count(5)==6);
  for(int i=0;i<n;i++) assert(log_t[i]%(log_id[i]*500)==0);
  for(int i=1;i<n;i++) assert(log_t[i]>=log_t[i-1]);
  /* equal due times keep registration order, e.g. 20000 ms: T1 T2 T4 T5 */
  for(int i=1;i<n;i++) if(log_t[i]==log_t[i-1]) assert(log_id[i]>log_id[i-1]);
  /* same due time -> registration order */
  SCH_Init(); n=0; SCH_Add_Task(t1,100,0);SCH_Add_Task(t2,100,0);SCH_Add_Task(t3,50,0);
  tick(10); assert(n==3&&log_id[0]==3&&log_id[1]==1&&log_id[2]==2&&log_t[0]==50&&log_t[1]==100);
  /* one-shot frees slot, delete, stale ID */
  SCH_Init(); n=0; uint32_t a=SCH_Add_Task(t1,0,0); tick(1); assert(n==1&&log_t[0]==10);
  assert(SCH_Delete_Task(a)==0);
  uint32_t b=SCH_Add_Task(t2,200,0); assert(SCH_Delete_Task(a)==0);
  uint32_t c=SCH_Add_Task(t3,300,0); assert(SCH_Delete_Task(b)==1); assert(SCH_Delete_Task(b)==0);
  tick(40); assert(n==2&&log_id[1]==3&&log_t[1]==310); (void)c;
  /* late dispatch: ticks while head overdue are not lost */
  SCH_Init(); n=0; SCH_Add_Task(t1,100,100); SCH_Add_Task(t2,150,0);
  for(int i=0;i<12;i++) SCH_Update();  /* main loop busy for 120 ms */
  SCH_Dispatch_Tasks(); tick(8); /* now 200 ms */
  assert(count(1)==2 && count(2)==1); assert(log_t[n-1]==200 && log_id[n-1]==1);
  /* full queue */
  SCH_Init(); for(unsigned i=0;i<SCH_MAX_TASKS;i++) assert(SCH_Add_Task(t1,1000,0)!=SCH_NO_TASK_ID);
  assert(SCH_Add_Task(t1,1000,0)==SCH_NO_TASK_ID);
  /* delete head mid-flight keeps others on time */
  SCH_Init(); n=0; uint32_t h=SCH_Add_Task(t1,100,0); SCH_Add_Task(t2,250,0);
  tick(5); SCH_Delete_Task(h); tick(20); assert(n==1&&log_id[0]==2&&log_t[0]==250);
  puts("all scheduler tests passed"); return 0;}
