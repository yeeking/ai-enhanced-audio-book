/*
  ==============================================================================

    MarkovChain.h
    Creat
    d: 25 Oct 2019 6:47:13am
    Author:  matthew

  ==============================================================================
*/
#include <string>
#include <map>
#include <vector>
#include <random>

#pragma once

typedef std::vector<std::string> state_sequence;
typedef std::string state_single;
typedef std::pair<state_single, state_single> state_and_observation;

/**
 * Represents a markov chain
 */
class MarkovChain {
  public:
    MarkovChain(unsigned long _maxOrder=100);
    ~MarkovChain();
    /** 
     * addObservation
     * add a single observation to the chain
     * @param prevState - the state preceeding the observation
     * @param currentState - the state observed
     * 
    */
    void addObservation(const state_sequence& prevState, state_single currentState);
    /**
     *  addObservationAllOrders
     * Add all orders of the sent observation to the chain
     * It breaks prevState into multiple orders of observation, 1-prevState.length 
     * then calls this->addObservation on each one 
     * @param prevState - the state preceeding the observation
     * @param currentState - the state observed
     */
    void addObservationAllOrders(const state_sequence& prevState, state_single currentState);

  // should be private once testing is complete... 
  // note to self - how to enable testing of private methods? 
  /**
   * breakStateIntoAllOrders
   * Convers the sent state_sequence (actually a vector of length n)
   * Into n new vectors, each one the size 1-n respectively
   * and containing elements indexed [n to 1] down to [n]
   * as we assume that prevState[n] was the most recent state
   * e.g. [a,b,c] -> [a,b,c], [b,c], [c]
   * @param state_sequence: the incoming vector that needs to be broken down
   * @return a vector of several_statess a
   */
    std::vector<state_sequence> breakStateIntoAllOrders(const state_sequence& prevState);
  
  /**
   * stateSequenceToString 
   * Converts a state_sequence into a string that can be used as a key
   * in the model. E.g.
   * ["a", "b", "thepther", "126"] -> "4,a,b,theother,127" (note it is prefixed with the length)
   * @param state_sequence: a vector of strings 
   */

    std::string stateSequenceToString(const state_sequence& sequence);
  /**
   * stateSequenceToString 
   * Converts a state_sequence into a string that can be used as a key
   * in the model. E.g.
   * sequence=["a", "b", "thepther", "126"], maxOrder=2 -> "2,theother,127" (note it is prefixed with the length)
   * Note that it takes the items from the end of the array, assuming they are the most recent states
   * @param state_sequence: a vector of strings 
   * @param max_order:an int representing which limits how much of the sequence we use 
   */

    std::string stateSequenceToString(const state_sequence& sequence, long unsigned int maxOrder);
    /**
     * generateObservation: generate a new observation from the chain. Tries to get highest possible 
     * order match to the incoming sequence by recursively testing sequences at length len(sequence) -> 1
     * 
     * Also remembers the final order it used into this->orderOfLastMatch
     * 
     * @param prevState - the lookup state used to query the model
     * @param maxOrder - how much of the previous state to consider. 
     * @param needChoice: set to true and it will always try and return from a lookup which yields at least two choices. This means it will often return at zero order when bootstrapping
     * It will try to query at this ordder, but if nothing is there, it'll
     * reduce the order until the query returns something.
     * @return a state sampled from the model
     */
    state_single generateObservation(const state_sequence& prevState, int maxOrderWanted, bool needChoice=false);
  /**
   * Picks a random observation from the sent sequence. 
   */
    state_single pickRandomObservation(const state_sequence& seq);
  /**
   * toString: convert the current model into a string for saving etc.
   * Example: 
  * 
  * MarkovChain m{};    m.addObservation(state_sequence{"one", "two", "three"}, "four");
  *
  * toString generates: 3,one,two,three,:1,four,\n"};
  * 
  * @return a string that can be sent to 'fromString' to recreate the model later
  */
    std::string toString();
    /**
     * fromString: recreate the model from the sent string
     * @param savedModel: the model we want
     * returns the result: false if it failed, true if it succeeded.
     */
    bool fromString(const std::string& savedModel);

    /** Yank the chain, as it were. 
     */
    void reset();
    /**return the order of the last match generated from generateObservation
     */
    int getOrderOfLastMatch();
    /**
     * pick a random observation from all observations
     */
    state_single zeroOrderSample();

    /**
     * returns the key-value that was used to 
     * generate the last observation via generateObservation
     */
    state_and_observation getLastMatch();

  /**
   * remove the mapping from the sent state key (derived from a state_sequence via stateSequenceToString) to the sent observation 
   * where state_key should be a key in this->map
   */
    void removeMapping(state_single state_key, state_single unwanted_option);
    
  /**
   * increase the chance of the sent mapping occuring by a certain amount 
   */
    void amplifyMapping(state_single state_key, state_single unwanted_option);
    
    /** return number of observations in the chain*/
    long size();

    /** checks if the sent state sequence is valid. i.e. does it contain blanks : "0" */
    bool validateStateSequence(const state_sequence& seq);

  /**
   * split the sent state string on the sent char separator 
   * returns a vector of strings. 
   * (here as it is needed by fromString)
   */
    static std::vector<std::string> tokenise(const std::string& s, char separator);

private:
/**
 * returns the available states that follow the sent key, where the sent key 
 * is derived from stateSequenceToString 
 */
    state_sequence getOptionsForSequenceKey(state_single seqAsKey);

/**
 * Checks if the sent string is suitable for parsing by fromString: 
 * super basic: minimal string is '1,a:2,b'-> length >= 7
 * does it have a colon?
 * does it have at least two commas? 
 */
static bool validateStateToObservationsString(const std::string& s);
/**
 * Maps from string keys to list of possible next states
 * 
 */
    std::map<state_single,state_sequence> model;
    unsigned long maxOrder; 
    unsigned long orderOfLastMatch;
    state_and_observation lastMatch;
};
