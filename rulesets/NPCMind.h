#ifndef NPC_MIND_H
#define NPC_MIND_H

typedef int bad_type; // Remove this to get unset type reporting

#define None 0 // Remove this to deal with un-initialied vars

class NPCMind : public BaseMind {
    Character * body;
    //Memory mem;
    
};

#endif /* NPC_MIND_H */
