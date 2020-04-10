#include "setting_helper.h"

using namespace std;

setting_helper::setting_helper()
{

}

setting_helper::~setting_helper()
{

}

Setting setting_helper::parse_setting(string fname) {
    string line;
    ifstream myfile (fname);
    double anomaly_level = 0.8;
    vector<string> models ({"ARIMA", "BGMM_REPRESENTATIVE", "BGMM_RECENT", "IFORESTS","OTHER_MODEL"});
    //int [] models_idx = {1,1,1,1};
    vector<vector<double>> barriers;
    vector<string> features ({"deltas_of_data", "delta_timestamp", "day_of_week", "time_of_day","month", "date"});
    //int [] features_idx = {1,1,1,1,1,1};
    vector<string> models_use = models;
    vector<string> features_use = features;
    int prediction_delay = 1000;
    int maxSize = 1000;
    int points_to_reconstruct = 1200;

    if (myfile.is_open()) {

        while (getline(myfile,line)) {

            if ((line.substr(0,3).compare("max")) == 0) {
                size_t found = line.find("=");
                //cout << found << endl;
                string sd = line.substr(found+2,line.length()-1);

                int res = stoi(sd);
                maxSize = res >= 0 ? res : maxSize;

            } else if ((line.substr(0,7).compare("anomaly")) == 0) {

                size_t found = line.find("=");
                string sd = line.substr(found+2,line.length()-1);
                anomaly_level = stod(sd);

            } else if ((line.substr(0,6).compare("models")) == 0) {
                line += ',';
                models_use.clear();

                for (int i = 0; i < (int) models.size(); i++){

                    if (line.find(models[i]) == std::string::npos){

                        models_use.push_back(models[i]);

                    } else if((line.at(line.find(models[i]) + models[i].length()) != ',') &&
                             (line.find(models[i]) + models[i].length() != line.length())){

                        models_use.push_back(models[i]);

                    }
                }
                vector<string> models_use;

            } else if ((line.substr(0,8).compare("barriers")) == 0) {

                size_t left_cb  = 0;
                size_t right_cb = 0;
                do { //Get barriers for each dimension of data in {...}, store as vector of vector
                    
                    left_cb  = line.find("{", right_cb+1);
                    right_cb = line.find("}", right_cb+1);
                    if ((int)left_cb == -1 && (int)right_cb == -1) break;
                    string subs_1=line.substr(left_cb, right_cb - left_cb+1);

                    vector<double> subbar;
                    size_t left_bk  = 0;
                    size_t comma    = 0;
                    size_t right_bk = 0;
                    if (right_cb-left_cb != 1){
                        do { //Get each barriers in the form of [x,y], store as vector of double
                            left_bk  = subs_1.find("[", right_bk+1);
                            comma    = subs_1.find(",", right_bk+2);
                            right_bk = subs_1.find("]", right_bk+1);
                            string lower_b = subs_1.substr(left_bk + 1,comma -left_bk-1);
                            string upper_b = subs_1.substr(comma + 1, right_bk- comma -1);

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
                prediction_delay = stoi(line.substr(found+2, line.length()-1));
            } else if ((line.substr(0,6).compare("points")) == 0){
                size_t found = line.find("=");
                points_to_reconstruct = stoi(line.substr(found+2, line.length()-1));
                //cout << "PR " << points_to_reconstruct << endl;
            }
                    
        }


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
    res.s_max_store = maxSize;
    res.s_prediction_delay = prediction_delay;
    res.s_models = models_use;
    res.s_features = features_use;
    res.s_barriers = barriers;
    res.s_points_to_reconstruct = points_to_reconstruct;
    return res;
}

// Creating a new setting file
void setting_helper::create_setting()
{
    // int dimension = 0;
    double threshold = 0;
    int msp = 0, pd = 0;
    string m_exclude;
    string f_exclude;
    string fname;
    string temp;
    cout << "" << endl;
    cout << "Please enter the name for your settings file" << endl;
    cin >> fname;
    ofstream outfile;
    outfile.open(fname, ios::out | ios::trunc);

    //dimension
    // cout << "Please input the dimension of the time-series data." << endl;
    // cin >> dimension;
    // outfile << "dimension of data = " << dimension << endl;


    //anomaly_level
    cout << "" << endl;
    cout << "Please input the desired anomaly threshold percentage from 0 to 1." << endl;
    cout << "Higher values indicate that more of the data will be called anomalous." << endl;
    cout << "We suggest a default of 0.05." << endl;
    cin >> threshold;
    outfile << "anomaly_level = " << threshold << endl;

    //maximum stored points
    cout << "" << endl;
    cout << "Some models must store data to function. Enter the maximum number of points that should be stored in memory." << endl;
    cout << "More points increase accuracy at the cost of speed and storage." << endl;
    cin >> msp;
    outfile << "maximum_stored_points = " << msp << endl;

    cout << "" << endl;

    //feature_exclude
    cout << "Please input the additional features to be excluded, separated by a comma and a space. Ex: \"day, month, year\"." << endl;
    cout << "We suggest that this is left blank to include all additional features." << endl;
    std::getline(std::cin, f_exclude);
    std::getline(std::cin, f_exclude);
    outfile << "feature_exclude = " << f_exclude << endl;

    if (f_exclude.compare("") != 0) {
        cout << "" << endl;
    }
    //model_exclude 
    cout << "Please input the models to be excluded, separated by a comma and a space. Ex: \"BGMM_RECENT, BGMM_REPRESENTATIVE, ARIMA, IFORESTS\"." << endl;
    cout << "We suggest that this is left blank to include all models." << endl;
    std::getline(std::cin, m_exclude);
    outfile << "models_exclude = " << m_exclude << endl;

    //barrier

    if (m_exclude.compare("") != 0) {
        cout << "" << endl;
    }

    cout << "Now input the preset anomaly barriers for each dimension." << endl;
    cout << "All points within such barriers will be declared anomalous." << endl;
    cout << "Enter barriers enclosed within brackets [] where commas seperate interior values" << endl;
    cout << "and subsequent barriers are separated by a comma and a space." << endl;
    cout << "Ex: \"[-10,5], [10,20]\" indicates that data between -10 and 5 or 10 and 20 are anomalies." << endl;

    int looper = 1;
    string temp_bar;
    string final_bar;
    
    ///
    string temp_name;
    string final_name;
    ///

    cout << "" << endl;

    while(true) {
        /////
        cout << "Please enter the name for dimension " << looper << "." << endl;
        cout << "If no more dimension, enter n." << endl;
        std::getline(std::cin, temp_name);
        if (temp_name.compare("n") == 0) break;
        final_name = final_name + "," + temp_name;
        ////

        cout << "Please enter barriers for dimension " << looper << "." << endl;
        cout << "Leave blank if this dimension has no barriers, but future dimensions do." << endl;
        cout << "If no more barriers are desired in further dimensions or no more dimensions exist, type \"n\"." << endl;
        std::getline(std::cin, temp_bar);
        if (temp_bar.compare("") != 0) {
            cout << "" << endl;
        }
        if (temp_bar.compare("n") == 0) break;

        //check if the input barrier is valid
        bool reset = false;
        size_t left_bk = 0;
        size_t right_bk = 0;
        size_t comma = 0;
        if (temp_bar.compare("") != 0) {

            while (right_bk != temp_bar.length() - 1) {

                left_bk  = temp_bar.find("[", right_bk);
                comma    = temp_bar.find(",", right_bk+2);
                right_bk = temp_bar.find("]", right_bk+1);
                string lower_b = temp_bar.substr(left_bk + 1,comma - left_bk - 1);
                string upper_b = temp_bar.substr(comma + 1, right_bk - comma - 1);

                double lower_bd, upper_bd;

                if (lower_b.compare("-inf") == 0) {
                    lower_bd = -DBL_MAX;
                } else {
                    lower_bd = stod(lower_b);
                }
                if (upper_b.compare("inf") == 0) {
                    upper_bd = DBL_MAX;
                } else {
                    upper_bd = stod(upper_b);
                }

                if (upper_bd < lower_bd) {
                    cout << "One or more upper bound is lower than its lower bound. Please re-enter all bounds for the barrier of the current dimension." << endl;
                    reset = true;
                    break;
                }
            }
        }
        if (reset == true) {
            reset = false;
            continue;
        }
        /////////

        temp_bar = "{" + temp_bar + "}";
        final_bar = final_bar + "," + temp_bar;
        looper ++;
    }

    ////
        final_name = final_name.substr(1, final_name.length() - 1);
        outfile << "dimension_name = " << final_name << endl;
    ////
    
    if (!final_bar.empty()){
        final_bar = final_bar.substr(1, final_bar.length() - 1);
        outfile << "barriers = " << final_bar << endl;
    } else {
        outfile << "barriers =" << endl;
    }
    

    //Prediction Delay
    cout << "In order for our models to accurately deduce anaomalies, they require some initial data." << endl;
    cout << "As a result, they will not be able to predict for the first few samples." << endl;
    cout << "Please enter how many samples you will allow the models to acclimate on? We suggest around 500 points." << endl;
    cin >> pd;
    outfile << "prediction_delay = " << pd << endl;

    //Points to Reconstruct
    int pr = 0;
    cout << "How many points do you want to Reconstruct" << endl;
    cin  >> pr;
    outfile << "points_to_reconstruct = " << pr << endl;


    cout << "\nThank you. Your settings file can be found at " << fname << endl;
    
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
