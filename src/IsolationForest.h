//  MIT License
//
//  Copyright © 2017 Michael J Simms. All rights reserved.
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in all
//  copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//  SOFTWARE.

#pragma once

#include <map>
#include <random>
#include <set>
#include <stdint.h>
#include <string>
#include <time.h>
#include <vector>
#include "model.h"

using namespace std;

namespace IsolationForest
{
    
    /// Tree node, used internally.
    class Node
    {
    public:
        Node();
        Node(size_t index, double splitValue, int count);
        virtual ~Node();

        virtual size_t Index() const { return m_index; };
        virtual double SplitValue() const { return m_splitValue; };

        Node* Left() const { return m_left; };
        Node* Right() const { return m_right; };

        void SetLeftSubTree(Node* subtree);
        void SetRightSubTree(Node* subtree);
        int getCount();

        std::string Dump() const;

    private:
        size_t m_index;
        double m_splitValue;
        int m_count = 0;

        Node* m_left;
        Node* m_right;

        void DestroyLeftSubtree();
        void DestroyRightSubtree();
    };

    typedef Node* NodePtr;
    typedef std::vector<NodePtr> NodePtrList;

    /// This class abstracts the random number generation.
    /// Inherit from this class if you wish to provide your own randomizer.
    /// Use Forest::SetRandomizer to override the default randomizer with one of your choosing.
    class Randomizer
    {
    public:
        Randomizer() : m_gen(m_rand()) {} ;
        virtual ~Randomizer() { };

        virtual uint64_t Rand() { return m_dist(m_gen); };
        virtual uint64_t RandUInt64(uint64_t min, uint64_t max) { return min + (Rand() % (max - min + 1)); }

    private:
        std::random_device m_rand;
        std::mt19937_64 m_gen;
        std::uniform_int_distribution<uint64_t> m_dist;
    };

    /// Isolation Forest implementation.
    class Forest : public model
    {
    public:
        Forest(vector<vector<double>> initial_input, int max_stored_data_points, int points_to_reconstruct);
        Forest(uint32_t numTrees, uint32_t subSamplingSize);

        virtual ~Forest();

        void SetRandomizer(Randomizer* newRandomizer);
        NodePtr CreateTree(const vector<vector<double>>& featureValues, const vector<vector<double>>& trainData, size_t depth);
        void Create();

        double Score(vector<double> data);
        double NormalizedScore(vector<double> data);

        string Dump() const;

        double process_input(vector<double> input_data);
        void process_feedback(vector<double> input_data, bool isAnomaly);
        void updateSetting(bool new_normalized_kept_points);

    private:
        Randomizer* m_randomizer; // Performs random number generation
        vector<vector<double>> m_featureValues; // Lists each feature and maps it to all unique values in the training set
        // m_featureValues is the transpose of the original
        vector<vector<double>> m_trainData;
        NodePtrList m_trees; // The decision trees that comprise the forest
        uint32_t m_numTreesToCreate; // The maximum number of trees to create
        uint32_t m_subSamplingSize; // The maximum depth of a tree

        int max_stored_data_points_; // TODO comment
        int points_to_reconstruct_;
        int point_count_ = 0;
        int dSize = 0, dNum = 0;

        bool normalized_kept_points;


        NodePtr CreateTree(const vector<vector<double>>& featureValues, size_t depth);
        double Score(vector<double>& data, const NodePtr tree);
        void push_data(vector<double>& data);
        void push_transpose_data(vector<double>& data);

        void Destroy();
        void DestroyRandomizer();
    };
};