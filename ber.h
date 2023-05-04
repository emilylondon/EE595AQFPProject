#include <iostream>

using namespace std;
class BER{
    public:
        BER(string fname);
        void csv_write();
        int local_max(vector<float> signal);
        int find_stability_point(vector<float> sig_in);
        bool satisfies_BE(float Iout, float Iin);
        void adc();
        float find_ber();
    private:
        long total_events;
        long total_correct;
        vector<float> time;
        vector<float> rIn;
        vector<float> Lin;
        vector<float> Lout;
        vector<float> LinD;
        vector<float> LoutD;
        vector<float> timeD;
        int start_ind;
        int newSize;
        string in_name;
};