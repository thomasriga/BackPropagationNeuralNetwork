﻿// Backpropagation, 25x25x8 units, binary sigmoid function network

/*
THR-BackPropagationNeuralNetwork Copyright (C) 2010 Thomas Riga

This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation; either version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

Contact the author at thomasriga@gmail.com (http://www.thomasriga.com)
*/


#include <iostream.h>
#include <fstream.h>
#include <conio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <stdio.h>
#include <float.h>
double **input,
        *hidden,
        **output,
        **target,
        *bias,
        **weight_i_h,
        **weight_h_o,
        *errorsignal_hidden,
        *errorsignal_output;
int input_array_size,
        hidden_array_size,
        output_array_size,
        max_patterns,
        bias_array_size,
        gaset = -2500,
        number_of_input_patterns,
        pattern,
        file_loaded = 0,
        ytemp = 0,
        ztemp = 0;
double learning_rate,
        max_error_tollerance = 0.1;
char filename[128];
#define IA   16807
#define IM   2147483647
#define AM   (1.0 / IM)
#define IQ   127773
#define IR   2836
#define NTAB 32
#define NDIV (1+(IM-1) / NTAB)
#define EPS  1.2e-7
#define RNMX (1.0 - EPS)
int compare_output_to_target();
void load_data(char *arg);
void save_data(char *argres);
void forward_pass(int pattern);
void backward_pass(int pattern);
void custom();
void compute_output_pattern();
void get_file_name();
float bedlam(long *idum);
void learn();
void make();
void test();
void print_data();
void print_data_to_screen();
void print_data_to_file();
void output_to_screen();
int getnumber();
void change_learning_rate();
void initialize_net();
void clear_memory();
main()
{
        for(;;) {
                char choice;
                cout << endl << "1. load data" << endl;
                cout << "2. learn from data" << endl;
                cout << "3. compute output pattern" << endl;
                cout << "4. make new data file" << endl;
                cout << "5. save data" << endl;
                cout << "6. print data" << endl;
                cout << "7. change learning rate" << endl;
                cout << "8. exit" << endl << endl;
                cout << "Enter your choice (1-8)";
                do { choice = getch(); } while (choice != '1' && 
                                                choice != '2' && 
                                                choice != '3' && 
                                                choice != '4' && 
                                                choice != '5' && 
                                                choice != '6' && 
                                                choice != '7' && 
                                                choice != '8');
                switch(choice) {
                        case '1':
                        {
                                if (file_loaded == 1) clear_memory();
                                get_file_name();
                                file_loaded = 1;
                                load_data(filename);
                        }
                        break;
                        case '2': learn();
                        break;
                        case '3': compute_output_pattern();
                        break;
                        case '4': make();
                        break;
                        case '5':
                        {
                                if (file_loaded == 0)
                                {
                                        cout << endl
                                             << "there is no data loaded into memory"
                                             << endl;
                                        break;
                                }
                                cout << endl << "enter a filename to save data to: ";
                                cin >> filename;
                                save_data(filename);
                        }
                        break;
                        case '6': print_data();
                        break;
                        case '7': change_learning_rate();
                        break;
                        case '8': return 0;
                };
        }
}
void initialize_net()
{
        int x;
        input = new double * [number_of_input_patterns];
        if(!input) { 
            cout << endl << "memory problem!"; exit(1);
        }
        for(x=0; x<number_of_input_patterns; x++)
        {
                input[x] = new double [input_array_size];
                if(!input[x]) { 
                    cout << endl << "memory problem!"; exit(1); 
                }
        }
        hidden = new double [hidden_array_size];
        if(!hidden) { 
            cout << endl << "memory problem!"; exit(1); 
        }
        output = new double * [number_of_input_patterns];
        if(!output) { cout << endl << "memory problem!"; exit(1); }
        for(x=0; x<number_of_input_patterns; x++)
        {
                output[x] = new double [output_array_size];
                if(!output[x]) { 
                    cout << endl << "memory problem!"; exit(1); 
                }
        }
        target = new double * [number_of_input_patterns];
        if(!target) { 
            cout << endl << "memory problem!"; exit(1); 
        }
        for(x=0; x<number_of_input_patterns; x++)
        {
                target[x] = new double [output_array_size];
                if(!target[x]) { 
                    cout << endl << "memory problem!"; exit(1); 
                }
        }
        bias = new double [bias_array_size];
        if(!bias) { 
            cout << endl << "memory problem!"; exit(1); 
        }
        weight_i_h = new double * [input_array_size];
        if(!weight_i_h) { 
            cout << endl << "memory problem!"; exit(1); 
        }
        for(x=0; x<input_array_size; x++)
        {
                weight_i_h[x] = new double [hidden_array_size];
                if(!weight_i_h[x]) { 
                    cout << endl << "memory problem!"; exit(1); 
                }
        }
        weight_h_o = new double * [hidden_array_size];
        if(!weight_h_o) {
            cout << endl << "memory problem!"; exit(1); 
        }
        for(x=0; x<hidden_array_size; x++)
        {
                weight_h_o[x] = new double [output_array_size];
                if(!weight_h_o[x]) { 
                    cout << endl << "memory problem!"; exit(1); 
                }
        }
        errorsignal_hidden = new double [hidden_array_size];
        if(!errorsignal_hidden) { 
            cout << endl << "memory problem!"; exit(1); 
        }
        errorsignal_output = new double [output_array_size];
        if(!errorsignal_output) { 
            cout << endl << "memory problem!"; exit(1);
        }
        return;
}
void learn()
{
        if (file_loaded == 0)
        {
                cout << endl
                         << "there is no data loaded into memory"
                         << endl;
                return;
        }
        cout << endl << "learning..." << endl 
             << "press a key to return to menu" << endl;
        register int y;
        while(!kbhit()) {
                for(y=0; y<number_of_input_patterns; y++) {
                        forward_pass(y);
                        backward_pass(y);
                }
                if(compare_output_to_target()) {
                        cout << endl 
                             << "learning successful" << endl;
                        return;
                }
        }
        cout << endl << "learning not successful yet" << endl;
        return;
}
void load_data(char *arg) {
        int x, y;
        ifstream in(arg);
        if(!in) { 
            cout << endl << "failed to load data file" << endl; 
            file_loaded = 0; 
            return; 
        }
        in >> input_array_size;
        in >> hidden_array_size;
        in >> output_array_size;
        in >> learning_rate;
        in >> number_of_input_patterns;
        bias_array_size = hidden_array_size + output_array_size;
        initialize_net();
        for(x=0; x<bias_array_size; x++) in >> bias[x];
        for(x=0; x<input_array_size; x++) { 
                for(y=0; y<hidden_array_size; y++) 
                    in >> weight_i_h[x][y];
        }
        for(x=0; x<hidden_array_size; x++) { 
                for(y=0; y<output_array_size; y++) 
                    in >> weight_h_o[x][y];
        }
        for(x=0; x<number_of_input_patterns; x++) {
                for(y=0; y<input_array_size; y++) 
                    in >> input[x][y];
        }
        for(x=0; x<number_of_input_patterns; x++) {
                for(y=0; y<output_array_size; y++) 
                    in >> target[x][y];
        }
        in.close();
        cout << endl << "data loaded" << endl;
        return;
}
void forward_pass(int pattern)
{
        _control87 (MCW_EM, MCW_EM);
        register double temp=0;
        register int x,y;
// INPUT -> HIDDEN
        for(y=0; y<hidden_array_size; y++) {
                for(x=0; x<input_array_size; x++) {
                        temp += 
                            (input[pattern][x] * weight_i_h[x][y]);
                }
                hidden[y] = 
                    (1.0 / (1.0 + exp(-1.0 * (temp + bias[y]))));
                temp = 0;
        }
// HIDDEN -> OUTPUT
        for(y=0; y<output_array_size; y++) {
                for(x=0; x<hidden_array_size; x++) {
                        temp += (hidden[x] * weight_h_o[x][y]);
                }
                output[pattern][y] = 
                    (1.0 / (1.0 + exp(-1.0 * 
                        (temp + bias[y + hidden_array_size])
                    )));
                temp = 0;
        }
        return;
}
void backward_pass(int pattern)
{
        register int x, y;
        register double temp = 0;
// COMPUTE ERRORSIGNAL FOR OUTPUT UNITS
        for(x=0; x<output_array_size; x++) {
                errorsignal_output[x] = 
                    (target[pattern][x] - output[pattern][x]);
        }
// COMPUTE ERRORSIGNAL FOR HIDDEN UNITS
        for(x=0; x<hidden_array_size; x++) {
                for(y=0; y<output_array_size; y++) { 
                        temp += 
                            (errorsignal_output[y] * weight_h_o[x][y]);
                }
        errorsignal_hidden[x] = hidden[x] * (1-hidden[x]) * temp;
        temp = 0.0;
        }
// ADJUST WEIGHTS OF CONNECTIONS FROM HIDDEN TO OUTPUT UNITS
        double length = 0.0;
        for (x=0; x<hidden_array_size; x++) {
                length += hidden[x]*hidden[x];
        }
        if (length<=0.1) length = 0.1;
        for(x=0; x<hidden_array_size; x++) {
                for(y=0; y<output_array_size; y++) {
                        weight_h_o[x][y] += 
                                (learning_rate * 
                                errorsignal_output[y] * 
                                hidden[x]/length);
                }
        }
// ADJUST BIASES OF HIDDEN UNITS
        for(x=hidden_array_size; x<bias_array_size; x++) {
                bias[x] += 
                    (learning_rate * errorsignal_output[x] / length);
        }
// ADJUST WEIGHTS OF CONNECTIONS FROM INPUT TO HIDDEN UNITS
        length = 0.0;
        for (x=0; x<input_array_size; x++) {
                length += input[pattern][x]*input[pattern][x];
        }
        if (length<=0.1) length = 0.1;
        for(x=0; x<input_array_size; x++) {
                for(y=0; y<hidden_array_size; y++) {
                        weight_i_h[x][y] += 
                            (learning_rate * errorsignal_hidden[y] * 
                        input[pattern][x]/length);
                }
        }
// ADJUST BIASES FOR OUTPUT UNITS
        for(x=0; x<hidden_array_size; x++) {
                bias[x] += 
                    (learning_rate * errorsignal_hidden[x] / length);
        }
        return;
}
int compare_output_to_target()
{
        register int y,z;
        register double temp, error = 0.0;
        temp = target[ytemp][ztemp] - output[ytemp][ztemp];
        if (temp < 0) error -= temp;
        else error += temp;
        if(error > max_error_tollerance) return 0;
        error = 0.0;
        for(y=0; y < number_of_input_patterns; y++) {
                for(z=0; z < output_array_size; z++) {
                        temp = target[y][z] - output[y][z];
                        if (temp < 0) error -= temp;
                        else error += temp;
                        if(error > max_error_tollerance) {
                                ytemp = y;
                                ztemp = z;
                                return 0;
                        }
                        error = 0.0;
                }
        }
        return 1;
}
void save_data(char *argres) {
        int x, y;
        ofstream out;    
        out.open(argres);
        if(!out) { 
            cout << endl << 
                "failed to save file" << endl; 
            return; 
        }
        out << input_array_size << endl;
        out << hidden_array_size << endl;
        out << output_array_size << endl;
        out << learning_rate << endl;
        out << number_of_input_patterns << endl << endl;
        for(x=0; x<bias_array_size; x++) out << bias[x] << ' ';
        out << endl << endl;
        for(x=0; x<input_array_size; x++) {
                for(y=0; y<hidden_array_size; y++) 
                    out << weight_i_h[x][y] << ' ';
        }
        out << endl << endl;
        for(x=0; x<hidden_array_size; x++) {
                for(y=0; y<output_array_size; y++) 
                    out << weight_h_o[x][y] << ' ';
        }
        out << endl << endl;
        for(x=0; x<number_of_input_patterns; x++) {
                for(y=0; y<input_array_size; y++) 
                    out << input[x][y] << ' ';
                out << endl;
        }
        out << endl;
        for(x=0; x<number_of_input_patterns; x++) {
                for(y=0; y<output_array_size; y++) 
                    out << target[x][y] << ' ';
                out << endl;
        }
        out.close();
        cout << endl << "data saved" << endl;
        return;
}    
void make()
{
        int x, y, z;
        double  inpx, 
                bias_array_size, 
                input_array_size, 
                hidden_array_size, 
                output_array_size;
        char makefilename[128];
        cout << endl << "enter name of new data file: ";
        cin >> makefilename;
        ofstream out;
        out.open(makefilename);
        if(!out) { 
            cout << endl << "failed to open file" << endl; 
            return;
        }
        cout << "how many input units? ";
        cin >> input_array_size;
        out << input_array_size << endl;
        cout << "how many hidden units? ";
        cin >> hidden_array_size;
        out << hidden_array_size << endl;
        cout << "how many output units? ";
        cin >> output_array_size;
        out << output_array_size << endl;
        bias_array_size = hidden_array_size + output_array_size;
        cout << endl << "Learning rate: ";
        cin >> inpx;
        out << inpx << endl;
        cout << endl << "Number of input patterns: ";
        cin >> z;
        out << z << endl << endl;
        for(x=0; x<bias_array_size; x++) 
            out << (1.0 - (2.0 * bedlam((long*)(gaset)))) << ' ';
        out << endl << endl;
        for(x=0; x<input_array_size; x++) {
                for(y=0; y<hidden_array_size; y++) 
                    out << (1.0 - (2.0 * bedlam((long*)(gaset)))) << ' ';
        }
        out << endl << endl;
        for(x=0; x<hidden_array_size; x++) {
                for(y=0; y<output_array_size; y++) 
                    out << (1.0 - (2.0 * bedlam((long*)(gaset)))) << ' ';
        }
        out << endl << endl;
        for(x=0; x < z; x++) {
                cout << endl << "input pattern " << (x + 1) << endl;
                for(y=0; y<input_array_size; y++) {
                        cout << (y+1) << ": ";
                        cin >> inpx;
                        out << inpx << ' ';
                }
                out << endl;
        }
        out << endl;
        for(x=0; x < z; x++) {
                cout << endl << "target output pattern " << (x+1) << endl;
                for(y=0; y<output_array_size; y++) {
                        cout << (y+1) << ": ";
                        cin >> inpx;
                        out << inpx << ' ';
                }
                out << endl;
        }
        out.close();
        cout << endl << 
            "data saved, to work with this new data file you first have to load it" 
            << endl;
        return;
}
float bedlam(long *idum)
{
  int xj;
  long xk;
  static long iy=0;
  static long iv[NTAB];
  float temp;
  if(*idum <= 0 || !iy)
  {
        if(-(*idum) < 1)
        {
          *idum = 1 + *idum;
        }
        else
        {
          *idum = -(*idum);
        }
        for(xj = NTAB+7; xj >= 0; xj--)
        {
          xk = (*idum) / IQ;
          *idum = IA * (*idum - xk * IQ) - IR * xk;
          if(*idum < 0)
          {
        *idum += IM;
          }
          if(xj < NTAB)
          {
        iv[xj] = *idum;
          }
        }
          iy = iv[0];
  }
  xk = (*idum) / IQ;
  *idum = IA * (*idum - xk * IQ) - IR * xk;
  if(*idum < 0)
  {
        *idum += IM;
  }
  xj = iy / NDIV;
  iy = iv[xj];
  iv[xj] = *idum;
  if((temp=AM*iy) > RNMX)
  {
        return(RNMX);
  }
  else
  {
        return(temp);
  }
}
void test()
{
        pattern = 0;
        while(pattern == 0) {
                cout << endl << endl << 
                    "There are " << number_of_input_patterns 
                    << " input patterns in the file," << endl 
                    << "enter a number within this range: ";
                pattern = getnumber();
        }
        pattern--;
        forward_pass(pattern);
        output_to_screen();
        return;
}
void output_to_screen()
{
        int x;
        cout << endl << "Output pattern:" << endl;
        for(x=0; x<output_array_size; x++) {
                cout << endl << (x+1) 
                     << ": " << output[pattern][x] << "    binary: ";
                if(output[pattern][x] >= 0.9) cout << "1";
                else if(output[pattern][x]<=0.1) cout << "0";
                else cout << "intermediate value";
        }
        cout << endl;
        return;
}
int getnumber()
{
        int a, b = 0;
        char c, d[5];
        while(b<4) {
        do { c = getch(); } while  (c != '1' && 
                                    c != '2' && 
                                    c != '3' && 
                                    c != '4' && 
                                    c != '5' && 
                                    c != '6' && 
                                    c != '7' && 
                                    c != '8' && 
                                    c != '9' && 
                                    c != '0' && 
                                    toascii(c) != 13);
                if(toascii(c)==13) break;
                if(toascii(c)==27) return 0;
                d[b] = c;
                cout << c;
                b++;
        }
        d[b] = '\0';
        a = atoi(d);
        if(a < 0 || a > number_of_input_patterns) a = 0;
        return a;
}
void get_file_name()
{
        cout << endl << "enter name of file to load: ";
        cin >> filename;
        return;
}
void print_data()
{
        char choice;
        if (file_loaded == 0)
        {
                cout << endl
                         << "there is no data loaded into memory"
                         << endl;
                return;
        }
        cout << endl << "1. print data to screen" << endl;
        cout << "2. print data to file" << endl;
        cout << "3. return to main menu" << endl << endl;
        cout << "Enter your choice (1-3)" << endl;
        do { choice = getch(); } 
            while (choice != '1' && choice != '2' && choice != '3');
        switch(choice) {
                case '1': print_data_to_screen();
                break;
                case '2': print_data_to_file();
                break;
                case '3': return;
        };
        return;
}
void print_data_to_screen() {
        register int x, y;
        cout << endl << endl << "DATA FILE: " << filename << endl;
        cout << "learning rate: " << learning_rate << endl;
        cout << "input units: " << input_array_size << endl;
        cout << "hidden units: " << hidden_array_size << endl;
        cout << "output units: " << output_array_size << endl;
        cout << "number of input and target output patterns: " 
             << number_of_input_patterns  << endl << endl;
        cout << "INPUT AND TARGET OUTPUT PATTERNS:";
        for(x=0; x<number_of_input_patterns; x++) {
                cout << endl << "input pattern: " << (x+1) << endl;
                for(y=0; y<input_array_size; y++) cout << input[x][y] << "  ";
                cout << endl << "target output pattern: " << (x+1) << endl;
                for(y=0; y<output_array_size; y++) cout << target[x][y] << "  ";
        }
        cout << endl << endl << "BIASES:" << endl;
        for(x=0; x<hidden_array_size; x++) {
                cout << "bias of hidden unit " << (x+1) << ": " << bias[x];
                if(x<output_array_size) 
                    cout << "      bias of output unit " 
                         << (x+1) << ": " << bias[x+hidden_array_size];
                cout << endl;
        }
        cout << endl << "WEIGHTS:" << endl;
        for(x=0; x<input_array_size; x++) {
                for(y=0; y<hidden_array_size; y++) 
                    cout << "i_h[" << x << "][" << y << "]: " 
                         << weight_i_h[x][y] << endl;
        }
        for(x=0; x<hidden_array_size; x++) {
                for(y=0; y<output_array_size; y++) 
                    cout << "h_o[" << x << "][" << y << "]: " 
                         << weight_h_o[x][y] << endl;
        }
        return;
}
void print_data_to_file()
{
        char printfile[128];
        cout << endl << "enter name of file to print data to: ";
        cin >> printfile;
        ofstream out;
        out.open(printfile);
        if(!out) { cout << endl << "failed to open file"; return; }
        register int x, y;
        out << endl << endl << "DATA FILE: " << filename << endl;
        out << "input units: " << input_array_size << endl;
        out << "hidden units: " << hidden_array_size << endl;
        out << "output units: " << output_array_size << endl;
        out << "learning rate: " << learning_rate << endl;
        out << "number of input and target output patterns: " 
            << number_of_input_patterns << endl << endl;
        out << "INPUT AND TARGET OUTPUT PATTERNS:";
        for(x=0; x<number_of_input_patterns; x++) {
                out << endl << "input pattern: " << (x+1) << endl;
                for(y=0; y<input_array_size; y++) out << input[x][y] << "  ";
                out << endl << "target output pattern: " << (x+1) << endl;
                for(y=0; y<output_array_size; y++) out << target[x][y] << "  ";
        }
        out << endl << endl << "BIASES:" << endl;
        for(x=0; x<hidden_array_size; x++) {
                out << "bias of hidden unit " << (x+1) << ": " << bias[x];
                if(x<output_array_size) 
                    out << "      bias of output unit " << (x+1) 
                        << ": " << bias[x+hidden_array_size];
                out << endl;
        }
        out << endl << "WEIGHTS:" << endl;
        for(x=0; x<input_array_size; x++) {
                for(y=0; y<hidden_array_size; y++) 
                    out << "i_h[" << x << "][" << y << "]: " 
                        << weight_i_h[x][y] << endl;
        }
        for(x=0; x<hidden_array_size; x++) {
                for(y=0; y<output_array_size; y++) 
                    out << "h_o[" << x << "][" << y << "]: " 
                        << weight_h_o[x][y] << endl;
        }
        out.close();
        cout << endl << "data has been printed to " << printfile << endl;
        return;
}
void change_learning_rate()
{
        if (file_loaded == 0)
        {
                cout << endl
                         << "there is no data loaded into memory"
                         << endl;
                return;
        }
        cout << endl << "actual learning rate: " 
             << learning_rate << " new value: ";
        cin >> learning_rate;
        return;
}
void compute_output_pattern()
{
        if (file_loaded == 0)
        {
                cout << endl
                         << "there is no data loaded into memory"
                         << endl;
                return;
        }
        char choice;
        cout << endl << endl << "1. load trained input pattern into network" << endl;
        cout << "2. load custom input pattern into network" << endl;
        cout << "3. go back to main menu" << endl << endl;
        cout << "Enter your choice (1-3)" << endl;
        do { choice = getch(); } while (choice != '1' && choice != '2' && choice != '3');
        switch(choice) {
                case '1': test();
                break;
                case '2': custom();
                break;
                case '3': return;
        };
}
void custom()
{
        _control87 (MCW_EM, MCW_EM);
        char filename[128];
        register double temp=0;
        register int x,y;
        double *custom_input = new double [input_array_size];
        if(!custom_input)
        {
                cout << endl << "memory problem!";
                return;
        }
        double *custom_output = new double [output_array_size];
        if(!custom_output)
        {
                delete [] custom_input;
                cout << endl << "memory problem!";
                return;
        }
        cout << endl << endl << "enter file that contains test input pattern: ";
        cin >> filename;
        ifstream in(filename);
        if(!in) { cout << endl << "failed to load data file" << endl; return; }
        for(x = 0; x < input_array_size; x++) {
                in >> custom_input[x];
        }
        for(y=0; y<hidden_array_size; y++) {
                for(x=0; x<input_array_size; x++) {
                        temp += (custom_input[x] * weight_i_h[x][y]);
                }
                hidden[y] = (1.0 / (1.0 + exp(-1.0 * (temp + bias[y]))));
                temp = 0;
        }
        for(y=0; y<output_array_size; y++) {
                for(x=0; x<hidden_array_size; x++) {
                        temp += (hidden[x] * weight_h_o[x][y]);
                }
                custom_output[y] = 
                    (1.0 / (1.0 + exp(-1.0 * (temp + bias[y + hidden_array_size]))));
                temp = 0;
        }
        cout << endl << "Input pattern:" << endl;
        for(x = 0; x < input_array_size; x++) {
                cout << "[" << (x + 1) << ": " << custom_input[x] << "]  ";
        }
        cout << endl << endl << "Output pattern:";
        for(x=0; x<output_array_size; x++) {
                cout << endl << (x+1) << ": " << custom_output[x] << "    binary: ";
                if(custom_output[x] >= 0.9) cout << "1";
                else if(custom_output[x]<=0.1) cout << "0";
                else cout << "intermediate value";
        }
        cout << endl;
        delete [] custom_input;
        delete [] custom_output;
        return;
}
void clear_memory()
{
        int x;
        for(x=0; x<number_of_input_patterns; x++)
        {
                delete [] input[x];
        }
        delete [] input;
        delete [] hidden;
        for(x=0; x<number_of_input_patterns; x++)
        {
                delete [] output[x];
        }
        delete [] output;
        for(x=0; x<number_of_input_patterns; x++)
        {
                delete [] target[x];
        }
        delete [] target;
        delete [] bias;
        for(x=0; x<input_array_size; x++)
        {
                delete [] weight_i_h[x];
        }
        delete [] weight_i_h;
        for(x=0; x<hidden_array_size; x++)
        {
                delete [] weight_h_o[x];
        }
        delete [] weight_h_o;
        delete [] errorsignal_hidden;
        delete [] errorsignal_output;
        file_loaded = 0;
        return;
}


