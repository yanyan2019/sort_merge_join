/* This is a skeleton code for Optimized Merge Sort, you can make modifications as long as you meet
   all question requirements*/

#include <bits/stdc++.h>
#include <algorithm>
#include "record_class.h"

using namespace std;

//defines how many blocks are available in the Main Memory
#define buffer_size 22

Records buffers[buffer_size]; //use this class object of size 22 as your main memory

/***You can change return type and arguments as you want.***/

// print buffers
void printBuffers(int start, int cur_size, char type){
    cout << "buffers: " << endl;
    for(int i = start; i < cur_size; i++){
        if(type == 'e'){
            cout << i << ": "<< buffers[i].emp_record.eid << " "<< buffers[i].emp_record.ename << endl;
        }else{
            cout << i << ": "<< buffers[i].dept_record.did << ","<< buffers[i].dept_record.dname << ","<< buffers[i].dept_record.budget <<  ","<< buffers[i].dept_record.managerid<< endl;
        }
    }
    cout << endl;
}

// comparator function for build-in sort() to compare eid
bool compareEid(Records r1, Records r2){
        return (r1.emp_record.eid < r2.emp_record.eid);
}

// comparator function  for build-in sort() to compare managerid
bool compareMid(Records r1, Records r2){
        return (r1.dept_record.managerid < r2.dept_record.managerid);
}

// output to file
void output_to_file(fstream& file, char type, int cur_size, int runs){
    // open temp file
    string file_name;
    if(type == 'e'){
        file_name = "emp_temp";
    }else{
        file_name = "dept_temp";
    }
    file.open(file_name + to_string(runs), ios::in | ios:: out |ios::app);

    // write record to file
    for(int i = 0 ;i < cur_size; i++){
        if(type == 'e'){
            file << buffers[i].emp_record.eid << "," <<  buffers[i].emp_record.ename << "," <<  buffers[i].emp_record.age << ","<<  buffers[i].emp_record.salary << endl;
        }else{
            file << buffers[i].dept_record.did << ","<< buffers[i].dept_record.dname << "," << buffers[i].dept_record.budget<< "," << buffers[i].dept_record.managerid << endl;
        }
    }
    file.close();
}


//Sorting the buffers in main_memory and storing the sorted records into a temporary file (runs)
void Sort_Buffer(fstream& file, char type, int& cur_size, int& runs){
    //Remember: You can use only [AT MOST] 22 blocks for sorting the records / tuples and create the runs
    Records record;
    fstream temp_file;

    //read file
    while(!(file.eof())){
        if(type == 'e'){
            record = Grab_Emp_Record(file);
        }else{
            record = Grab_Dept_Record(file);
        }
        if(record.no_values != -1){
            // check if buffer is full
            if(cur_size >= buffer_size){
                //printBuffers(cur_size, type);
                if(type == 'e'){
                     sort(buffers, buffers + buffer_size, compareEid);
                }else{
                     sort(buffers, buffers + buffer_size, compareMid);
                }
                //printBuffers(cur_size, type);
                output_to_file(temp_file, type, cur_size, runs);

                runs++;
                cur_size = 0;
            }
            // insert record to buffers
            buffers[cur_size] = record;
            cur_size++;
        }
    }

    // handle rest of records in buffers
    if(cur_size > 0){
        //printBuffers(cur_size,type);
        if(type == 'e'){
            sort(buffers, buffers + cur_size, compareEid);
        }else{
            sort(buffers, buffers + cur_size, compareMid);
        }
        //printBuffers(cur_size,type);
        output_to_file(temp_file, type, cur_size, runs);
        runs++;
        cur_size = 0;
    }
    return;
}

//Prints out the attributes from empRecord and deptRecord when a join condition is met
//and puts it in file Join.csv
void PrintJoin(fstream& joinout, int e_pos, int d_pos) {
    joinout << buffers[e_pos].emp_record.eid << ","<< buffers[e_pos].emp_record.ename << "," << buffers[e_pos].emp_record.age << "," << buffers[e_pos].emp_record.salary << " ";
    joinout << buffers[d_pos].dept_record.did << ","<< buffers[d_pos].dept_record.dname << "," << buffers[d_pos].dept_record.budget<< "," << buffers[d_pos].dept_record.managerid << endl;
    return;
}

//Use main memory to Merge and Join tuples
//which are already sorted in 'runs' of the relations Dept and Emp
void Merge_Join_Runs(int emp_runs, int dept_runs,fstream& joinout){
    fstream emp_temp, dept_temp;
    int e_count = 0, d_count = 0,count = 0, cur_size = 0;
    Records record, record2;
    // array for marking position in runs
    int pos_array[2*(emp_runs * dept_runs)];
    for(int i = 0; i < 2*(emp_runs * dept_runs) ;i++){
        pos_array[i] = 0;
        //cout << pos_array[i] << " " << endl;
    }


    cout << "emp runs: " << emp_runs << " dept_runs: " << dept_runs << endl;
    // fill up buffers with first tuple in each run
    for(int i = 0; i < (emp_runs + dept_runs); i++, count++){
        if(count == (emp_runs)){
            count = 0;
        }
        if(i < emp_runs){
            emp_temp.open("emp_temp" + to_string(count), ios::in);
            record = Grab_Emp_Record(emp_temp);
            emp_temp.close();
        }else{
            dept_temp.open("dept_temp" + to_string(count),  ios::in);
            record = Grab_Dept_Record(dept_temp);
            dept_temp.close();
        }
        buffers[i] = record;
        cur_size++;
    }
    printBuffers(0, emp_runs, 'e');
    printBuffers(emp_runs, emp_runs+ dept_runs, 'd');

    // loop until end of all temp files
    //while(e_count < emp_runs && d_count < emp_runs){
    while(e_count < (emp_runs + dept_runs)){
        // loop through buffers until all e's done matching with all d's
        for(int i = 0; i < emp_runs; i++){
            for(int j = 0; j < dept_runs; j++){
                // recover buffers where it was for different combinations
                dept_temp.open("dept_temp" + to_string(j), ios::in);
                emp_temp.open("emp_temp" + to_string(i), ios::in);
                dept_temp.seekg( pos_array[(i * emp_runs ) +1] );
                emp_temp.seekg( pos_array[(i * emp_runs )] );

                cout <<  pos_array[(i * emp_runs ) +1] << endl;
                cout <<  pos_array[(i * emp_runs )] << endl;
                //exit(0);

                buffers[i] = Grab_Emp_Record(emp_temp);

                buffers[emp_runs + j] = Grab_Dept_Record(dept_temp);

                //exit(1);
                if(buffers[i].no_values == -1 || buffers[emp_runs + j].no_values == -1){
                    continue;
                    //cout << "continue " << endl;
                }

                // if e > d
                if(buffers[i].emp_record.eid > buffers[emp_runs + j].dept_record.managerid){
                    // read next d to buffer
                   //dept_temp.open("dept_temp" + to_string(j), ios::in);
                   record = Grab_Dept_Record(dept_temp);

                   // mark next pos in temp file
                   pos_array[(i * emp_runs) +1] = dept_temp.tellg();
                   buffers[emp_runs + j] = record;
                }
                // else if e < d
                else if(buffers[i].emp_record.eid < buffers[emp_runs + j].dept_record.managerid){
                    // read next e to buffer
                    //emp_temp.open("emp_temp" + to_string(i), ios::in);
                    record = Grab_Emp_Record(emp_temp);

                    // mark next position
                    pos_array[(i * emp_runs )] = emp_temp.tellg();
                    buffers[i] = record;
                }
                else if(buffers[i].emp_record.eid == buffers[emp_runs + j].dept_record.managerid){
                    // else matches store the Joined new tuples using PrintJoin()
                    cout << "matches!" << endl;
                    PrintJoin(joinout, i, emp_runs + j);

                    // update next read position
                    record2 = Grab_Dept_Record(dept_temp);
                    pos_array[(i * emp_runs ) +1] = dept_temp.tellg();
                    buffers[emp_runs + i] = record2;

                    if(record2.no_values == -1){
                        e_count++;
                    }
                    record = Grab_Emp_Record(emp_temp);
                    pos_array[(i * emp_runs )] = emp_temp.tellg();
                    buffers[i] = record;
                }

                if(record.no_values == -1){
                    cout << e_count << endl;
                    e_count++;
                }
            }
        }
    }
    return;
}

int main() {

    //Open file streams to read and write
    //Opening out two relations Emp.csv and Dept.csv which we want to join
    fstream empin;
    fstream deptin;
    empin.open("Emp.csv", ios::in);
    deptin.open("Dept.csv", ios::in);

    //Creating the Join.csv file where we will store our joined results
    fstream joinout;
    joinout.open("Join.csv", ios::out | ios::app);

    //1. Create runs for Dept and Emp which are sorted using Sort_Buffer()
    // emp
    int cur_size = 0;
    int emp_runs = 0;
    Sort_Buffer(empin, 'e', cur_size, emp_runs);
    empin.close();

    // dept
    cur_size = 0;
    int dept_runs = 0;
    Sort_Buffer(deptin, 'd', cur_size, dept_runs);
    deptin.close();

    //2. Use Merge_Join_Runs() to Join the runs of Dept and Emp relations
    Merge_Join_Runs(emp_runs, dept_runs, joinout);

    //Please delete the temporary files (runs) after you've joined both Emp.csv and Dept.csv

    return 0;
}
