#include "ChordDetector.h"
#include <iostream>
#include <assert.h>

int main()
{
    ChordDetector cd {5};

    cd.addNote(25, 100);
    cd.addNote(25, 101);
    // 0.1 < 0.5 no chord
    assert(cd.hasChord() == false );
    // 5 > 0.1 - chord ready
    cd.addNote(25, 105);
    assert(cd.hasChord() == true );
    std::vector<int> chord = cd.getChord();
    assert(chord.size() == 2);
    // 5 > 0.1 - 'chord' with one note ready
    cd.addNote(25, 110);
    chord = cd.getChord();
    assert(chord.size() == 1);
    
    // now try putting in two chords and checking it wipes 
    // the first one
    cd.addNote(25, 105);
    cd.addNote(25, 106);
    
    cd.addNote(25, 120);
    // now should have a chord with 2 notes
    // but - don't ask for it
    cd.addNote(25, 121);
    cd.addNote(25, 122);
    cd.addNote(25, 123);
    cd.addNote(25, 140);
    // now should have a chord with 4 notes
    chord = cd.getChord();
    assert(chord.size() == 4);
    
    

    
    
    
    
}