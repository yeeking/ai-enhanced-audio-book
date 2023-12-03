#include "ChordDetector.h"


ChordDetector::ChordDetector(unsigned long _maxInterval)
: maxInterval{_maxInterval}
{

}

void ChordDetector::addNote(int note, unsigned long time)
{
    std::cout << "ChordDetector::addNote " << maxInterval << " n: " << note << " : " << time << std::endl;
   // check if we are ready to release a chord
   // case 1: no notes yet
   if (notes.size() == 0){
     std::cout << "no notes" << std::endl;
    notes.push_back({note, time});
    return; 
   }

   unsigned long timeDiff = time - notes.back().time;

   // case 2: have notes, ready for chord
   // check distance from previous note
   if (timeDiff > maxInterval) {// chord time
    // convert stored notes into a vector and store it
    lastChord.clear();
    for (const NoteInfo& info : notes){
        lastChord.push_back(info.note);
    }
    // now add a fresh note
    notes.clear();
    notes.push_back({note, time});
    return; 
   }
   // case 3: have notes, not ready for chord
   if (timeDiff < maxInterval){
    // just add it to the notes
    notes.push_back({note, time});
   }

}

bool ChordDetector::hasChord() const 
{
   if (lastChord.size() == 0){
    return false; 
   }
   return true; 
}

std::vector<int> ChordDetector::getChord() const 
{
    return lastChord; 
}