#include<iostream>
#include<algorithm>
#include<cmath>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include<map>
#include<time.h>
#include<random>
#include<unordered_map>
#include<set>
#include<chrono>

using namespace std;

const int input_id=3;//0:netflix, 1:amazon_M, 2:amazon_K, 3:MovieLens

struct Item{
    int id;
    vector<double> i2v_vec;
    vector<double> mf_vec;
    double i2v_norm =0;
    double mf_norm =0;
    double ip =0;
    int flag =0;
    double min_dist= 10000000;
    int check_ite = 0;
    int flag_ip =0;
    double key =0;
};

bool norm_comp(const Item& a, const Item& b){
    return a.mf_norm < b.mf_norm;
}

bool id_comp(const Item& a, const Item& b){
    return a.id < b.id;
}

bool ip_comp(const Item& a, const Item& b){
    return a.ip > b.ip;
}

bool norm_comp_dec(const Item& a, const Item& b){
    return a.mf_norm > b.mf_norm;
}

bool key_comp(const Item& a, const Item& b){
    return a.key > b.key;
}


struct User{
    vector<double> vec;
    double norm=0;
};

struct Result_data{
    vector<int> answer_id;
    double min_norm;
    double ip_sum=0;
    double dist_min = 10000000;
    double final_score=0;
    double msec1=0;//first iteration time
    double msec2=0;// other iterations time
    vector<double> time_vec;
};

struct Result_tmp{
    double score= -1000000;
    int id;
    double dist_min=10000000;
    double ip;
};



//input data
vector<string> split(string& input, char delimiter)
{
    istringstream stream(input);
    string field;
    vector<string> result;
    while (getline(stream, field, delimiter)) {
        result.push_back(field);
    }
    return result;
}

void input_MF(vector<User> &vec_users, vector<Item> &vec_items){
    string input_file;
    if(input_id ==0){
        input_file = "../dataset/dataset_MF200/netflix_mf-200.txt";
    }else if(input_id ==1){
        input_file = "../dataset/dataset_MF200/amazon_Movies_and_TV_mf-200.txt";
    }else if(input_id ==2){
        input_file = "../dataset/dataset_MF200/amazon_Kindle_Store_mf-200.txt";
    }else if(input_id == 3){
        input_file = "../dataset/dataset_MF200/MovieLens_mf-200.txt";
    }

    ifstream ifs(input_file);
    if(!ifs){
        cout<<"Error! File can not be opened"<<endl;
    }

    string line;
    int i=0;
    int j=0;
    while (getline(ifs, line)){
        vector<string> strvec = split(line,' ');
        if(strvec[1]=="F"){
            continue;
        }
        string tmp_str = strvec[0];
        if(tmp_str[0] == 'p'){//user
            strvec.erase(strvec.begin());//erase meta information
            strvec.erase(strvec.begin());

            vector<double> tmp;
            for(j=0;j<strvec.size();j++){
                double d= stod(strvec.at(j));
                tmp.push_back(d);
            }
            User user;
            user.vec = tmp;
            vec_users.push_back(user);
        }

        if(tmp_str[0] == 'q'){//item
            Item item;
            item.id = stoi(tmp_str.substr(1));

            strvec.erase(strvec.begin());//erase meta information
            strvec.erase(strvec.begin());
            vector<double> tmp;
            for(j=0;j<strvec.size();j++){
                double d= stod(strvec.at(j));
                tmp.push_back(d);
            }
            item.mf_vec = tmp;
            vec_items.push_back(item);
            
        }        
    }
}


void input_item2vec(vector<Item> &vec_items){
    string input_file;
    if(input_id ==0){
        input_file = "../dataset/dataset_item2vec/netflix_item2vec_d-200.txt";
    }else if(input_id ==1){
        input_file = "../dataset/dataset_item2vec/amazon_Movie_item2vec_d-200.txt";
    }else if(input_id ==2){
        input_file = "../dataset/dataset_item2vec/amazon_Kindle_item2vec_d-200.txt";
    }else if(input_id == 3){
        input_file = "../dataset/dataset_item2vec/MovieLens_item2vec_d-200.txt";
    }
    ifstream ifs(input_file);
    if(!ifs){
        cout<<"Error! File can not be opened"<<endl;
    }

    cout << "mf items " << vec_items.size() << endl;
    string line;
    int i=0;
    int j=0;
    int count=0;
    while (getline(ifs, line)) {
        vector<string> strvec = split(line,' ');
        strvec.erase(strvec.begin());//erase meta information

        vector<double> tmp;
        for(i=0; i<strvec.size(); i++){
            double d = stod(strvec.at(i));
            tmp.push_back(d);
        }
        vec_items[count].i2v_vec = tmp;
        count++;
    }
}

//input parameter
int input_k(){
    ifstream ifs("../parameter/k.txt");
    if(!ifs){
        cout<<"Error! File can not be opened"<<endl;
    }
    string line;
    int k;
    while (getline(ifs, line)) {
        vector<string> strvec = split(line,' ');
        k = stoi(strvec[0]);
    }
    return k;
}

double input_lamda(){
    ifstream ifs("../parameter/lamda.txt");
    if(!ifs){
        cout<<"Error! File can not be opened"<<endl;
    }
    string line;
    double lamda;
    while (getline(ifs, line)) {
        vector<string> strvec = split(line,' ');
        lamda = stod(strvec[0]);
    }
    return lamda;
}


