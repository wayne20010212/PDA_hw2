#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <set>
#include <utility>
#include <string>
#include <string.h>
#include <math.h>

using namespace std;


class FM{
private:
public:
    int max_area;
    int cell_num;
    int net_num;
    int left_size = 0, right_size = 0;
    int temp_left_size = 0, temp_right_size = 0;
    int total_area = 0;
    // vector<int> cell_area;
    vector<int> cell_area_a, cell_area_b;
    vector<int> net_cell_num;
    vector<string> cell_name;
    unordered_map<int,unordered_set<int>> net_have_cell;
    unordered_map<int,unordered_set<int>> cell_have_net;

    unordered_map<int,unordered_set<int>> net_left_cell, net_right_cell;
    unordered_map<int,unordered_set<int>> temp_net_left_cell, temp_net_right_cell;

    unordered_map<int,int> left_cell_num, right_cell_num; // net conatain cell num
    unordered_map<int,int> temp_left_cell_num, temp_right_cell_num; // net conatain cell num

    unordered_map<int,bool> left_cell, right_cell; // lift side right side
    unordered_map<int,bool> temp_left_cell, temp_right_cell; // lift side right side

    unordered_map<int,unordered_set<int>> order_gain;
    unordered_map<string,int> name_to_order;

    int min_con_cell_net = 0;
    unordered_map<int,int> gain;
    unordered_set<int> not_lock;
    int it = 0;
    int max_gain_cell;
    int max_gain = 0;
    int p_max = 0;
    int sw = 0;

    void init(ifstream &cells, ifstream &nets){
        string in;
        int temp_cell_num = 0;
        string name;
        while(getline(cells,in)){
            stringstream ss(in);
            ss>>name;
            cell_name.push_back(name);
            int area;
            ss>>area;
            cell_area_a.push_back(area);
            ss>>area;
            cell_area_b.push_back(area);
            name_to_order[name] = temp_cell_num;
            temp_cell_num++;
        }
        cell_num = temp_cell_num;
        if(cell_num>=300000){
            sw = 1;
        }
        else if(cell_num<=50000){
            sw = -3;
        }

        int temp_net_num = 0;
        while(getline(nets,in)){
            stringstream ss(in);
            ss>>name;
            ss>>name;
            int temp_net_cell_num = 0;
            ss>>name;
            while(true){
                ss>>name;
                if(name=="}"){
                    break;
                }
                temp_net_cell_num++;
                net_have_cell[temp_net_num].insert(name_to_order[name]);
                cell_have_net[name_to_order[name]].insert(temp_net_num);
                if(cell_have_net[name_to_order[name]].size() > p_max){
                    p_max = cell_have_net[name_to_order[name]].size();
                }
            }
            net_cell_num.push_back(temp_net_cell_num);
            if(temp_net_cell_num < net_cell_num[min_con_cell_net]){
                min_con_cell_net = temp_net_num;
            }
            temp_net_num++;
        }
        net_num = temp_net_num;
        cout<<"init done"<<endl;

        return;
    }


    void gen_start(){
        unordered_set<int> not_check;
        for(int i = 0; i < cell_num; i++){
            not_check.insert(i);
        }
        
        for(auto p : net_have_cell[min_con_cell_net]){
            left_size += cell_area_a[p];
            left_cell[p] = true;
            for(auto n : cell_have_net[p]){
                left_cell_num[n]++;
                net_left_cell[n].insert(p);
            }
            not_check.erase(p);
        }

        while(!not_check.empty()){
            int maxc = *not_check.begin();
            if(left_size>right_size){
                right_size += cell_area_b[maxc];
                right_cell[maxc]=true;
                for(auto n : cell_have_net[maxc]){
                    right_cell_num[n]++;
                    net_right_cell[n].insert(maxc);
                }
                not_check.erase(maxc);
            }
            else{
                left_size += cell_area_a[maxc];
                left_cell[maxc]=true;
                for(auto n : cell_have_net[maxc]){
                    left_cell_num[n]++;
                    net_left_cell[n].insert(maxc);
                }
                not_check.erase(maxc);
            }
        }

        while(!balance(left_size,right_size)){
            if(left_size>right_size){
                for(int i = 0; i < cell_num; i++){
                    if(left_cell[i]){
                        // if(balance(left_size-cell_area_a[i],right_size + cell_area_b[i])){
                            right_size += cell_area_b[i];
                            left_size -= cell_area_a[i];
                            right_cell[i] = true;
                            left_cell[i] = false;
                            for(auto n : cell_have_net[i]){
                                left_cell_num[n]--;
                                right_cell_num[n]++;
                                net_right_cell[n].insert(i);
                                net_left_cell[n].erase(i);
                            }
                            break;
                        // }
                    }
                }
            }
            else{
                for(int i = 0; i < cell_num; i++){
                    if(right_cell[i]){
                        // if(balance(left_size + cell_area_a[i],right_size - cell_area_b[i])){
                            right_size -= cell_area_b[i];
                            left_size += cell_area_a[i];
                            right_cell[i] = false;
                            left_cell[i] = true;
                            for(auto n : cell_have_net[i]){
                                left_cell_num[n]++;
                                right_cell_num[n]--;
                                net_right_cell[n].erase(i);
                                net_left_cell[n].insert(i);
                            }
                            break;
                        // }
                    }
                }
            }
            if(balance(left_size,right_size)){
                break;
            }
        }

        if(balance(left_size,right_size)){
            cout<<"OK"<<endl;
        }
        else{
            cout<<"Not OK"<<endl;
        }
        cout<<"gen start done!"<<endl;
        return;

    }

    inline bool balance(int a, int b){
        return abs(a-b)<((abs(a+b))/10);
    }

    void init_gain(){
        // return ;
        for(int i = 0; i< cell_num; i++){
            gain[i] = 0;
        }
        for(int i = 0; i < net_num;i++){
            if(left_cell_num[i] == 0){
                for(auto s:net_have_cell[i]){
                    gain[s]--;
                }
            }
            else if(left_cell_num[i]==1){
                gain[*net_left_cell[i].begin()]++;
            }

            if(right_cell_num[i] == 0){
                for(auto s:net_have_cell[i]){
                    gain[s]--;
                }
            }
            else if(right_cell_num[i]==1){
                gain[*net_right_cell[i].begin()]++;
            }
        }
        order_gain.clear();
        for(int i = 0; i< cell_num; i++){
            order_gain[gain[i]].insert(i);
            if(gain[i]>max_gain){
                max_gain = gain[i];
            }
        }
        cout<<"init gain done"<<endl;
    }


    int get_max_gain(){
        if(not_lock.empty()){
            return -1;
        }
        bool first_flag = true;
        for(int i = max_gain; i >= -p_max; i--){
            if(!order_gain[i].empty()){
                if(first_flag){
                    max_gain = i;
                    first_flag = false;
                }
                    
                for(int c : order_gain[i]){
                    if(temp_left_cell[c]){
                        if(balance(temp_left_size-cell_area_a[c],temp_right_size+cell_area_b[c])){
                            order_gain[i].erase(c);
                            // cout<<"get done!"<<endl;
                            return c;
                        }
                        else{
                            // cout<<"size too large"<<cell_area[c]<<endl;
                        }
                    }
                    else{
                        if(temp_right_cell[c]!=true){
                            cout<<"error"<<endl;
                        }
                        if(balance(temp_left_size+cell_area_a[c],temp_right_size-cell_area_b[c])){
                            order_gain[i].erase(c);
                            // cout<<"get done!"<<endl;
                            return c;
                        }
                        else{
                            // cout<<"size too large"<<cell_area[c]<<endl;
                        }
                    }
                }
            }
        }




        
        
        
        return -1;



    }

    void fm(){
        cout<<"iter = "<<it<<endl;
        it++;
        for(int i =0; i<cell_num;i++){
            not_lock.insert(i);
        }
        temp_left_cell = left_cell;
        temp_right_cell = right_cell;
        temp_left_cell_num = left_cell_num;
        temp_right_cell_num = right_cell_num;
        temp_left_size = left_size;
        temp_right_size = right_size;
        temp_net_left_cell = net_left_cell;
        temp_net_right_cell = net_right_cell;

        // cout<<"test"<<endl;
        // for(auto g : gain){
        //     cout<< g.second <<endl;
        // }
        vector<int> gain_his, cell_his;
        unordered_map<int,bool> net_lock_left, net_lock_right, net_have_lock;
        while(!not_lock.empty()){
            int gain_cell = get_max_gain();
            if(gain_cell==-1){
                cout<<"can't move!"<<endl;
                break;
            }
            else{
                // cout<<"gain: "<<gain[gain_cell]<<endl;
            }
            int cur_gain = gain[gain_cell];
            // cout<<cur_gain<<endl;
            if(cur_gain <sw){
                break;
            }
            not_lock.erase(gain_cell);
            // if(left_cell[gain_cell]){
            //     for(int n : cell_have_net[gain_cell]){
            //         net_lock_left[n] = true;
            //         if(net_lock_left[n]&&net_lock_right[n]&& !net_have_lock[n]){
            //             for(int ce: net_have_cell[n]){
            //                 not_lock.erase(ce);
            //             }
            //             net_have_lock[n] = true;
            //         }
            //     }
            // }
            // else{
            //     for(int n : cell_have_net[gain_cell]){
            //         net_lock_right[n] = true;
            //         if(net_lock_left[n]&&net_lock_right[n]&& !net_have_lock[n]){
            //             for(int ce: net_have_cell[n]){
            //                 not_lock.erase(ce);
            //             }
            //             net_have_lock[n] = true;
            //         }
            //     }
            // }
            

            update_gain(gain_cell);
            // cout<<"update done!"<<endl;
            gain_his.push_back(cur_gain);
            cell_his.push_back(gain_cell);
            
        }
        int part=0, sum =0, index = 0;
        for(int i = 0; i < gain_his.size(); i++){
            sum += gain_his[i];
            if(sum>part){
                part = sum;
                index = i;
            }
        }
        if(part == 0){
            //
            //
            cout<< "part = 0 end"<<endl;
            return;
        }

        for(int i = 0; i <= index; i++){
            int move_cell = cell_his[i];
            for(auto n : cell_have_net[move_cell]){
                if(left_cell[move_cell]==true){
                    left_cell_num[n]--;
                    right_cell_num[n]++;
                    net_left_cell[n].erase(move_cell);
                    net_right_cell[n].insert(move_cell);
                }
                else{
                    right_cell_num[n]--;
                    left_cell_num[n]++;
                    net_right_cell[n].erase(move_cell);
                    net_left_cell[n].insert(move_cell);
                }
            }
            if(left_cell[move_cell]==true){
                left_cell[move_cell] = false;
                right_cell[move_cell] = true;
                left_size -= cell_area_a[move_cell];
                right_size += cell_area_b[move_cell];
            }
            else{
                right_cell[move_cell] = false;
                left_cell[move_cell] = true;
                right_size -= cell_area_b[move_cell];
                left_size += cell_area_a[move_cell];
            }
        }
        init_gain();

        fm();

    }


    

    void update_gain(int cell){
        // return;
        for(int n : cell_have_net[cell]){
            if(temp_left_cell[cell]){
                if(temp_left_cell_num[n]==1){
                    for(int c : temp_net_right_cell[n]){
                        if(not_lock.count(c)==1){
                            order_gain[gain[c]].erase(c);
                            gain[c]--;
                            order_gain[gain[c]].insert(c);
                        }
                    }
                }
                if(temp_right_cell_num[n]==0){
                    for(int c : temp_net_left_cell[n]){
                        if(not_lock.count(c)==1){
                            order_gain[gain[c]].erase(c);
                            gain[c]++;
                            if(gain[c]>max_gain){
                                max_gain = gain[c];
                            }
                            order_gain[gain[c]].insert(c);
                        }
                    }
                }
                temp_left_cell_num[n]--;
                temp_right_cell_num[n]++;
                temp_net_left_cell[n].erase(cell);
                temp_net_right_cell[n].insert(cell);

            }
            else{
                if(temp_right_cell_num[n]==1){
                    for(int c : temp_net_left_cell[n]){
                        if(not_lock.count(c)==1){
                            order_gain[gain[c]].erase(c);
                            gain[c]--;
                            order_gain[gain[c]].insert(c);
                        }
                    }
                }
                if(temp_left_cell_num[n]==0){
                    for(int c : temp_net_right_cell[n]){
                        if(not_lock.count(c)==1){
                            order_gain[gain[c]].erase(c);
                            gain[c]++;
                            if(gain[c]>max_gain){
                                max_gain = gain[c];
                            }
                            order_gain[gain[c]].insert(c);
                        }
                    }
                }
                temp_right_cell_num[n]--;
                temp_left_cell_num[n]++;
                temp_net_right_cell[n].erase(cell);
                temp_net_left_cell[n].insert(cell);
            }
        }
        if(temp_left_cell[cell]){
            temp_left_size -= cell_area_a[cell];
            temp_right_size += cell_area_b[cell];
            temp_left_cell[cell] = false;
            temp_right_cell[cell] = true;
        }
        else{
            temp_right_size -= cell_area_b[cell];
            temp_left_size += cell_area_a[cell];
            temp_right_cell[cell] = false;
            temp_left_cell[cell] = true;
        }
    }

    void count(ofstream &out){
        cout<<"ready to output"<<endl;
        int ans =0;
        for(int i =0; i<net_num;i++){
            if(left_cell_num[i]!=0&&right_cell_num[i]!=0){
                ans++;
            }
        }
        out<<"cut_size "<<ans<<endl;
        cout<<"cut_size "<<ans<<endl;

        int a = 0, b = 0;
        string temp = "", temp2 = "";
        for(int i = 0; i<cell_num; i++){
            if(left_cell[i]){
                a++;
                temp+=cell_name[i]+"\n";
            }
            else{
                temp2+=cell_name[i]+"\n";
            }
        }
        out<<"A "<<a<<endl;
        out<<temp;
        b = cell_num - a;
        out<<"B "<<b<<endl;
        out<<temp2;

        return ;
    }
    

};






int main(int argc, char* argv[]){
    ifstream cell(argv[1]);
    ifstream net(argv[2]);
    ofstream out(argv[3]);
    FM pda;
    pda.init(cell,net);
    pda.gen_start();
    pda.init_gain();
    pda.fm();
    pda.count(out);

    return 0;
}