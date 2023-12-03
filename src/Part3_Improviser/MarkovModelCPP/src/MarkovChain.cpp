/*
  ==============================================================================

    MarkovChain.cpp
    Created: 25 Oct 2019 6:47:13am
    Author:  matthew

  ==============================================================================
*/

#include "MarkovChain.h"
#include <iostream>
#include <ctime>

MarkovChain::MarkovChain(unsigned long  _maxOrder) : maxOrder{_maxOrder}, orderOfLastMatch{0}
{
  srand((int)time(NULL));
}

MarkovChain::~MarkovChain()
{

}

void MarkovChain::addObservation(const state_sequence& prevState, state_single currentState)
{
  if (currentState == "0")
  {
    //std::cout << "MarkovChain::addObservation received invalid state. Ignoring it " << currentState << std::endl;
    //throw "MarkovChain::addObservation observation 0 is reserved";
  }
  // convert the previous state to a CSV style key
  if (!validateStateSequence(prevState)) 
  {
    //std::cout << "MarkovChain::addObservation invalid prev state " << std::endl;
    return; 
  }
  state_single key = stateSequenceToString(prevState);
  // do we have this key already
  bool have_key = true;
  try
  {
    model.at(key);
  }
  catch (const std::out_of_range& oor)
  {
    have_key = false;
  }
  // if we have this key, we are adding the new obs on that key
  if (have_key)
  {
    model[key].push_back(currentState);
  }
  else{// new key - create a new next state sequence
    state_sequence seq = {currentState};
    model[key] = seq;
  }
}

void MarkovChain::addObservationAllOrders(const state_sequence& prevState, state_single currentState)
{
  std::vector<state_sequence> allPrevs = breakStateIntoAllOrders(prevState);
  for (state_sequence& seq : allPrevs)
  {
    //std::cout << "MarkovChain::addObservationAllOrders adding obs for " << this->stateSequenceToString(seq) << " to " << currentState <<  std::endl; 
    addObservation(seq, currentState);
  } 
}

std::vector<state_sequence>  MarkovChain::breakStateIntoAllOrders(const state_sequence& prevState)
{
  std::vector<state_sequence> allPrevs;
  // start is in the range 0-prevState.size() - 1
  long unsigned int end = prevState.size();
  allPrevs.push_back(prevState);
  for (unsigned long int start = 1; start < end; ++start)
  {
    state_sequence::const_iterator first = prevState.begin() + start;
    state_sequence::const_iterator last = prevState.begin() + prevState.size();
    state_sequence prevStateShort(first, last);
    allPrevs.push_back(prevStateShort);
  }
  return allPrevs;
}


std::string MarkovChain::stateSequenceToString(const state_sequence& sequence)
{
  std::string str = std::to_string(sequence.size()); // write the order first
  str.append(",");
  for (const state_single& s : sequence)
  {
      str.append(s);
      str.append(",");  
  } 
  return str;
}
std::string MarkovChain::stateSequenceToString(const state_sequence& sequence, long unsigned int maxOrder)
{
  if (maxOrder >= sequence.size()){ 
    // max order is higher pr == than the order we have
    // simply ignore it and return the highest order we can
    return stateSequenceToString(sequence);
  }
  else {// requested maxOrdder is lower than the available order in the incoming state seq.
    std::string str = std::to_string(maxOrder); // write the order first
    str.append(",");
    long unsigned int want_to_skip = sequence.size() - maxOrder;
    long unsigned int skipped = 0;
    // prefix it with the order
    for (const state_single& s : sequence)
    {
     if (skipped < want_to_skip) 
     {
        skipped ++;
        continue; 
     } 
      str.append(s);
      str.append(",");
    } 
    return str;
  }
}

state_single MarkovChain::generateObservation(const state_sequence& prevState, int maxOrderWanted, bool needChoice)
{
  // check for empty model
  if (model.size() == 0)
  {
    //std::cout << "warning - requested obs from empty model " << std::endl;
    return "0";
  }
  // don't allow orders beyond our own maxOrder
  if (maxOrderWanted > this->maxOrder) maxOrderWanted = this->maxOrder;
  // attempt to find a key in the chain that matches the incoming prevState
  state_single key = stateSequenceToString(prevState, maxOrderWanted);
  state_sequence poss_next_states{};
  bool have_key = true;
  // bad boy using exception handling for control flow. live on the edge! 
  try
  {
    poss_next_states = model.at(key);
    //std::cout << "MarkovChain::generateObservation want choice " << needChoice << "  got choices" << poss_next_states.size() << std::endl;
    
    // now if the caller demanded choices, we need to check there are choices
    if (needChoice && poss_next_states.size() < 2) { // want choice, none there
      //std::cout << "MarkovChain::generateObservation want choice but only " << poss_next_states.size() << " going lower order than " << maxOrderWanted << std::endl;
      have_key = false; 
    }

  }
  catch (const std::out_of_range& oor)
  {
    have_key = false;
  }
  if (have_key)
  {
    // get a random choice from the available ones 
    //std::cout << "MarkovChain::generateObservation key: '" << key << "' Got possible next states " << poss_next_states.size() << std::endl;
    state_single obs = pickRandomObservation(poss_next_states);
    // remember what we did
    this->orderOfLastMatch = maxOrderWanted; 
    this->lastMatch = state_and_observation{key, obs};
    return obs; 
  }
  else {
    //std::cout << "MarkovChain::generateObservation no match for that key " << key << "  at order " << maxOrderWanted << std::endl;
    if (maxOrderWanted > 1) 
    {
      //std::cout << "no match, reducing order to " << maxOrder - 1 << std::endl;
      // recurse with lower max order
      return generateObservation(prevState, maxOrderWanted-1, needChoice);
    }
    else {
      // worst case - nothing at higher than zero order
      this->orderOfLastMatch = 0;
      //std::cout << "MarkovChain::generateObservation no match for " << key << " doing zero order " << std::endl;
      state_single obs = zeroOrderSample();
      this->lastMatch = state_and_observation{"0", obs};
      return obs; 
    }
  }
}

state_single MarkovChain::zeroOrderSample()
{

  state_sequence poss_next_states{};
  // no key - choose something at random from all next observed states
  int randInd = 0;
  if (model.size() > 1) randInd = rand() % model.size();
  //std::cout << "MarkovChain::zeroOrderSample rand " << randInd << " from " << model.size() << std::endl; 
  int ind = 0;
  state_single state = "0"; // start on the default state
  // iterate the map until we teach our random index
  // have to do this as skips are not possible
  for (std::map<std::string,state_sequence>::iterator it=model.begin();it!=model.end(); ++it)
  {
    if (ind == randInd){
      poss_next_states = (it->second);
      state = pickRandomObservation(poss_next_states);
      break;// jump down to the return statement 
    }
    else {
      ind ++;
      continue;
    }
  }
  return state;
}


state_single MarkovChain::pickRandomObservation(const state_sequence& seq)
{
  if (seq.size() == 0) // they key existed but there';s nothing there.
  {
    return "0";
  } 
  auto ind = 0;
  if (seq.size() > 1) ind = rand() % seq.size();  
  return seq.at(ind);
  //return "0";
}

std::string MarkovChain::toString()
{
  //std::cout << "MarkovChain::toString model size " << model.size() << std::endl;
  std::string s{""};
  for(auto const& kv_pair: model){
    s += kv_pair.first + ":";
    s += this->stateSequenceToString(model[kv_pair.first]);
    s += "\n";
  }
  return s;
}

bool MarkovChain::validateStateToObservationsString(const std::string& data)
{
//    * super basic: minimal string is '1,a:2,b'-> length >= 7  
  if (data.size() < 7) {
    std::cout << "MarkovChain::validateDataString too short " << std::endl;  
    return false; 
  }
//  * does it have a colon?
  if (data.find_first_of(':') == std::string::npos) {
    std::cout << "MarkovChain::validateDataString no colon " << std::endl;  
    return false; 
  }
//  * does it have at least two commas? 
  auto found = data.find_first_of(',');
  int count = 0;
  while (found!=std::string::npos)
  {
    count ++;
    if (count > 1) break;
    found=data.find_first_of(',',found+1);
  }
  if (count < 2){
    std::cout << "MarkovChain::validateDataString need two commas" << std::endl;  
    return false; 
  }
  return true;

}
bool MarkovChain::fromString(const std::string& savedModel)
{
  //unsigned long int startSize = model.size();
  // example
  // 3,one,two,three,:1,four,five,\n
  // 2,two,three,:1,four,\n
  // -> order,state,:order,observation 1,observation n
  // -> convert 'state' into a vector of strings (state_sequence)
  // -> convert 'observation 1...n' to a vector of strings ["four", "five"]
  // algo:
  // split on lines '\n'
  // for each line
  // split on ':'
  // [0] is key
  // split [1] on ','
  // convert first element to int (it is the number of different observations)
  // convert the remaining elements to a string vector
  std::vector<std::string> lines = MarkovChain::tokenise(savedModel, '\n');
  for (const std::string& line : lines){
    //std::cout << "MarkovChain::fromString processing line " << line << std::endl; 

    // skip invalid lines
    if (! MarkovChain::validateStateToObservationsString(line)) continue; 
    //std::cout << "MarkovChain::fromString line valid. tokenising on ':'" << line << std::endl; 

    std::vector<std::string> k_v = MarkovChain::tokenise(line, ':');
    //std::cout << "MarkovChain::fromString tokenised line to " << k_v[0] << " and " << k_v[1] << " getting prev state "<< std::endl; 
    state_sequence prevState = MarkovChain::tokenise(k_v[0], ',');
    // maybe remove unwanted elements from prevState here...
    // ... here... 
    state_sequence prevStateFilt{};
    // check the first element is a number
    if (prevState.size() == 1) continue; // should have a number then the prev states so len at least 2
    //std::cout << "MarkovChain::fromString building prev state. size is " << prevState.size() << std::endl; 

    for (unsigned long i=1;i<prevState.size();++i){
      prevStateFilt.push_back(prevState[i]);
    }
    state_sequence all_obs = MarkovChain::tokenise(k_v[1], ','); // all observations following that state
    if (all_obs.size() == 1) continue; // should have a number then the actual states so len at least 2
    for (unsigned long i=1;i<all_obs.size();++i){ // 1 as first is no. different observations
      this->addObservation(prevStateFilt, all_obs[i]);
    }
  }
  // at this point, we hope something was loaded. if the file was invalid, meh
  return true;
  //if (model.size() > startSize ) return true;
  //else return false; 
}

void MarkovChain::reset()
{
    model.clear();
}

int MarkovChain::getOrderOfLastMatch()
{
  return this->orderOfLastMatch;
}

state_and_observation MarkovChain::getLastMatch()
{
  return this->lastMatch;
}

void  MarkovChain::removeMapping(state_single state_key, state_single unwanted_option)
{
  if (model.size() ==0 ) return; 
  state_sequence current_options{};
  bool have_key = true;
  try
  {
    current_options = model.at(state_key);
  }
  catch (const std::out_of_range& oor)
  {
    have_key = false; // nothing to do as we don't even have the state_key 
  }
  if (have_key) // we have seen this state_key
  {
    // create a new set of possible 
    state_sequence updated_options{};
    for (const state_single& obs : current_options)
    {
      if (obs != unwanted_option)
      {
        updated_options.push_back(obs);
      }
    }
    this->model[state_key] = updated_options;
  }
}

void MarkovChain::amplifyMapping(state_single state_key, state_single wanted_option)
{
  if (model.size() ==0 ) return; 
  state_sequence options = getOptionsForSequenceKey(state_key);
  if (options.size() == 0) // nothing mapped to this key... easy! 
  {
    options.push_back(wanted_option);
    this->model[state_key] = options; 
    return; 
  }
  // how many of the wanted option are there, relative to the total?
  float wanted = 0;
  float othermappings = 0;
  for (const state_single& s : options) {
    if (s == wanted_option) wanted ++;
    else othermappings ++;
  }
  // basically match the number of othermappings
  // to make this mapping as likely as any other
  for (auto i=0;i<othermappings;i++) model[state_key].push_back(wanted_option);
}


state_sequence MarkovChain::getOptionsForSequenceKey(state_single seqAsKey)
{
  state_sequence options{};
  try
  {
    options = model.at(seqAsKey);
  }
  catch (const std::out_of_range& oor)
  {
    // that's ok...we caught it :) 
  }
  return options; 
}


std::vector<std::string> MarkovChain::tokenise(const std::string& input, char separator)
{
   std::vector<std::string> tokens;
   long unsigned int start, end;
   std::string token;
    start = input.find_first_not_of(separator, 0);
    do{
      end = input.find_first_of(separator, start);
      if (start == input.length() || start == end) break;
      if (end >= 0) token = input.substr(start, end - start);
      else token = input.substr(start, input.length() - start);
      tokens.push_back(token);
    start = end + 1;
    //}while(end > 0);
    // at some point it needed to be npos
    }while(end != std::string::npos);

   return tokens; 
}

long MarkovChain::size()
{
  return model.size();
}

bool MarkovChain::validateStateSequence(const state_sequence& seq)
{
  if (seq.size() == 0) return false; 
  for (const state_single& s : seq)
  {
    if (s == "0") // blank state - this state sequence is not useable 
      return false;
  } 
  return true;
  
}


