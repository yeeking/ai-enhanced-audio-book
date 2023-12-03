#include <vector> 
#include <iostream>

class ChordDetector{
    public:
        /**
         * @brief Construct a new Chord Detector object
         * 
         * @param maxInterval: if notes are further apart than this, they are put in separate chords
         */
        ChordDetector(unsigned long _maxInterval);
        /**
         * @brief Add a note to the detector. if the note is > maxInterval after the previously added note, it prepares a chord
         * which you can access via getChord
         * 
         * @param note 
         * @param timeMs 
         */
        void addNote(int note, unsigned long time);
        /**
         * @brief Does it have a chord ready? 
         * 
         * @return true if last two notes are at least maxInterval apart 
         * @return false if last two notes are less than maxInterval or not enough notes yet (0 or 1)
         */
        bool hasChord() const;
        /**
         * @brief get the last detected chord. every time a new chord comes in, it wipes the old one
         * 
         * @return std::vector<int> which might be empty if 'hasChord == false
         */
        std::vector<int> getChord() const;
    private:
        struct NoteInfo {int note; 
                         unsigned long time;
                         };
        std::vector<NoteInfo> notes; 
        std::vector<int> lastChord; 
        unsigned long maxInterval;

};