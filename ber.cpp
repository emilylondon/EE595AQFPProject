#include <iostream>
#include <random>
#include <fstream>
#include <vector>
#include "rapidcsv.h"
#include "ber.h"

#define HIGH 0.000005
#define LOW -0.000005
#define ERROR(a, b) ((a-b)/b)
using namespace std;

/**
 * Constructor for the Bit Error Rate (BER) class. 
 * @param fname Filename and path to input csv file 
**/
BER::BER(string fname){
    rapidcsv::Document josimOut(fname);
    time = josimOut.GetColumn<float>("time");
    rIn = josimOut.GetColumn<float>("I(RIN)");
    Lin = josimOut.GetColumn<float>("I(LIN|XI4)");
    Lout = josimOut.GetColumn<float>("I(LOUT|XI4)");
    total_events = 0;
    total_correct = 0;
}

/**
 * Output the normalized Iout and Iin to compare the outputs and ensure the signal is aligned properly. 
 * @param start_ind The starting point for signal measurement after Rin has settled 
**/
void BER::csv_write(){
   fstream fout;
   system("rm processed_buffer.csv");
   fout.open("processed_buffer.csv", ios::out | ios::app);
   fout << "time,\"I(LIN|XI4)\",\"I(LOUT|XI4)\"" << "\n";
   for (int i = 0; i < timeD.size(); i++){
      fout << timeD[i] << "," << LinD[i] << "," << LoutD[i] << "\n";
   }
}

/**
 * Finds the local maximum of the signal given within a specified region of the signal. 
 * @param signal Signal vector of floats 
**/ 
int BER::local_max(vector<float> signal){
  int n = signal.size()/6;
  float thresh = .00001; 
  float max = -100;
  int maxind = -1;
  float thresh_reached = false;
  for (int i = 0; i < n; i++){
    if (signal[i] > thresh && !thresh_reached){
      thresh_reached = true;
    }
    else if (signal[i] < thresh && thresh_reached){
      return maxind;
    }

    if (signal[i]>max){
      max = signal[i];
      maxind = i;
    }
  }
}

/**
 * Finds the point where the Rin vector stablizes and stops switching
 * @param sig_in Input RIN signal
 **/
int BER::find_stability_point(vector<float> sig_in){
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

/**
 * Checks if Lin signal and Lout meet the bit error condition or do not 
 * @param Iout Output signal value 
 * @param Iin Input signal value
**/
bool BER::satisfies_BE(float Iout, float Iin){
  return abs(Iout- Iin)<.00005 ? true: false;  //condition for no bit error
}

float BER::find_ber(){
    // find local max ind running time of lin:x4 after the 1.8xe-10 mark so around the 900th instance 
    int local_max_ind_lin = local_max(Lin), local_max_ind_lout = local_max(Lout);
    // subtract the first time from the second one 
    int diff = local_max_ind_lout - (local_max_ind_lin+1);
    //shift the Lout signal to the left 
    Lout.erase(Lout.begin(), Lout.begin() + diff);
    for (int i = 0; i < diff; i++){
    Lout.push_back(0);
    }
    newSize = Lout.size()-diff;

    // only calculate after rin is stable/high
    //check all of rin mark the time where changes to up/down occur 
    //set that time as our start time 

    // go through and check the conditions Lout x Lin > 0 and abs Lout > abs Lin 
    start_ind = find_stability_point(rIn);
    adc();
    for (int i = 0; i < timeD.size(); i++){
        total_events++;
        if (satisfies_BE(LoutD[i], LinD[i])){
            total_correct++;
        }
    }
    csv_write();
    return 1.0-(float)total_correct/(float)total_events;
}

void BER::adc(){
  //sampling t interval = 0.1 nanosecond 
  float t_samp = 0.1e-9;
  float delta = 0;
  float t_prev = time[start_ind];
  vector<float> quant_levels = {-20e-6, -12e-6, -4e-6, 4e-6, 12e-6};
  timeD.push_back(time[start_ind]);
  bool in_found = false, out_found = false; 
  for (int j = quant_levels.size(); j >= 0; j--){
    if (Lin[start_ind] >= quant_levels[j] && !in_found){
      in_found = true; 
      LinD.push_back(quant_levels[j]);
    }
    if (Lout[start_ind] >= quant_levels[j] && !out_found){
      out_found = true; 
      LoutD.push_back(quant_levels[j]);
    }
  }

  for (int i = start_ind; i < newSize; i++){
    if (delta >= t_samp){
      delta = 0;
      bool in_found = false, out_found = false; 
      for (int j = quant_levels.size(); j >= 0; j--){
        if (Lin[i] >= quant_levels[j] && !in_found){
          in_found = true; 
          LinD.push_back(quant_levels[j]);
        }
        if (Lout[i] >= quant_levels[j] && !out_found){
          out_found = true; 
          LoutD.push_back(quant_levels[j]);
        }
      }
      timeD.push_back(timeD[timeD.size()-1]+t_samp);
    }

    else {
      delta += time[i]-t_prev;
      t_prev = time[i];
    }
  }
}
/*
int main() {
    BER b("ex_pi_DQFP_buffer_chan_410.csv");
    float ber = b.find_ber();
    cout << "Bit error rate is " << ber << endl;
}
 */