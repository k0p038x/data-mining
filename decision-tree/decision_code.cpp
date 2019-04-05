#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <vector>
#include <math.h>
#include <queue>
#include <set>
using namespace std;

#define vi vector<int>
#define pair_int pair<int,int>
#define MAX_ROWS 1730

int table[MAX_ROWS][7];
int total_rows, total_attr = 7;
string attributes[6] = {"buying", "maint", "doors", "persons", "lug boot", "safety"};
map<string, int> encode_string[7];
map<int, string> decode_int[7];


void scanData()
{
    fstream fin;
    fin.open("cardata.txt");

    string str;
    int assign[7]={1,1,1,1,1,1,1};
    
    int total = 0;
    while(fin)
    {
        getline(fin, str);
        if(str == "")
            break;
        // cout << str << "\n";

        
        string arr[7];
        int idx = 0;
        string tmp = "";
        for(int i=0;i<str.length();i++)
        {
            if(str[i]==',')
            {
                arr[idx] = tmp;
                tmp="";
                idx++;
            }
            else
                tmp+=str[i];
        }
        arr[idx] = tmp;
        for(int i=0;i<7;i++)
        {
            string item = arr[i];
            if(encode_string[i][item]==0)
            {
                encode_string[i][item] = assign[i];
                decode_int[i][assign[i]] = item;
                // cout << attributes[i] << " " << assign[i] << " : " << item << "\n";
                assign[i]++;
            }
            table[total][i] = encode_string[i][item];
        }
        total++;
        total_rows++;
    }
    
    fin.close();
}


void bfs(int ty)
{   
    int ex_count = 0;
    vi fattr;
    for(int i=0;i<6;i++)
        fattr.push_back(i);
    
    vi frows;
    for(int i=0;i<total_rows;i++)
        frows.push_back(i);

    queue<string> q;
    queue< pair<vi,vi> > qData;
    qData.push({frows, fattr});
    q.push("root");

    while(!q.empty())
    {
        pair<vi, vi> f = qData.front(); qData.pop();
        string nodeName = q.front();    q.pop();
        vi rows = f.first;
        vi attr = f.second;
        if(rows.size()==0)
            break;
        cout << nodeName;
        set<int> pos_class;
        map<int,int> count_class;
        for(auto i : rows)
        {
            pos_class.insert(table[i][6]);
            count_class[table[i][6]]++;
        }
        
        // Base cases : 
        // 1. No attr
        if(attr.size()==0)
        {
            int mx = 0;
            int max_ind = -1;
            for(auto i : count_class)
            {
                if(i.second > mx)
                {
                    mx = i.second;
                    max_ind = i.first;
                }
            }
            
            // Ended
            // final value - max_ind
            cout << " :: output : " << decode_int[6][max_ind] << "\n";
            continue;
        }

        // 2. Only single class
        if(pos_class.size()==1)
        {
            int val;
            for(auto i : pos_class)
            {
                val = i;
            }
            // Ended
            // final Value - val 
            cout << " :: output : " << decode_int[6][val] << "\n";
            continue;
        }

        // selection
        set<int> pos_attr_values[10];
        map<pair<pair_int,int>,int> count_attr_class;
        map<pair<int,int>,int> count_attr;

        for(int i=0;i<attr.size();i++)
        {
            int cur_attr = attr[i];
            for(auto j : rows)
            {
                int attr_value = table[j][cur_attr];
                int class_value = table[j][6];
                pos_attr_values[cur_attr].insert(attr_value);
                count_attr_class[{{cur_attr, attr_value}, class_value}]++;
                count_attr[{cur_attr, attr_value}]++;
            }
        }

        double info_d = 0.0;
        double d = rows.size() * 1.0;
        for(auto i : pos_class)
        {
            if(count_class[i]==0)
                continue;
            info_d += (count_class[i]/d)*(log2(count_class[i]) - log2(d));
        }
        info_d*=-1;
        // cout << info_d << "\n";
        vector<double> info_attr;
        vector<double> gain_attr;
        vector<double> gain_ratio;
        for(int i=0;i < attr.size();i++)
        {
            int cur_attr = attr[i];
            double info_tmp = 0.0;
            double split_tmp = 0.0;
            for(auto j : pos_attr_values[cur_attr])
            {
                int attr_value = j;
                int cnt = count_attr[{cur_attr,attr_value}];
                double var = 0.0;
                for(auto k : pos_class)
                {
                    int num = count_attr_class[{{cur_attr, attr_value}, k}];
                    if(num==0)
                        continue;
                    
                    var+= ((num*1.0)/cnt)*(log2(num)-log2(cnt));
                }
                var*=(cnt/d);
                split_tmp+=((cnt/d)*(log2(cnt) - log2(d)));
                info_tmp+=var;
            }
            info_tmp*=-1;
            split_tmp*=-1;
            
            info_attr.push_back(info_tmp);
            // cout << attributes[cur_attr] << " : " << info_d - info_tmp << "\n";
            gain_attr.push_back(info_d - info_tmp);
            gain_ratio.push_back((info_d - info_tmp)/split_tmp);
        }

        int selected_attr = attr[0];
        if(ty==0)
        {
            double mx = gain_attr[0];
        
            for(int i=1;i<gain_attr.size();i++)
            {
                if(mx < gain_attr[i])
                {
                    mx = gain_attr[i];
                    selected_attr = attr[i];
                }
            }

            // cout << "Selected idx : " << selected_attr << " " << mx << "\n";
        }
        if(ty==1)
        {
            double mx = gain_ratio[0];
        
            for(int i=1;i<gain_ratio.size();i++)
            {
                if(mx < gain_ratio[i])
                {
                    mx = gain_ratio[i];
                    selected_attr = attr[i];
                }
            }
        }
        
        // selected_attr variable fixed 
        // cout << "selected_attr : " << attributes[selected_attr] << "\n";
        for(auto i : pos_attr_values[selected_attr])
        {
            vi child_rows;
            vi child_attr;
            string child_name;

            for(auto j : attr)
            {
                if(j!=selected_attr)
                    child_attr.push_back(j);
            }
            for(auto j : rows)
            {
                if(table[j][selected_attr]==i)
                {
                    child_rows.push_back(j);
                }
            }

            child_name = nodeName + " | " + attributes[selected_attr]+ " : " + decode_int[selected_attr][i];
            // cout << child_name << " , " << i << " ; ";

            q.push(child_name);
            qData.push({child_rows, child_attr});
            
        }
        cout << "\n";
    }
}

void cart()
{
    vi fattr;
    for(int i=0;i<6;i++)
        fattr.push_back(i);
    
    vi frows;
    for(int i=0;i<total_rows;i++)
        frows.push_back(i);

    queue<string> q;
    queue< pair<vi,vi> > qData;
    qData.push({frows, fattr});
    q.push("root");

    while(!q.empty())
    {
        pair<vi ,vi> f = qData.front(); qData.pop();
        vi rows = f.first;
        vi attr = f.second;
        string nodeName = q.front();    q.pop();
        cout << nodeName ;

        set<int> pos_class_values;
        map<int,int> count_class_values;

        for(auto i : rows)
        {
            pos_class_values.insert(table[i][6]);
            count_class_values[table[i][6]]++;
        }

        // Base Cases :
        // 1. No attributes
        if(attr.size()==0)
        {
            int mx = -1;
            int mx_idx = -1;
            for(auto i : count_class_values)
            {
                if(i.second > mx)
                {
                    mx = i.second;
                    mx_idx = i.first;
                }
            }
            cout << " :: output : " << decode_int[6][mx_idx] << "\n";
            continue;
        }
        
        // only one possible class value
        else if(pos_class_values.size()==1)
        {
            int val = -1;
            for(auto i : pos_class_values)
            {
                val = i;
            }

            cout << " :: output : " << decode_int[6][val] << "\n";
            continue;
        }

        double gini_d = 0.0;
        double d = (double)rows.size();
        for(auto i : count_class_values)
        {
            gini_d+=((i.second*i.second)/(d*d));
            // cout << gini_d << "\n";
        }
        gini_d=1-gini_d;
        // cout << "gini : " << gini_d << "\n";
        set<int> pos_attr_values[10];
        map<pair<pair_int,int>,int> count_attr_class;
        map<pair<int,int>,int> count_attr;
        map<int, double> gini_attr;
        map<int, pair<vi, vi> > gini_subsets;
        for(int i=0;i<attr.size();i++)
        {
            int cur_attr = attr[i];
            for(auto j : rows)
            {
                int attr_value = table[j][cur_attr];
                int class_value = table[j][6];
                pos_attr_values[cur_attr].insert(attr_value);
                count_attr_class[{{cur_attr, attr_value}, class_value}]++;
                count_attr[{cur_attr, attr_value}]++;
            }
        }

        for(int i=0;i<attr.size();i++)
        {
            int cur_attr = attr[i];
            // cout << "In " << attributes[cur_attr] << " : \n";
            if(pos_attr_values[cur_attr].size()<2)
            {
                continue;
            }
            vi possible_values;

            for(auto j : pos_attr_values[cur_attr])
            {
                possible_values.push_back(j);
            }

            int len = possible_values.size();

            for(int j=1;j<(1<<len)-1;j++)
            {
                vi s1;
                vi s2;
                for(int k=0;k<len;k++)
                {
                    if((j & (1<<k))>0)
                    {
                        s1.push_back(possible_values[k]);
                    }
                    else
                    {
                        s2.push_back(possible_values[k]);
                    }  
                }


                double d1 = 0;
                double d2 = 0;
                map<int, int> count_class_values_1;
                map<int, int> count_class_values_2;

                for(auto p : s1)
                {
                    d1+=count_attr[{cur_attr, p}];

                    for(auto q : pos_class_values)
                    {
                        count_class_values_1[q]+=count_attr_class[{{cur_attr, p}, q}];
                    }
                }

                for(auto p : s2)
                {
                    d2+=count_attr[{cur_attr, p}];
                    for(auto q : pos_class_values)
                    {
                        count_class_values_2[q]+=count_attr_class[{{cur_attr, p}, q}];
                    }
                }

                double gini_tmp1 = 0;
                double gini_tmp2 = 0;

                for(auto p : count_class_values_1)
                {
                    gini_tmp1+=((p.second*p.second)/(d1*d1));
                }
                gini_tmp1 = 1 - gini_tmp1;
                gini_tmp1 *= (d1/d);

                for(auto p : count_class_values_2)
                {
                    gini_tmp2+=((p.second*p.second)/(d2*d2));
                }
                gini_tmp2 = 1 - gini_tmp2;
                gini_tmp2 *= (d2/d);
                double gain_val = gini_d - (gini_tmp1 + gini_tmp2);
                // cout << "gain_attr : " << gini_d - (gini_tmp1 + gini_tmp2) << "\n";
                if(gini_attr[cur_attr]==0 || (gini_attr[cur_attr] < gain_val))
                {
                    gini_attr[cur_attr] = gain_val;
                    gini_subsets[cur_attr] = {s1, s2};
                }
                
            }
        }

        
        if(gini_attr.size()==0)
        {
            int mx1 = -1;
            int mx_idx = -1;
            for(auto i : count_class_values)
            {
                if(i.second > mx1)
                {
                    mx1 = i.second;
                    mx_idx = i.first;
                }
            }
            cout << " :: output : " << decode_int[6][mx_idx] << "\n";
            continue;
        }

        int idx = -1;
        double mx = -1000000;
        vi s1, s2;
        for(auto i : gini_attr)
        {
            if(i.second > mx)
            {
                idx = i.first;
                s1 = gini_subsets[i.first].first;
                s2 = gini_subsets[i.first].second;
                mx = i.second;
            }
        }

        
        vi chRows1, chRows2;
       
        for(auto i : s1)
        {
            for(auto j : rows)
            {
                if(table[j][idx] == i)
                {
                    chRows1.push_back(j);
                }
            }
        }

        for(auto i : s2)
        {
            for(auto j : rows)
            {
                if(table[j][idx] == i)
                {
                    chRows2.push_back(j);
                }
            }
        }

        qData.push({chRows1, attr});
        qData.push({chRows2, attr});
        string child_name = nodeName + " | " + attributes[idx]+ " : {";
        string tmp = child_name;
        for(auto i : s1)
        {
            tmp += " " + decode_int[idx][i];
        }
        q.push(tmp+" }");
        tmp = child_name;
        for(auto i : s2)
        {
            tmp +=  " " + decode_int[idx][i];
        }
        q.push(tmp+" }");
        cout << "\n";
    }

}

int main()
{
    scanData();
    cout << decode_int[2][1] << "\n";
    cout << "Selected Algo : \n 0 for ID3 \n 1 for C4.5\n 2 for CART\n";
    cout << "Enter : ";

    int x;  
    cin >> x;   
    
    if(x==1 || x==0)
        bfs(x);
    else if(x==2)
    {
        cart();
    }
    else
    {
        cout << "Wrong entry!!" << endl;
    }
    return 0;
}