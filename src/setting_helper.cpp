#include "setting_helper.h"

setting_helper::setting_helper()
{

}

setting_helper::~setting_helper()
{
    
}

Setting setting_helper::parseSetting(string fname) {
    string line;
    ifstream myfile (fname);
    double anomaly_level = 0.8;
    vector<string> models ({"ARIMA", "BGMM","IF","OTHER_MODEL"});
    //int [] models_idx = {1,1,1,1};
    vector<vector<double>> barriers;
    vector<string> features ({"deltas_of_data", "delta_timestamp", "day_of_week", "time_of_day","month", "date"});
    //int [] features_idx = {1,1,1,1,1,1};
    vector<string> models_use = models;
    vector<string> features_use = features;
    int prediction_delay = 500;
    int maxSize = 100;


    if (myfile.is_open()) {
        while (getline(myfile,line)) {
            if ((line.substr(0,3).compare("max")) == 0) {
                size_t found = line.find("=");
                //cout << found << endl;
                string sd = line.substr(found+2,line.length()-1);

                int res = stoi(sd);
                maxSize = res >= 0 ? res : maxSize;

                ///cout << sd << endl;
                //dimension = stoi(sd);
                // cout << maxSize << endl;
            } else if ((line.substr(0,7).compare("anomaly")) == 0) {
                size_t found = line.find("=");
                //cout << found << endl;
                string sd = line.substr(found+2,line.length()-1);
                ///cout << sd << endl;
                anomaly_level = stod(sd);
                // cout << anomaly_level << endl;
            } else if ((line.substr(0,6).compare("models")) == 0) {
                line += ',';
                models_use.clear();
                for (int i = 0; i < (int) models.size(); i++){
                    if (line.find(models[i]) == std::string::npos){
                        //TODO: get only correct model
                        //models_idx[i]=0;
                        models_use.push_back(models[i]);

                    } else if((line.at(line.find(models[i]) + models[i].length()) != ',') &&
                             (line.find(models[i]) + models[i].length() != line.length())){
                        models_use.push_back(models[i]);
                    }
                }
            } else if ((line.substr(0,8).compare("barriers")) == 0) {
                size_t left_cb  = 0;
                size_t right_cb = 0;
                do { //Get barriers for each dimension of data in {...}, store as vector of vector
                    
                    left_cb  = line.find("{", right_cb+1);
                    right_cb = line.find("}", right_cb+1);
                    string subs_1=line.substr(left_cb, right_cb - left_cb+1);
                    // cout << line << endl;
                    // cout << "left_cb" << left_cb << endl;
                    // cout << "right_cb" << right_cb << endl;
                    // cout << "first sub" << subs_1 << endl;
                    vector<double> subbar;
                    size_t left_bk  = 0;
                    size_t comma    = 0;
                    size_t right_bk = 0;
                    if (right_cb-left_cb != 1){
                        do { //Get each barriers in the form of [x,y], store as vector of double
                            left_bk  = subs_1.find("[", right_bk+1);
                            comma    = subs_1.find(",", right_bk+2);
                            right_bk = subs_1.find("]", right_bk+1);
                            // cout << "subs1"<<subs_1 << endl;
                            // cout << "left_bk" << left_bk << endl;
                            // cout << "comma" << comma<< endl;
                            // cout << "right_bk" << right_bk << endl;
                            string lower_b = subs_1.substr(left_bk + 1,comma -left_bk-1);
                            string upper_b = subs_1.substr(comma + 1, right_bk- comma -1);
                            // cout << "lowerB " << lower_b << endl;
                            // cout << "upperB " << upper_b << endl; 
                            double lower_bd, upper_bd;

                            if (lower_b.compare("-inf") == 0) {
                                lower_bd = -DBL_MAX;
                                
                            } else {
                                //cout << lower_b << endl;
                                lower_bd = stod(lower_b);
                                
                            }

                            if (upper_b.compare("inf") == 0) {
                                upper_bd = DBL_MAX;
                            } else {
                                //cout << upper_b << endl;
                                upper_bd = stod(upper_b);
                            }
                            
                            subbar.push_back(lower_bd);
                            subbar.push_back(upper_bd);

                        } while(right_bk!= subs_1.length()-2);
                    }
                    barriers.push_back(subbar);

                } while(right_cb!= line.length()-1);

            } else if ((line.substr(0,7).compare("feature")) == 0) {
                line += ',';
                features_use.clear();
                for (int i = 0; i < (int) features.size(); i++){
                   // cout << features[i] << endl;
                    if (line.find(features[i]) == std::string::npos){
                        //features_idx[i]=0;
                        features_use.push_back(features[i]);
                    } else if ((line.find(features[i]) + features[i].length() != line.length()) &&
                             (line.at(line.find(features[i]) + features[i].length()) != ',')) {
                             
                        features_use.push_back(features[i]);
                    }
                }
            } else if ((line.substr(0, 10).compare("prediction")) == 0) {
                size_t found = line.find("=");
                // cout << line.substr(found+2, line.length()-1) << endl;
                prediction_delay = stod(line.substr(found+2, line.length()-1));
            }
        }


        // dimension = max(dimension, (int) barriers.size());
        
        // while ((int) barriers.size() != dimension) {
        //     vector<double> dummy;
        //     barriers.push_back(dummy);
        // }


        for (int i = 0; i < (int) models_use.size(); i++) {
            cout << models_use[i] << endl;
        }
        
        for (int i = 0; i < (int) features_use.size(); i++) {
            cout << features_use[i] << endl;
        }
        for (int i = 0; i < (int) barriers.size(); i++) {
            cout << "{";
            for (size_t j = 0; j < barriers[i].size(); j++) {
                cout << barriers[i][j] << " ";
            }
            cout << "}" << endl;
        }

        myfile.close();
    }
    else {
        cout << "Unable to open file";
    }

    Setting res;

    res.s_anomaly_level = anomaly_level;
    //res.s_dimension = dimension;
    res.s_prediction_delay = prediction_delay;
    res.s_models = models_use;
    res.s_features = features_use;
    res.s_barriers = barriers;
    res.s_max_store = maxSize;

    return res;
}


//print the existing setting
//input: string filename
// void printSetting(string fname)
// {
//  string line;
//      ifstream myfile (fname);
//      stringstream ss;
//     string temp; 
//     int found;

//     if (myfile.is_open()) {
//         while (getline (myfile,line)) {
//          ss << line;
//         }
//          while (!ss.eof()) { 
//                 ss >> temp; 
//                 if (stringstream(temp) >> found) {
//                     cout << found << " "; 
//                 }
//                 /* To save from space at the end of string */
//                 temp = ""; 
//             } 
//             myfile.close();
//     }
//     else cout << "Unable to open file";
// }

// Creating a new setting file
void setting_helper::createSetting()
{

    // int dimension = 0;
    double threshold = 0;
    int msp = 0, pd = -1; 
    string m_exclude;
    string f_exclude;
    string fname;
    string temp;
    string sampleSize;
    cout << "Please name your setting file" << endl;
    cin >> fname;
    ofstream outfile;
    outfile.open(fname, ios::out | ios::trunc);

    //dimension
    // cout << "Please input the dimension of the time-series data." << endl;
    // cin >> dimension;
    // outfile << "dimension of data = " << dimension << endl;


    //anomaly_level
    cout << "Please input the desired anomaly threshold percentage. (from 0 to 1): " << endl;
    cin >> threshold;
    outfile << "anomaly_level = " << threshold << endl;

    //maximum stored points
    cout << "Please input the desired maximum stored points." << endl;
    cin >> msp;
    outfile << "maximum_stored_points = " << msp << endl;

    cout << "" << endl;

    //feature_exclude
    cout << "Please input the features to be excluded, separated by coma and space. (i.e. day, month, year)" << endl;
    cout << "All features = deltas_of_data, delta_timestamp, day_of_week, time_of_day, month, date" << endl;
    std::getline(std::cin, f_exclude);
    std::getline(std::cin, f_exclude);
    outfile << "feature_exclude = " << f_exclude << endl;

    cout << "" << endl;
    //model_exclude 
    cout << "Please input the models to be excluded, separated by coma and space. (i.e. BGMM, IF)" << endl;
    cout << "All models = ARIMA, BGMM, IF, OTHER_MODEL" << endl;
    std::getline(std::cin, m_exclude);
    outfile << "models_exclude = " << m_exclude << endl;

    //barrier

    cout << "" << endl;

    cout << "Please input the barriers for normal data for each dimension." << endl;
    cout << "use -inf and inf for negative and positive infinities." << endl;
    cout << "example: [-inf,5], [10,20] " << endl;
    cout << "each [] is a set of barriers." << endl;

    int looper = 1;
    string temp_bar;
    string final_bar;

    while(true) {
        cout << "Please barriers for dimension " << looper << endl;
        cout << "if not more barriers are needed, enter n" << endl;
        std::getline(std::cin, temp_bar);
        if (temp_bar.compare("n") == 0) break;
        temp_bar = "{" + temp_bar + "}";
        final_bar = final_bar + "," + temp_bar;
        looper ++;
    }
    
    if (!final_bar.empty()){
        final_bar = final_bar.substr(1, final_bar.length() - 1);
        outfile << "barriers = " << final_bar << endl;
    }

    //Sample Size
    cout << "" << endl;
    cout << "In order for our models to accurately deduce anaomalies, they require some initial data." << endl;
    cout << "As a result, they will not be able to predict for the first few samples." << endl;
    cout << "Please enter how many samples you will allow the models to acclimate on? We suggest around 500 points." << endl;
    cin >> pd;
    outfile << "prediction_delay = " << pd << endl;
    
    // Eigen::MatrixXd range (dimension,2);

    // cout << "For each dimension, please enter the threshold and range" << endl;

    // for (int i = 0; i < dimension; i++) {
    //  cout << "dimension " << i+1 << endl;
    //  cout << "Input the range of normal data for each dimension." << endl;
    //  cout << " *Note: The range recorded would superceded the model predicion."
    //   <<"If the result from the model is desired, press n. Otherwise, enter the number." << endl;
    //  cout << "lower bound: " << endl;

    //  cin >> temp;
    //  if(temp == "n")
    //      range(i,0) = NULL;
    //  else
    //      range(i,0) = stoi(temp);
    //  cout << "upper bound: ";
    //  cin >> temp;
    //  if(temp == "n")
    //      range(i,1) = NULL;
    //  else
    //      range(i,1) = stoi(temp);

    //  outfile << "Dimension "<< i+1 << ": "<<range(i,0) << ", " << range(i,1) << endl;
    // }
    outfile.close();    
}
