#include <iostream>
#include <random>
#include <fstream>
#include <vector>
#include "rapidcsv.h"

#define HIGH 0.000005
#define LOW -0.000005

using namespace std;


void csv_write(int start_ind, vector<float> time, vector<float> rin, vector<float> lin, vector<float> lout){
   fstream fout;
   fout.open("processed_buffer.csv", ios::out | ios::app);
   fout << "time,\"I(RIN)\",\"I(LIN|XI4)\",\"I(LOUT|XI4)\"" << "\n";
   for (int i = start_ind; i < lout.size(); i++){
      fout << time[i] << "," << rin[i] << "," << lin[i] << "," << lout[i] << "\n";
   }
}



int local_max(vector<float> signal){
  float max = INT_MIN;
  int maxind = 900;
  for (int i = 900; i < 1400; i++){
    if (signal[i] > max){
      max = signal[i];
      maxind = i;
    }
  }
  
  return maxind;
}

int find_stability_point(vector<float> sig_in){
  int last_switch;
  float tol = 2e-06;
  int state = 0; //low
  for (int i = 0; i < sig_in.size(); i++){
    if (state == 0){
      if (sig_in[i] > HIGH-tol){
        state = 1;
        last_switch = i;
      }
    }
    else {
      if (sig_in[i] < LOW+tol){
        state = 0;
        last_switch = i;
      }
    }
  }
  return last_switch;
}

bool satisfies_BE(float Iout, float Iin){
  return (Iout*Iin > 0) && (abs(Iout)>abs(Iin)) ? true: false;  //condition for no bit error
}

int main() {

long total_events = 0;
long total_correct = 0; 

// run josim x times
// process output file:

// read csv where we want into 3 1d vectors 
rapidcsv::Document josimOut("examples/ex_pi_DQFP_buffer_chan.csv");
std::vector<float> time = josimOut.GetColumn<float>("time");
std::vector<float> rIn = josimOut.GetColumn<float>("I(RIN)");
std::vector<float> Lin = josimOut.GetColumn<float>("I(LIN|XI4)");
std::vector<float> Lout = josimOut.GetColumn<float>("I(LOUT|XI4)");
int timeSize = Lout.size();
std::cout << timeSize << std::endl;

// find local max ind running time of lin:x4 after the 1.8xe-10 mark so around the 900th instance 
int local_max_ind_lin = local_max(Lin), local_max_ind_lout = local_max(Lout);
cout << "Lin first local_max = " << local_max_ind_lin << endl;
cout << "Lout first local_max =  " << local_max_ind_lout << endl;
int diff = local_max_ind_lout - local_max_ind_lin;

// subtract the first time from the second one 
for (int i = 0; i < diff; i++){
  Lin.insert(Lin.begin(), 0);
}

int newSize = Lout.size()-diff;
// only calculate after rin is stable/high
    //check all of rin mark the time where changes to up/down occur 
    //set that time as our start time 

// go through and check the conditions Lout x Lin > 0 and abs Lout > abs Lin 
for (int i = find_stability_point(rIn); i < newSize; i++){
  total_events++;
  if (satisfies_BE(Lout[i], Lin[i])){
    total_correct++;
  }
}


// in theory rerun this with different noise/temp parameters 
// delete/rewrite output file 

  cout << "Total events: " << total_events << endl;
  cout << "Total correct: " << total_correct << endl;
  csv_write(find_stability_point(rIn), time, rIn, Lin, Lout);
  return 0;
}