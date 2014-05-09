//
//  project1.cpp
//  CSE 100 Extra Credit Project 1
//
//  Created by Nathaniel Lewis, Matthew Bucci, Zachary Canann on 4/28/14.
//  Copyright (c) 2014 HoodooNet. All rights reserved.
//

#include <iostream>
#include <algorithm>
#include <set>
#include <cstdint>
#include <cassert>

#include "Utilities.h"

#include "HashTable.hpp"
#include "HashFunctions.hpp"
#include "ChainingHashTable.hpp"

// Hash table type
typedef HashTable<int64_t, hash<int64_t> > MyHashTable;

// Generate the keys for the experiment
void GenerateKeys(size_t m, size_t n, std::set<int64_t>& S, std::set<int64_t>& D, std::set<int64_t>& U)
{
    // Clear the provided sets
    S.clear();
    D.clear();
    U.clear();
    
    // Generate the in hash table set
    while(S.size() < (n / 2))
    {
        // Generate a key
        int64_t key = lrand() % (m * 10000);
        S.insert(key);
    }
    
    // Generate the to delete set
    while(D.size() < (n / 2))
    {
        // Generate a key
        int64_t key = lrand() % (m * 10000);
        
        // if this key does not exist in either array
        if(S.find(key) == S.end())
        {
            D.insert(key);
        }
    }
    
    // Generate the not in table set
    while(U.size() < (n / 2))
    {
        // Generate a key
        int64_t key = (lrand() % (m * 10000)) + (m * 10000);
        U.insert(key);
    }
}

class Result
{
	struct timeval startTime, stopTime;
	size_t numberOfElements;

public:
	double duration;

	void start(size_t numberOfElements)
	{
		this->numberOfElements = numberOfElements;
		gettimeofday(&startTime, NULL);
	}

	void stop()
	{
		gettimeofday(&stopTime, NULL);
		duration = (stopTime.tv_sec - startTime.tv_sec) + (stopTime.tv_usec - startTime.tv_usec) / 1000000.0;
		duration /= (double)numberOfElements;
	}
	
	double operator/(const Result& r)
	{
		return this->duration / r.duration;
	}
	
};

std::ostream& operator<<(std::ostream& os, const Result& r)
{
	os << r.duration;
	return os;
}

struct ExperimentResult
{
	double minTime, maxTime;
	std::string minType, maxType;
	Result tS, tU, ptS, ptU;
	
	void calcMin()
	{
		minTime = tS.duration;
		minType = "tS";

		if (tU.duration < minTime)
		{
			minTime = tU.duration;
			minType = "tU";
		}

		if (ptS.duration < minTime)
		{
			minTime = ptS.duration;
			minType = "ptS";
		}

		if (ptU.duration < minTime)
		{
			minTime = ptU.duration;
			minType = "ptU";
		}
	}

	void calcMax()
	{
		maxTime = tS.duration;
		maxType = "tS";

		if (tU.duration > maxTime)
		{
			maxTime = tU.duration;
			maxType = "tU";
		}

		if (ptS.duration > maxTime)
		{
			maxTime = ptS.duration;
			maxType = "ptS";
		}

		if (ptU.duration > maxTime)
		{
			maxTime = ptU.duration;
			maxType = "ptU";
		}
	}

	void print()
	{
		calcMin();
		calcMax();

		// Results
		std::cout << "tS = " << tS << ", tU = " << tU << std::endl;
		std::cout << "tU / tS = " << tU / tS << std::endl;
		std::cout << "tS' = " << ptS << ", tU' = " << ptU << std::endl;
		std::cout << "tU' / tS' = " << ptU / ptS << std::endl;

		std::cout << "Min is " << minType.c_str() << " with time:\t" << minTime << std::endl;
		std::cout << "Max is " << maxType.c_str() << " with time:\t" << maxTime << std::endl;
	}
};

// Run the experiment
void RunExperiment(size_t m, size_t n)
{
    // Datasets for the experiment
    std::set<int64_t> S;
    std::set<int64_t> D;
    std::set<int64_t> U;

	ExperimentResult expResult;
    
    
    // Make shit
    GenerateKeys(m, n, S, D, U);
    
    
    // Create a hash table w/ open indexing for this lab
    MyHashTable t(m);
    //ChainingHashTable<int64_t, chash<int64_t> > ct(m);
    
    // Insert the elements from the S set
    for(std::set<int64_t>::iterator it = S.begin(); it != S.end(); it++)
    {
        assert(t.insert(*it));
    }
    
    
    // Insert the elements from the D set
    for(std::set<int64_t>::iterator it = D.begin(); it != D.end(); it++)
    {
        assert(t.insert(*it));
    }
    
    
    // Perform the initial experiment
    // Search for the elements in the S set
	expResult.tS.start(S.size());
    for(std::set<int64_t>::iterator it = S.begin(); it != S.end(); it++)
    {
        // Perform the search
        MyHashTable::search_result r = t.search(*it);
        assert(*r == *it);
    }
	expResult.tS.stop();
    
    
    // Search for the elements in the U set
	expResult.tU.start(U.size());
    for(std::set<int64_t>::iterator it = U.begin(); it != U.end(); it++)
    {
        // Perform the search
        MyHashTable::search_result r = t.search(*it);
        assert(r == t.NotFound());
    }
	expResult.tU.stop();
    
    
    // Get the clusters
    MyHashTable::cluster_type clusters;
    t.clusters(clusters);
    
    // Print the clusters
    size_t count = 0;
    for(MyHashTable::cluster_type::iterator cluster = clusters.begin(); cluster != clusters.end(); cluster++)
    {
        std::cout << "Cluster Size: " << cluster->first << "; Frequency: " << cluster->second << std::endl;
        count += cluster->first * cluster->second;
    }
    std::cout << "Total Results = " << count << std::endl << std::endl;
    
    
    // Remove the elements from the D set
    for(std::set<int64_t>::iterator it = D.begin(); it != D.end(); it++)
    {
        t.remove(*it);
    }
    
    
    // Perform the secondary experiment
    // Search for the elements in the S set
	expResult.ptS.start(S.size());
    for(std::set<int64_t>::iterator it = S.begin(); it != S.end(); it++)
    {
        // Perform the search
        MyHashTable::search_result r = t.search(*it);
        assert(*r == *it);
    }
	expResult.ptS.stop();
    
    
    // Search for the elements in the U set
	expResult.ptU.start(U.size());
    for(std::set<int64_t>::iterator it = U.begin(); it != U.end(); it++)
    {
        // Perform the search
        MyHashTable::search_result r = t.search(*it);
        assert(r == t.NotFound());
    }
	expResult.ptU.stop();
    
    // Get the clusters
    t.clusters(clusters);
    
    // Print the clusters
    count = 0;
    for(MyHashTable::cluster_type::iterator cluster = clusters.begin(); cluster != clusters.end(); cluster++)
    {
        std::cout << "Cluster Size: " << cluster->first << "; Frequency: " << cluster->second << std::endl;
        count += cluster->first * cluster->second;
    }
    std::cout << "Total Results = " << count << std::endl << std::endl;
}

// Main method
int main(int argc, const char * argv[])
{
    int tablesize = 1048576;
    int num_of_elements = 1000000;
    int testnumber = 0;
    
    /*do
    {
        do
        {
            //output the test number
            std::cout << "TEST " << testnumber << std::endl;
            
            //get the table size
            std::cout << "Enter The Tablesize: ";
            std::cin >> tablesize;
            
            //get the number of elements
            std::cout << "Enter The Number Of Elements: ";
            std::cin >> num_of_elements;
            
            //check for tards
            if(tablesize <= num_of_elements)
                std::cout << "Test Not Valid" << std::endl;
            
        } while(tablesize <= num_of_elements);*/
        
        // Seed the random generator with the current time
        srand(static_cast<unsigned int> (time(NULL)));
        
        // Time the experiment
        struct timeval start,end;
        gettimeofday(&start,NULL);
        
        // Run the experiment (tablesize = 1M elements, generate 10k elements)
        RunExperiment(tablesize, num_of_elements);
        
        // Print the runtime
        gettimeofday(&end,NULL);
        fprintf(stdout,"Experiment Runtime: %f\n\n",(end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/1000000.0);
        
        //testnumber++;
    //} while(tablesize != 0  && num_of_elements != 0);
    
    return 0;
}
