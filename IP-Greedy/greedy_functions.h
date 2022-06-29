#include "Datainput.h"
struct Sign_data{//data for sort
    double min_key=10000000;
    int checked_id;
};

double normfun(vector<double> &vec){
    double norm_tmp=0;
    for(int i=0;i<vec.size();i++){
        norm_tmp=norm_tmp + vec[i]*vec[i];
    }
    double norm =sqrt(norm_tmp);
    return norm;
}

double ip_fun(vector<double> &a, vector<double> &b){
    double ip =0;
    for(int i=0; i< a.size(); i++){
        ip = ip + a[i]*b[i];
    }
    return ip;
}

double dist_fun(vector<double> &a, vector<double> &b){
    double tmp=0;
    for(int i=0; i< a.size(); i++){
        tmp += pow((a[i]-b[i]) ,2);
    }
    return sqrt(tmp);
}


double get_scale(vector<Item> &items){
    vector<double> max_scalar_vec = items[0].i2v_vec;
    vector<double> min_scalar_vec = items[0].i2v_vec;

    for(int i=1; i< items.size(); i++){
        for(int j=0; j< items[i].i2v_vec.size(); j++){
            if(max_scalar_vec[j] < items[i].i2v_vec[j]){
                max_scalar_vec[j] = items[i].i2v_vec[j];
            }

            if(min_scalar_vec[j] > items[i].i2v_vec[j]){
                min_scalar_vec[j] = items[i].i2v_vec[j];
            }
        }
    }
    vector<double> tmp;
    for(int i=0; i < max_scalar_vec.size(); i++){
        tmp.push_back( max_scalar_vec[i] - min_scalar_vec[i]);
    }
    double s = normfun(tmp);
    return s;
}

void compute_items_norm_mf(vector<Item> &items){
    for(int i=0; i< items.size(); i++){
        double norm = normfun(items[i].mf_vec);
        items[i].mf_norm = norm;
    }
}


void compute_items_norm_i2v(vector<Item> &items){
    for(int i=0; i< items.size(); i++){
        double norm = normfun(items[i].i2v_vec);
        items[i].i2v_norm = norm;
    }
}

void make_key(vector<Item> &items){
    for(int i=0; i< items.size(); i++){
        double tmp = items[i].mf_norm;
        items[i].key = tmp;
    }
}


void Init_items(vector<Item> &items){
    for(int i=0; i < items.size(); i++){
        items[i].ip = 0;
        items[i].flag = 0;
        items[i].min_dist= 10000000;
        items[i].check_ite =0;
        items[i].flag_ip=0;
        double tmp = items[i].mf_norm;
        items[i].key = tmp;
    }
}

int cauchy(vector<double> &query, vector<Item> &items, double query_norm, Sign_data &sign_data, int &ip_count){
    double threshold = -1000000;
    int id;
    double min_tmp=10000000;//minimum key in checked items
    int checked_id;

    for(int i=0; i < items.size(); i++){
        if(query_norm * items[i].mf_norm < threshold && i > items.size() *0.3){
            checked_id = i-1;
            break;
        }
        double ip = ip_fun(query, items[i].mf_vec);
        items[i].ip = ip;
        items[i].flag_ip = 1;//computed ip
        double tmp = (double)ip/query_norm;
        items[i].key = tmp;//update key

        ip_count++;

        if(tmp < min_tmp){//update minimum key
            min_tmp = tmp;
        }

        if(threshold < ip){
            threshold = ip;
            id = i;
        }
    }

    sign_data.min_key = min_tmp;//minimum key in checked items
    sign_data.checked_id = checked_id;
    return id;
}


void partly_sort_key(vector<Item> &items, Sign_data &sign_data){
    Item tmp;
    tmp.key = sign_data.min_key;
    int checked_id = sign_data.checked_id;
    if(items.size() == checked_id +1){//sort all data
        sort(items.begin(), items.end(), key_comp);

    }else{//partly sort
        auto itr_l =lower_bound(items.begin() + checked_id + 1, items.end(), tmp ,key_comp);
        if(itr_l == items.begin()){
            sort(items.begin(), itr_l+1, key_comp);

        }else{
            sort(items.begin(), itr_l, key_comp);
        }
    }

}

double compute_time(chrono::system_clock::time_point start, chrono::system_clock::time_point end){
    auto time = end - start;
    double msec = std::chrono::duration_cast<chrono::microseconds>(time).count();
    return (double)msec/1000;
}


void greedy_search_rev(vector<double> &query, int k, double lamda, double s, vector<Item> &items, Result_data &result){
    //first iteration
    vector<vector<double> > result_i2v_vec;//answer i2v vectors
    chrono::system_clock::time_point  start1, end1, start2, end2, start3, end3, start, end;

    start1 = chrono::system_clock::now();
    double query_norm = normfun(query);
    Sign_data sign_data;
    int ip_count =0;

    int start_id = cauchy(query, items, query_norm, sign_data, ip_count);//MIPS
    result.answer_id.push_back(items[start_id].id);
    items[start_id].flag=1;
    result.ip_sum += items[start_id].ip;
    result.min_norm = items[start_id].i2v_norm;
    result_i2v_vec.push_back(items[start_id].i2v_vec);

    end1 = chrono::system_clock::now();
    result.msec1 = compute_time(start1, end1);
    result.time_vec.push_back(compute_time(start1, end1));
    
    start = chrono::system_clock::now();
    int sort_flag =1;//１:sort

    for(int i=1; i<k; i++){//iterations
        start1 = chrono::system_clock::now();
        Result_tmp result_tmp;
        ip_count = 0;
        int tmp_dist_count = 0;
    
        if(sort_flag ==1){//sort
            partly_sort_key(items, sign_data);
            sort_flag =0;
        }

        sign_data.min_key =10000000;
        for(int j=0; j<items.size(); j++){
            if(items[j].flag ==1){//already solved
                if(items[j].key < sign_data.min_key){
                    sign_data.min_key = items[j].key;
                }
                sign_data.checked_id = j;
                continue;
            }

            double  bound_r = (double)(result_tmp.score - lamda * items[j].key * query_norm)/(s*(1-lamda));
            if(result.dist_min < bound_r){//termination
                sign_data.checked_id = j-1;
                break;
            }

            double threshold = bound_r;
            double ip;           
            
            if(items[j].i2v_norm + result.min_norm < threshold){//termination
                if(items[j].key < sign_data.min_key){
                    sign_data.min_key = items[j].key;
                }
                sign_data.checked_id =j;
                continue;
            }

            if(items[j].flag_ip ==0){//compute ip
                ip = ip_fun(query, items[j].mf_vec);
                items[j].ip = ip;
                items[j].flag_ip = 1;
                items[j].key = (double)ip/query_norm;//update key
                sort_flag=1;
                threshold = (double)(result_tmp.score - lamda * items[j].ip)/(s*(1-lamda));

                if(result.dist_min < threshold){//termination
                    if(items[j].key < sign_data.min_key){
                        sign_data.min_key = items[j].key;
                    }
                    sign_data.checked_id =j;
                    continue;
                }

                if(items[j].i2v_norm + result.min_norm < threshold){//termination
                    if(items[j].key < sign_data.min_key){
                        sign_data.min_key = items[j].key;
                    }
                    sign_data.checked_id =j;
                    continue;
                }
            }else{
                ip = items[j].ip;
            }

            if(items[j].min_dist < threshold){//compare already computed pair dist
                if(items[j].key < sign_data.min_key){
                    sign_data.min_key = items[j].key;
                }
                sign_data.checked_id =j;
                continue;
            }

            //dist information
            double min_dist = min(result.dist_min, items[j].min_dist);
            int start_index = items[j].check_ite;//index where item starts compute dist
            int flag =0;
            for(int l= start_index; l< result.answer_id.size(); l++){
                double tmp = dist_fun(items[j].i2v_vec, result_i2v_vec[l]);
                items[j].check_ite++;

                if(tmp < items[j].min_dist){
                    items[j].min_dist = tmp;
                }

                if(tmp < min_dist){//update min dist in the set when added
                    min_dist = tmp;
                    if(min_dist < threshold){
                        flag =1;
                        continue;
                    }
                }
            }

            if(items[j].key < sign_data.min_key){
                sign_data.min_key = items[j].key;
            }
            sign_data.checked_id =j;            

            if(flag ==1){
                continue;
            }
        
            //score in the set when added
            double score_tmp = lamda * ip + s*(1-lamda) * min_dist;
            if(result_tmp.score < score_tmp){
                result_tmp.score = score_tmp;
                result_tmp.id = j;
                result_tmp.dist_min = min_dist;
                result_tmp.ip = ip;
            }  
        }

        //update result list
        int answer_id_true = items[result_tmp.id].id;
        result.answer_id.push_back(answer_id_true);
        result_i2v_vec.push_back(items[result_tmp.id].i2v_vec);
        items[result_tmp.id].flag = 1;
        result.ip_sum += result_tmp.ip;
        result.dist_min = result_tmp.dist_min;
        double answer_norm = items[ result_tmp.id ].i2v_norm;
        if( answer_norm < result.min_norm){
            result.min_norm = answer_norm;
        }

        end1 = chrono::system_clock::now();
        result.time_vec.push_back(compute_time(start1, end1));
    }
    end = chrono::system_clock::now();
    result.msec2 = compute_time(start, end);

    result.final_score = (double)(lamda * result.ip_sum )/k  + s * (1-lamda) * result.dist_min;
}


void record_fun_double(int user_id, vector<vector<double> > &record, vector<double> &record_tmp){
    vector<double> tmp;
    tmp.push_back(user_id);
    for(int i=0; i < record_tmp.size(); i++){
        tmp.push_back(record_tmp[i]);
    }
    record.push_back(tmp);
}
void record_fun_int(int user_id, vector<vector<double> > &record, vector<int> &record_tmp){
    vector<double> tmp;
    tmp.push_back(user_id);
    for(int i=0; i < record_tmp.size(); i++){
        tmp.push_back(record_tmp[i]);
    }
    record.push_back(tmp);
}

void compute_file(string output_file_name, vector<vector<double> > &record){
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
}
