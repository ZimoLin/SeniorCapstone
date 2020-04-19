//      MIT License
//
//  Copyright © 2017 Michael J Simms. All rights reserved.
//
//      Permission is hereby granted, free of charge, to any person obtaining a copy
//      of this software and associated documentation files (the "Software"), to deal
//      in the Software without restriction, including without limitation the rights
//      to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//      copies of the Software, and to permit persons to whom the Software is
//      furnished to do so, subject to the following conditions:
//
//      The above copyright notice and this permission notice shall be included in all
//      copies or substantial portions of the Software.
//
//      THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//      IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//      FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//      AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//      LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//      OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//      SOFTWARE.

#include "IsolationForest.h"
#include "stateHelper.h"
#include <math.h>
#include <iostream>

using namespace std;

#define H(i) (log(i) + 0.5772156649)
#define C(n) (2 * H(n - 1) - (2 * (n - 1) / n))

namespace IsolationForest
{
    // Constructor.
    Node::Node() :
    m_splitValue(0.0),
    m_left(NULL),
    m_right(NULL)
    {
    }

    // Constructor.
    Node::Node(size_t index, double splitValue, int count) :
    m_index(index),
    m_splitValue(splitValue),
    m_count(count),
    m_left(NULL),
    m_right(NULL)
    {
    }

    // Destructor.
    Node::~Node()
    {
        DestroyLeftSubtree();
        DestroyRightSubtree();
    }

    void Node::SetLeftSubTree(Node* subtree)
    {
        DestroyLeftSubtree();
        m_left = subtree;
    }

    void Node::SetRightSubTree(Node* subtree)
    {
        DestroyRightSubtree();
        m_right = subtree;
    }

    void Node::DestroyLeftSubtree()
    {
        if (m_left)
        {
            delete m_left;
            m_left = NULL;
        }
    }

    void Node::DestroyRightSubtree()
    {
        if (m_right)
        {
            delete m_right;
            m_right = NULL;
        }
    }
    int Node::getCount()
    {
        return m_count;
    }


    // Returns the node as a JSON string.
    string Node::Dump() const
    {
        string data = "{";

        data.append("'Feature Name': '");
        data.append(to_string(this->m_index));
        data.append("', 'Split Value': ");
        data.append(to_string(m_splitValue));
        data.append(", 'Left': ");
        if (this->m_left)
            data.append(this->m_left->Dump());
        else
            data.append("{}");
        data.append(", 'Right': ");
        if (this->m_right)
            data.append(this->m_right->Dump());
        else
            data.append("{}");
        data.append("}");
        return data;
    }

    // Constructor.
    Forest::Forest(vector<vector<double>>& initial_input, int max_stored_data_points, int points_to_reconstruct) :
    model(initial_input, max_stored_data_points, points_to_reconstruct)
    {
        this->m_randomizer_ = new Randomizer();
        this->m_numTreesToCreate_ = 10;
        this->m_subSamplingSize_ = 10;
        this->max_stored_data_points_ = max_stored_data_points;
        this->points_to_reconstruct_ = points_to_reconstruct;
        this->normalized_kept_points_ = false;
        for (auto data : initial_input)
            push_data(data);
    }

    // Constructor.
    Forest::Forest(uint32_t numTrees, uint32_t subSamplingSize) :
    model("")
    {
        this->m_randomizer_ = new Randomizer();
        this->m_numTreesToCreate_ = numTrees;
        this->m_subSamplingSize_ = subSamplingSize;
    }

    // Constructor for recovery from saved state
    Forest::Forest(string saved_state) : model(saved_state)
    {
        stateHelper helper;
        vector<vector<vector<double>>> data = helper.string_to_matrices(saved_state);

        // data[0][0] will be the vector that holds settings
        vector<double> settings = data[0][0];

        max_stored_data_points_ = settings[0];
        points_to_reconstruct_ = settings[1];
        point_count_ = settings[2];
        dSize = settings[3];
        dNum = settings[4];
        normalized_kept_points_ = settings[5] == 1.0 ? true : false;

        m_featureValues_ = data[1];
        m_trainData_ = data[2];

        create();
    }



    // Destructor.
    Forest::~Forest()
    {
        destroy_randomizer();
        destroy();
    }

    void Forest::set_randomizer(Randomizer* newRandomizer)
    {
        destroy_randomizer();
        m_randomizer_ = newRandomizer;
    }

    // Creates and returns a single tree. As this is a recursive function,
    // depth indicates the current depth of the recursion.
    NodePtr Forest::create_tree(const vector<vector<double>>& featureValues, const vector<vector<double>>& trainData, size_t depth)
    {
        // Sanity check.
        size_t featureValuesLen = featureValues.size();
        if (featureValuesLen <= 1)
        {
            return NULL;
        }

        // If we've exceeded the maximum desired depth, then stop.
        if ((m_subSamplingSize_ > 0) && (depth >= m_subSamplingSize_))
        {
            return NULL;
        }

        // Randomly select a feature.
        size_t selectedFeatureIndex = (size_t)m_randomizer_->RandUInt64(0, featureValuesLen - 1);
                
        // Get the value list to split on.
        vector<double> featureValueList = featureValues[selectedFeatureIndex];

        if (featureValueList.size() == 0)
        {
            return NULL;
        }

        // Randomly select a split value.
        size_t splitValueIndex = 0;
        if (featureValueList.size() > 1)
            splitValueIndex = (size_t)m_randomizer_->RandUInt64(0, featureValueList.size() - 1);

        double splitValue = featureValueList[splitValueIndex];

        // Create a tree node to hold the split value.
        NodePtr tree = new Node(selectedFeatureIndex, splitValue, trainData.size());

        if (tree)
        {
            // Create two versions of the feature value set that we just used,
            // one for the left side of the tree and one for the right.
            vector<vector<double>> tempFeatureValues;
            for (vector<double> data : featureValues)
                tempFeatureValues.push_back(data);

            vector<vector<double>> leftTrainData;
            vector<vector<double>> rightTrainData;
            for (vector<double> data : trainData) {
                if (data[splitValueIndex] < splitValue) {
                    leftTrainData.push_back(data);
                } else {
                    rightTrainData.push_back(data);
                }
            }

            // Create the left subtree.

            vector<double> leftFeatureValueList;
            for (double d : featureValueList){
                if (d < splitValue)
                    leftFeatureValueList.push_back(d);
            }

            tempFeatureValues[selectedFeatureIndex] = leftFeatureValueList;
            tree->SetLeftSubTree(create_tree(tempFeatureValues, leftTrainData, depth + 1));


            // Create the right subtree.
            if (splitValueIndex < featureValueList.size() - 1)
            {
                vector<double> rightFeatureValueList;
                for (double d : featureValueList){
                    if (d > splitValue)
                        rightFeatureValueList.push_back(d);
                }

                tempFeatureValues[selectedFeatureIndex] = rightFeatureValueList;
                tree->SetRightSubTree(create_tree(tempFeatureValues, rightTrainData, depth + 1));
            }

        }
        return tree;
    }

    // Creates a forest containing the number of trees specified to the constructor.
    void Forest::create()
    {
        m_trees_.reserve(m_numTreesToCreate_);


        for (size_t i = 0; i < m_numTreesToCreate_; ++i)
        {
            NodePtr tree = create_tree(m_featureValues_, m_trainData_, 0);

            if (tree)
            {
                m_trees_.push_back(tree);
            }
        }
    }

    double Forest::process_input(vector<double>& input_data)
    {
        push_data(input_data);
        if (point_count_ == 0) {
            create();
            point_count_ = points_to_reconstruct_;
        }
        point_count_--;
        double score = normalized_score(input_data);
        double worse_values = 0;
        for (size_t j = 0; j < m_trainData_.size(); ++j) {
            double comparison_score = normalized_score(m_trainData_[j]);
            
            if (comparison_score <= score) 
                worse_values++;
        }
        if (point_count_ == 0) {
            destroy();
        }  

        double proportional_likelihood = worse_values / m_trainData_.size();

        if (proportional_likelihood > 0.999) 
            return 25.0;
        else if (proportional_likelihood < 0.001)
            return -25.0;
        return log(proportional_likelihood/(1-proportional_likelihood));
    }

    void Forest::process_feedback(vector<double>& input_data, bool isAnomaly)
    {
        (void) input_data;
        (void) isAnomaly;
        return;
    }


    // Scores the sample against the specified tree.
    double Forest::score(vector<double>& data, const NodePtr tree)
    {
        double depth = (double)0.0;

        NodePtr currentNode = tree;
        NodePtr lastNode = NULL;
        while (currentNode)
        {
            lastNode = currentNode;
            if (data[currentNode->Index()] < currentNode->SplitValue())
            {
                currentNode = currentNode->Left();
            } else {
                currentNode = currentNode->Right();
            }
            ++depth;
        }
        if (lastNode != NULL) {
            int curCount = lastNode->getCount();
            depth += curCount == 0 ? 0 : log2(curCount) * (m_featureValues_.size());
        }

        return depth;
    }

    // Scores the sample against the entire forest of trees. Result is the average path length.
    double Forest::score(vector<double>& data)
    {
        double avgPathLen = (double)0.0;

        if (m_trees_.size() > 0)
        {
            NodePtrList::const_iterator treeIter = m_trees_.begin();
            while (treeIter != m_trees_.end())
            {
                avgPathLen += (double)score(data, (*treeIter));
                ++treeIter;
            }
            avgPathLen /= (double)m_trees_.size();
        }
        return avgPathLen;
    }

    // Scores the sample against the entire forest of trees. Result is normalized so that values
    // close to 1 indicate anomalies and values close to zero indicate normal values.
    double Forest::normalized_score(vector<double>& data)
    {
        double res_score = (double)0.0;
        size_t numTrees = m_trees_.size();

        if (numTrees > 0)
        {
            double avgPathLen = (double)0.0;

            NodePtrList::const_iterator treeIter = m_trees_.begin();
            while (treeIter != m_trees_.end())
            {
                avgPathLen += (double)score(data, (*treeIter));
                ++treeIter;
            }
            avgPathLen /= (double)numTrees;

            if (numTrees > 1)
            {
                double exponent = -1.0 * (avgPathLen / C(numTrees));
                res_score = pow(2, exponent);
            }
        }
        return 1 - res_score;

    }

    // push a new data point into feature value list.
    void Forest::push_data(vector<double>& data)
    {
        if (dSize == 0)
            dSize = (int)data.size();
        else if ((int)data.size() != dSize)
            throw "Input data has wrong dimensionality";

        ++dNum;

        if ((int)m_trainData_.size() == max_stored_data_points_){
            if (normalized_kept_points_){
                double add_odds = ((double) max_stored_data_points_) / dNum;
                double temp = ((double)rand() / (RAND_MAX));
                if (temp < add_odds) {
                    int index_to_drop = rand() % max_stored_data_points_;
                    m_trainData_[index_to_drop] = data;
                    for (int i = 0; i < dSize; ++i)
                        m_featureValues_[i][index_to_drop] = data[i];
                } else {
                    m_trainData_.erase(m_trainData_.begin());
                    m_trainData_.push_back(data);
                    for (int i = 0; i < dSize; ++i)
                        m_featureValues_[i].erase(m_featureValues_[i].begin());
                    push_transpose_data(data);
                }
            }
        } else {
            m_trainData_.push_back(data);    
            push_transpose_data(data);
        }
    }

    // push data into the transpose matrix
    void Forest::push_transpose_data(vector<double>& data)
    {
        if (m_featureValues_.size() == 0){
            for (size_t i = 0; i < data.size(); ++i){
                vector<double> featureValueList;
                featureValueList.push_back(data[i]);
                m_featureValues_.push_back(featureValueList);
            }
        } else {
            for (size_t i = 0; i < data.size(); ++i){
                vector<double>& featureValueList = m_featureValues_[i];
                featureValueList.push_back(data[i]);
            }
        }
    }

    // Destroys the entire forest of trees.
    void Forest::destroy()
    {
        vector<NodePtr>::iterator iter = m_trees_.begin();
        while (iter != m_trees_.end())
        {
            NodePtr tree = (*iter);
            if (tree)
            {
                delete tree;
            }
            ++iter;
        }
        m_trees_.clear();
    }

    // Frees the custom randomizer object (if any).
    void Forest::destroy_randomizer()
    {
        if (m_randomizer_)
        {
            delete m_randomizer_;
            m_randomizer_ = NULL;
        }
    }

    // Returns the forest as a JSON object.
    string Forest::Dump() const
    {
        string data = "{";
        size_t treeIndex = 0;

        vector<NodePtr>::const_iterator iter = m_trees_.begin();
        while (iter != m_trees_.end())
        {
            NodePtr tree = (*iter);
            string treeData = "'Tree ";

            treeData.append(to_string(treeIndex));
            treeData.append("': ");
            treeData.append(tree->Dump());
            ++iter;
            if (iter != m_trees_.end())
                treeData.append(", ");
            ++treeIndex;
            data.append(treeData);
        }
        data.append("}");
        return data;
    }

    void Forest::update_setting(bool new_normalized_kept_points)
    {
        normalized_kept_points_ = new_normalized_kept_points;
    }

    string Forest::save_state()
    {
        vector<vector<vector<double>>> state;
        vector<double> settings;

        settings.push_back((double)max_stored_data_points_);
        settings.push_back((double)points_to_reconstruct_);
        settings.push_back((double)point_count_);
        settings.push_back((double)dSize);
        settings.push_back((double)dNum);
        settings.push_back(normalized_kept_points_? 1.0 : 0.0);
        vector<vector<double>> temp(1, settings);
        
        state.push_back(temp);
        state.push_back(m_featureValues_);
        state.push_back(m_trainData_);

        stateHelper helper;
        return helper.matrices_to_string(state);
    }

}