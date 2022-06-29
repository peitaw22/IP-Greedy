#include "greedy_functions.h"

mt19937 mt(1);

int main(){
    //parameter
    int k = input_k();
    double lamda = input_lamda();
    ///

    vector<Item> items;
    vector<User> users;

    input_MF(users, items);
    input_item2vec(items);

    compute_items_norm_mf(items);
    compute_items_norm_i2v(items);
    make_key(items);
    sort(items.begin(), items.end(), key_comp);
    double s = get_scale(items);//scale
    

    cout << "item " << items.size() << " user " << users.size() << endl;
    cout << "k " << k << " lamda " << lamda << endl;
    cout << "scale " << s <<endl;

    vector<vector<double> > record;
    vector<vector<double> > record_time; 
    std::uniform_int_distribution<> rand_query_data(0, users.size()-1); 

    for(int i=0; i< 100; i++){
        Init_items(items);
        sort(items.begin(), items.end(), key_comp);
        int user_id = rand_query_data(mt);
        
        Result_data result;
        
        chrono::system_clock::time_point  start, end;

        start = chrono::system_clock::now();
        greedy_search_rev(users[user_id].vec, k, lamda, (double)5/s,  items, result);
        end= chrono::system_clock::now();
        
        auto time = end - start;
        double msec = std::chrono::duration_cast<chrono::microseconds>(time).count();
        msec =(double)msec/1000;
        
        //record
        vector<double> record_tmp;
        record_tmp.push_back(user_id);
        record_tmp.push_back(result.final_score);
        record_tmp.push_back(result.dist_min);
        record_tmp.push_back(result.msec1);
        record_tmp.push_back(result.msec2);
        record_tmp.push_back(msec);
        for(int j=0; j < result.answer_id.size(); j++){
            record_tmp.push_back(result.answer_id[j]);
        }

        record.push_back(record_tmp);
        record_fun_double(user_id, record_time, result.time_vec);    
    }


    //Output
    string output_file_name;
    if(input_id ==0){
        output_file_name = "netflix200_k-" + to_string(k) + "_lam-" + to_string(lamda) + "_100.csv";
    }else if(input_id ==1){
        output_file_name = "amazon_M200_k-" + to_string(k) + "_lam-" + to_string(lamda) + "_100.csv";
    }else if(input_id ==2){
        output_file_name = "amazon_K200_k-" + to_string(k) + "_lam-" + to_string(lamda) + "_100.csv";
    }else if(input_id == 3){
        output_file_name = "MovieLens200_k-" + to_string(k) + "_lam-" + to_string(lamda) + "_100.csv";
    }

    ofstream file(output_file_name);
    for(int i=0; i< record.size();i++){
        for(int j=0; j<record[i].size()-1;j++){
            file<< record[i][j];
            file<<',';
        }
        file<< record[i][ record[i].size() -1];
        file <<'\n';
    }
    file.close();

    compute_file("time_" + output_file_name, record_time);
    cout << "finish" <<endl;
    return 0;
}
