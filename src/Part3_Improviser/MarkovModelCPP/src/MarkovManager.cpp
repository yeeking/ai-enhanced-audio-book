/*
  ==============================================================================

    MarkovManager.cpp
    Created: 30 Oct 2019 3:28:02pm
    Author:  matthew

  ==============================================================================
*/

#include "MarkovManager.h"
#include <iostream>
#include <fstream>
#include <sstream>

MarkovManager::MarkovManager(unsigned long maxOrder, unsigned long chainEventMemoryLength) 
  : maxChainEventMemory{chainEventMemoryLength}, 
  chainEventIndex{0}, 
  locked{false}
{
  inputMemory.assign(maxOrder, "0");
  outputMemory.assign(maxOrder, "0");
  
}
MarkovManager::~MarkovManager()
{
  
}
void MarkovManager::reset()
{
  mtx.lock();  
  inputMemory.assign(inputMemory.size(), "0");
  outputMemory.assign(outputMemory.size(), "0");
  chain.reset();
  mtx.unlock();
}
void MarkovManager::putEvent(state_single event)
{
  mtx.lock();
  try{
  // add the observation to the markov 
  // note that when we are boostrapping, i.e. filling up the input memory
  // we should not pass states in that include the "0"
  chain.addObservationAllOrders(inputMemory, event);
  // update the input memory
  addStateToStateSequence(inputMemory, event);
  }catch(...){// put this here as my JUCE thing crashes due to lack of thread-safeness
    std::cout << "MarkovManager::putEvent crashed... catching" << std::endl;
  }  
  mtx.unlock();
}
state_single MarkovManager::getEvent(bool needChoices)
{
  mtx.lock();
  state_single event{""};

  try{
    // get an observation
    event = chain.generateObservation(outputMemory, outputMemory.size(), needChoices);
    // check the output
    // update the outputMemory
    addStateToStateSequence(outputMemory, event);
    // store the event in case we want to provide negative or positive feedback to the chain
    // later
    rememberChainEvent(chain.getLastMatch());
  }catch(...){// put this here as my JUCE thing crashes due to lack of thread-safeness
    std::cout << "MarkovManager::getEvent crashed... catching" << std::endl;
    event = "0";
  }
  mtx.unlock();
  return event;
}

void MarkovManager::addStateToStateSequence(state_sequence& seq, state_single new_state){
  // shift everything across
  for (long unsigned int i=1;i<seq.size();i++)
  {
    seq[i-1] = seq[i];
  }
  // replace the final state with the new one
  seq[seq.size()-1] = new_state;
}

int MarkovManager::getOrderOfLastEvent()
{
  return chain.getOrderOfLastMatch();
}


void MarkovManager::rememberChainEvent(state_and_observation sObs)
{
  // the memory of chain events is not full yet
  if (chainEvents.size() < maxChainEventMemory)
  {
    chainEvents.push_back(sObs);
  }
  else 
  {
    // the memory of chain events is full - do FIFO
    chainEvents[chainEventIndex] = sObs;
    chainEventIndex = (chainEventIndex + 1) % maxChainEventMemory;
  }
}

void MarkovManager::giveNegativeFeedback()
{
  // remove all recently used mappings
  for (state_and_observation& so : chainEvents)
  {
    chain.removeMapping(so.first, so.second);
  }
}


void MarkovManager::givePositiveFeedback()
{
  // amplify all recently used mappings
  for (state_and_observation& so : chainEvents)
  {
    chain.amplifyMapping(so.first, so.second);
  }
}

bool MarkovManager::loadModel(const std::string& filename)
{
  if (std::ifstream in {filename})
  {
    std::ostringstream sstr{};
    sstr << in.rdbuf();
    std::string data = sstr.str();
    in.close();
    return chain.fromString(data);
  }
  else {
    return false; 
  }
}

bool MarkovManager::saveModel(const std::string& filename)
{
    if (std::ofstream ofs{filename}){
      ofs << chain.toString();
      ofs.close();
      return true; 
    }
    else {
      std::cout << "MarkovManager::saveModel failed to save to file " << filename << std::endl;
      return false; 
    }
}

std::string MarkovManager::getModelAsString()
{
  return chain.toString();
}

bool MarkovManager::setupModelFromString(std::string modelData)
{
  return chain.fromString(modelData);
}

MarkovChain MarkovManager::getCopyOfModel()
{
  return chain;
}
