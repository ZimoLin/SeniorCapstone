#include "stateHelper.h"

using namespace std;

stateHelper::stateHelper()
{

}

stateHelper::~stateHelper()
{
    
}

string stateHelper::matrices_to_string(vector<vector<vector<double>>> data_matrix)
{
    string res = "";
    for (size_t i = 0; i < data_matrix.size(); i++) {
        res += "{";

        vector<vector<double> > curr_matrix = data_matrix[i];
        for (size_t j = 0; j < curr_matrix.size(); j++) {
            res += "[";

            vector<double> curr_row = curr_matrix[j];
            for (size_t k = 0; k < curr_row.size(); k++) {
                res += to_string(curr_row[k]);
                res += ", ";
            }

            res = res.erase(res.length() - 2, 2);
            res += "], ";
        }

        if ((res.length() > 1) & (curr_matrix.size() != 0)) 
            res.erase(res.length() - 2, 2);

        res += "}, ";
    }
     res.erase(res.length() - 2, 2);
    return res;
}

vector<vector<vector<double>>> stateHelper::string_to_matrices(string line)
{
    vector<vector<vector<double>>> matrices;
        size_t left_cb  = -1;
        size_t right_cb = -1;

        do { //get each matrix

            left_cb  = line.find("{", right_cb+1);
            right_cb = line.find("}", right_cb+1);
            //cout << "{"<< left_cb << endl;
            //cout << "}"<< right_cb << endl;

            if ((int)left_cb == -1 && (int)right_cb == -1) break;
                //cout << "r-l " << right_cb-left_cb + 1; 
            string s_matrix=line.substr(left_cb, right_cb - left_cb+1);
                //cout << "Matrix: " << s_matrix << endl;
            vector <vector<double>> matrix;
            size_t left_bk  = 0;
                //size_t comma    = 0;
            size_t right_bk = 0;
            if (right_cb-left_cb != 1){
                do { //get each row
                    left_bk  = s_matrix.find("[", right_bk+1);                       
                    right_bk = s_matrix.find("]", right_bk+1);
                    string s_row = s_matrix.substr(left_bk,right_bk - left_bk+1);
                    //cout << "Row: " << s_row << endl;
                    vector <double> row;
                    size_t lp = 0;
                    size_t rp = s_row.find(",",0);
                    if (rp > s_row.length()) {
                        rp = s_row.length();
                        string s_element = s_row.substr(lp+1,rp-1);
                        double entry = stod(s_element);
                        row.push_back(entry);
                    } else{
                        do{//get each element
                            //cout << "lp " << lp << ", rp " << rp << endl;
                            string s_element = s_row.substr(lp+1,rp-lp-1);
                            //cout << "element " << s_element << endl;
                            double entry = stod(s_element);

                            //cout << "double " << entry << endl;
                            lp = rp;
                            rp = s_row.find(",",rp+1);
                            row.push_back(entry);
                        } while(rp < s_row.length()+1);
                        rp=s_row.find("]",0);
                        string s_element = s_row.substr(lp+1,rp-lp-1);
                        double entry = stod(s_element);
                        row.push_back(entry);
                    }
                        matrix.push_back(row);
                    } while(right_bk!= s_matrix.length()-2);
                }

                matrices.push_back(matrix);
        } while(right_cb!= line.length()-1);
        
    return matrices;
}