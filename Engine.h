#ifndef ENGINE_H
#define ENGINE_H

#include <iostream>   
#include <vector>     
#include "BST.h"      
#include "Record.h"
//add header files as needed

using namespace std;

// Converts a string to lowercase (used for case-insensitive searches)
static inline string toLower(string s) {
    for (char &c : s) c = (char)tolower((unsigned char)c);
    return s;
}

// ================== Index Engine ==================
// Acts like a small "database engine" that manages records and two BST indexes:
// 1) idIndex: maps student_id → record index (unique key)
// 2) lastIndex: maps lowercase(last_name) → list of record indices (non-unique key)
struct Engine {
    vector<Record> heap;                  // the main data store (simulates a heap file)
    BST<int, int> idIndex;                // index by student ID
    BST<string, vector<int>> lastIndex;   // index by last name (can have duplicates)

    // Inserts a new record and updates both indexes.
    // Returns the record ID (RID) in the heap.
    int insertRecord(const Record &recIn) {
        //TODO
        int insertPosition = heap.size(); // get the position for the record
        heap.push_back(recIn); // add the record to the heap
        idIndex.insert(recIn.id, insertPosition); // update the id index
        string lowerLastName = toLower(recIn.last); // update the last name index
        vector<int>* existing = lastIndex.find(lowerLastName);
        if (existing == nullptr){ // if there is no lastname
            vector<int> newList; // create a new list for indexes
            newList.push_back(insertPosition); // add position
            lastIndex.insert(lowerLastName, newList); // add lastname and index into heap
        } else{
            existing->push_back(insertPosition);
        }
        return recIn.id;
    }

    // Deletes a record logically (marks as deleted and updates indexes)
    // Returns true if deletion succeeded.
    bool deleteById(int id) {
        //TODO
        int* pos = idIndex.find(id);
        if (pos == nullptr){
            return false;
        }
        if (*pos < 0 || *pos >= (int)heap.size() || heap[*pos].deleted){
            return false;
        }
        heap[*pos].deleted = true;
        return true;
    }

    // Finds a record by student ID.
    // Returns a pointer to the record, or nullptr if not found.
    // Outputs the number of comparisons made in the search.
    const Record *findById(int id, int &cmpOut) {
        //TODO
        idIndex.resetMetrics(); // reset count first
        int* pos = idIndex.find(id); // search heap position and save as pointer
        if (pos == nullptr){ //id not found
            cmpOut = idIndex.comparisons; // get comparison counts
            return nullptr;
        }
        if (*pos < 0 || *pos >= (int)heap.size() || heap[*pos].deleted){
            cmpOut = idIndex.comparisons;
            return nullptr;
        }
        cmpOut = idIndex.comparisons;
        return &heap[*pos]; //if everything is fine return record pointer
    }

    // Returns all records with ID in the range [lo, hi].
    // Also reports the number of key comparisons performed.
    vector<const Record *> rangeById(int lo, int hi, int &cmpOut) {
        //TODO
        idIndex.resetMetrics();
        vector<const Record*> out;
        idIndex.rangeApply(lo,hi, [&](const int &k, int &rid){
            if (rid >= 0 && rid < (int)heap.size() && !heap[rid].deleted)
                out.push_back(&heap[rid]);
        });
        cmpOut = idIndex.comparisons;
        return out;
    }

    // Returns all records whose last name begins with a given prefix.
    // Case-insensitive using lowercase comparison.
    vector<const Record *> prefixByLast(const string &prefix, int &cmpOut) {
        //TODO
        lastIndex.resetMetrics();
        vector<const Record*> out;

        string lo = toLower(prefix);
        string hi = lo;
        if (!hi.empty()){
            hi.back()++;
        }

        lastIndex.rangeApply(lo,hi, [&](const string &k, vector<int> &rids){
            for (int rid : rids){
                if (rid >= 0 && rid < (int)heap.size() && !heap[rid].deleted){
                    out.push_back(&heap[rid]);
                }
            }
        });
        cmpOut = lastIndex.comparisons;
        return out;
    }
};

#endif