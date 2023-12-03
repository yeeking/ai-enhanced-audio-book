/*
  ==============================================================================

    MarkovManager.h
    Created: 30 Oct 2019 3:28:02pm
    Author:  matthew

  ==============================================================================
*/

#pragma once
#include "MarkovChain.h"
#include <mutex>


/**
 * Manages a markov chain for training and generation purposes
 */
class MarkovManager {
  public:
  /**
   * Create a markov manager. chainEventMemoryLength is how many chain events we 
   * remember. Chain events are remembered so we can delete or amplify parts of the chain
   * using givePositive and giveNegative feedback. 
   */
      MarkovManager(unsigned long maxOrder=100, unsigned long chainEventMemoryLength=20);
      ~MarkovManager();
      /** add an event to the chain. The manager manages previous events to ensure 
       * that variable orders are passed to the underlying markov model
      */
      void putEvent(state_single symbol);
      /**
      * retrieve an event from the underlying markov model. 
      * @param needChoices: if true, requires that the underlying model only selects states which have at least two observations for them
      */
      state_single getEvent(bool needChoices = true);
      /**
       * returns the order of the model that generated the last event 
       * calls 
       */
      int getOrderOfLastEvent();
      /**
       * wipe the underlying model and reset short term input and output memory. 
       */
      void reset();

      /**
       * Rotates the sent seq and pops the sent item on the end
       * [1,2,3], 4 -> [2,3,4]
       */
      void addStateToStateSequence(state_sequence& seq, state_single new_state);
      /**
       * Update the chain by removing recently visited parts 
       */
      void giveNegativeFeedback();
      /**
       * Update the chain by amplifying recently visited parts 
       */
      void givePositiveFeedback();
       
      /**
       * convenience function to save the model to the sent file. uses model.toString to first
       * convert it to a file
       */
      bool saveModel(const std::string& filename);
       /**
       * convenience function to  load the model to the sent file. uses model.fromString to first
       * convert it to a file
       */
      bool loadModel(const std::string& filename);

      /** returns a string representation of the model suitable for saving
       * in case you don't want to use saveModel directly
      */
      std::string getModelAsString();
      /** tries to convert the sent string into a model by calling model.fromString */
      bool setupModelFromString(std::string);


      /** returns a copy of the model */
      MarkovChain getCopyOfModel();
  private:
      void rememberChainEvent(state_and_observation event);
      
      state_sequence inputMemory;
      state_sequence outputMemory;
      MarkovChain chain;
      std::vector<state_and_observation> chainEvents;
      unsigned long  maxChainEventMemory;
      unsigned long  chainEventIndex;
      bool locked;
      std::mutex mtx;
};

