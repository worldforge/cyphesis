#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Operation/Login.h>

#include "NPCMind.h"



// reverse_cmp={'>':'<'};

NPCMind::NPCMind(Character * body) : body(body)
{
    //NPCMind::knowledge=Knowledge();
    //NPCMind::mem=Memory(map=NPCMind::map);

// *** STOP * Am I quite sure that this is a necessary parent of External Mind
// Maybe this belongs in the python portion of the server?
// I will have to ask aloril about this. Maybe ExternalMind should inherit
// from BaseMind()
    things={};
    NPCMind::reverse_knowledge();
    NPCMind::goals=[];
    NPCMind::trigger_goals={};
    NPCMind::tick_count=0;
    NPCMind::message_queue=None;
    NPCMind::map.add_hooks.append(NPCMind::add_map);
    NPCMind::map.update_hooks.append(NPCMind::update_map);
    NPCMind::map.delete_hooks.append(NPCMind::delete_map);
}

bad_type NPCMind::add_map(bad_type obj)
{
    pass;
}

bad_type NPCMind::update_map(bad_type obj)
{
    foo_lst = NPCMind::things.get('Foo',[]);
    for (/*foo in foo_lst[:]*/) {
        ; //us copy in loop, because it might get modified;
        if (foo.id==obj.id) {
            NPCMind::remove_thing(foo);
            NPCMind::add_thing(obj);
        }
    }
}

bad_type NPCMind::delete_map(bad_type obj)
{
    NPCMind::remove_thing(obj);
}

bad_type NPCMind::setup_operation(bad_type op)
{
    return Operation("look")+Operation("tick",to=this);
}

bad_type NPCMind::tick_operation(bad_type op)
{
    NPCMind::tick_count=NPCMind::tick_count+1;
    opTick=Operation("tick",to=this);
    opTick.time.sadd=const.basic_tick;
    result=NPCMind::think();
    if (NPCMind::message_queue) {
        result = NPCMind::message_queue + result;
        NPCMind::message_queue = None;
    }
    return result+opTick;
}

bad_type NPCMind::sight_create_operation(bad_type original_op, bad_type op)
{
    obj=NPCMind::map.add(op[0]);
    if (op.from_==this) {
        NPCMind::add_thing(obj);
    }
}

bad_type NPCMind::sight_move_operation(bad_type original_op, bad_type op)
{
    obj=NPCMind::map.update(op[0]);
    if (obj.location.parent==this) {
        NPCMind::add_thing(obj);
    }
}

bad_type NPCMind::admin_sound(bad_type op)
{
    return op[0].from_.id==NPCMind::id;
}


bad_type NPCMind::interlinguish_warning(bad_type op, bad_type say, bad_type msg)
{
    log.debug(1,str(NPCMind::id)+" interlinguish_warning: "+str(msg)+\;
              ": "+str(say[0].lexlink.id[1:]),op);
}

bad_type NPCMind::interlinguish_desire_verb3_buy_verb1_operation(bad_type op, bad_type say)
{
    object=say[1].word;
    thing=NPCMind::things.get(object);
    if (thing) {
        thing=thing[0];
        NPCMind::remove_thing(thing);
        NPCMind::mode="selling";
        ent=Entity(thing.id,location=Location(op.from_,Vector3D(0,0,0)));
        return Message(Operation("move", ent,to=op.from_),Operation("set", Entity(NPCMind::id, mode=NPCMind::mode),to=this));
    }
}

bad_type NPCMind::interlinguish_desire_verb3_operation(bad_type op, bad_type say)
{
    object=say[2:];
    verb=interlinguish.get_verb(object);
    operation_method=NPCMind::find_operation(verb,"interlinguish_desire_verb3_",;
                                         NPCMind::interlinguish_undefined_operation);
    return operation_method(op, object);
}

bad_type NPCMind::interlinguish_be_verb1_operation(bad_type op, bad_type say)
{
    if (not NPCMind::admin_sound(op)) {
        return NPCMind::interlinguish_warning(op,say,"You are not admin");
    }
    res=interlinguish.match_importance(say);
    if (res) {
        return NPCMind::add_importance(res['sub'].id,'>',res['obj'].id);
    }
    else {
        return NPCMind::interlinguish_warning(op,say,"Unkown assertion");
    }
}

bad_type NPCMind::interlinguish_know_verb1_operation(bad_type op, bad_type say)
{
    if (not NPCMind::admin_sound(op)) {
        return NPCMind::interlinguish_warning(op,say,"You are not admin");
    }
    subject=say[1].word;
    object=say[2].word;
    if (object[0]=='(') {
        xyz=list(eval(object));
        loc=NPCMind::location.copy();
        loc.coordinates=Vector3D(xyz);
        NPCMind::add_knowledge("location",subject,loc);
    }
    else {
        NPCMind::add_knowledge("place",subject,object);
    }
}

bad_type NPCMind::interlinguish_learn_verb1_operation(bad_type op, bad_type say)
{
    if (not NPCMind::admin_sound(op)) {
        return NPCMind::interlinguish_warning(op,say,"You are not admin");
    }
    subject=say[1].word;
    object=say[2].word;
    NPCMind::add_goal(subject,object);
}

bad_type NPCMind::interlinguish_own_verb1_operation(bad_type op, bad_type say)
{
    if (not NPCMind::admin_sound(op)) {
        return NPCMind::interlinguish_warning(op,say,"You are not admin");
    }
    subject=NPCMind::map.get_add(say[1].word);
    object=NPCMind::map.get_add(say[2].word);
    if (subject==this) {
        NPCMind::add_thing(object);
    }
}

bad_type NPCMind::interlinguish_undefined_operation(bad_type op, bad_type say)
{
    log.debug(2,str(NPCMind::id)+" interlinguish_undefined_operation:",op);
    log.debug(2,str(say));
}

bad_type NPCMind::talk_undefined_operation(bad_type op, bad_type say)
{
    pass;
}

bad_type NPCMind::sound_talk_operation(bad_type original_op, bad_type op)
{
    talk_entity=op[0];
    if (interlinguish.convert_english_to_interlinguish(this,talk_entity)) {
        say=talk_entity.interlinguish;
        verb=interlinguish.get_verb(say);
        operation_method=NPCMind::find_operation(verb,"interlinguish_",;
                              NPCMind::interlinguish_undefined_operation);
    }
    else {
        operation_method=NPCMind::talk_undefined_operation;
        say=talk_entity;
        if (hasattr(say,"say")) {
            say=say.say;
        }
    }
    log.debug(3,"talk: "+str(operation_method));
    return operation_method(original_op,say);
}

bad_type NPCMind::call_triggers(bad_type op)
{
    event_name, sub_op = NPCMind::get_op_name_and_sub(op);
    reply = Message();
    for (/*goal in NPCMind::trigger_goals.get(event_name,[])*/) {
        reply = reply + goal.event(this, op, sub_op);
    }
    return reply;
}

bad_type NPCMind::reverse_knowledge()
{
    NPCMind::reverse_knowledge=Knowledge();
    for (/*(k,v) in NPCMind::knowledge.location.items()*/) {
        if (not NPCMind::reverse_knowledge.location.get(v)) {
            NPCMind::reverse_knowledge.add("location",v,k);
        }
    }
}

bad_type NPCMind::get_reverse_knowledge(bad_type what, bad_type key)
{
    d=getattr(NPCMind::reverse_knowledge,what);
    return d.get(key);
}

bad_type NPCMind::get_knowledge(bad_type what, bad_type key)
{
    d=getattr(NPCMind::knowledge,what);
    return d.get(key);
}

bad_type NPCMind::add_knowledge(bad_type what,bad_type key,bad_type value)
{
    NPCMind::knowledge.add(what,key,value);
    if (type(value)==InstanceType) {
        if (what=="goal") {
            thought_value = value.info();
        }
        else {
            thought_value = `value`;
        }
    }
    else {
        thought_value = value;
    }
    desc="%s knowledge about %s is %s" % (what,key,thought_value);
    ent = Entity(description=desc, what=what, key=key, value=thought_value);
    NPCMind::send(Operation("thought",ent));
    if (what=="location") {
        NPCMind::reverse_knowledge.add("location",value,key);
    }
}

bad_type NPCMind::add_importance(bad_type sub, bad_type cmp, bad_type obj)
{
    NPCMind::add_knowledge('importance',(sub,obj),cmp);
    NPCMind::add_knowledge('importance',(obj,sub),reverse_cmp[cmp]);
}

bad_type NPCMind::cmp_goal_importance(bad_type g1, bad_type g2)
{
    try {
        id1=g1.key[1];
        id2=g2.key[1];
    }
    catch (AttributeError) {
        return 1;
    }
    l1=ontology.get_isa(id1);
    l2=ontology.get_isa(id2);
    for (/*s1 in l1*/) {
        for (/*s2 in l2*/) {
            cmp=NPCMind::knowledge.importance.get((s1.id,s2.id));
            if (cmp) {
                return cmp=='>';
            }
        }
    }
    return 1;
}

def add_thing(self,thing): ;
    log.debug(3,str(self)+" "+str(thing)+" before add_thing: "+str(self.things));
    desc="I own %s." % thing.name;
    ent = Entity(description=desc, what=thing.as_entity());
    self.send(Operation("thought",ent));
    dictlist.add_value(self.things,thing.name,thing);
    log.debug(3,"\tafter: "+str(self.things));
bad_type NPCMind::find_thing(bad_type thing)
{
    if (StringType==type(thing)) {
        return NPCMind::things.get(thing,[]);
    }
    found=[];
    for (/*t in NPCMind::things.get(thing.name,[])*/) {
        if (t==thing) {
            found.append(t);
        }
    }
    return found;
}

bad_type NPCMind::remove_thing(bad_type thing)
{
    dictlist.remove_value(NPCMind::things, thing);
}

bad_type NPCMind::add_goal(bad_type name, bad_type str_goal)
{
    goal=eval("mind.goals."+str_goal);
    if (const.debug_thinking) {
        goal.debug=1;
    }
    goal.str=str_goal;
    if (type(name)==StringType) {
        goal.key=eval(name);
    }
    else {
    }
    NPCMind::add_knowledge("goal",name,goal);
    if (hasattr(goal,"trigger")) {
        dictlist.add_value(NPCMind::trigger_goals, goal.trigger(), goal);
        return;
    }
    for (/*i in range(len(NPCMind::goals)-1,-1,-1)*/) {
        if (NPCMind::cmp_goal_importance(NPCMind::goals[i],goal)) {
            NPCMind::goals.insert(i+1,goal);
            return;
        }
    }
    NPCMind::goals.insert(0,goal);
}

bad_type NPCMind::fulfill_goals(bad_type time)
{
    "see if all goals are fulfilled: if not try to fulfill them";
    for (/*g in NPCMind::goals[:]*/) {
        if (g.irrelevant) {
            NPCMind::goals.remove(g);
            continue;
        }
        res=g.check_goal(this,time);
        if (res!=None) {
            return res;
        }
    }
}

bad_type NPCMind::teach_children(bad_type child)
{
    res=Message();
    for (/*k in NPCMind::knowledge.location.keys()*/) {
        es=Entity(verb='know',subject=k,object=NPCMind::knowledge.location[k]);
        res.append(Operation('say',es,to=child));
    }
    for (/*k in NPCMind::knowledge.place.keys()*/) {
        es=Entity(verb='know',subject=k,object=NPCMind::knowledge.place[k]);
        res.append(Operation('say',es,to=child));
    }
    for (/*g in NPCMind::goals*/) {
        es=Entity(verb='learn',subject=g.key,object=g.str);
        res.append(Operation('say',es,to=child));
    }
    for (/*im in NPCMind::knowledge.importance.keys()*/) {
        cmp=NPCMind::knowledge.importance[im];
        if (cmp=='>') {
            s,i=il.importance(im[0],cmp,im[1]);
            es=Entity(say=s,interlinguish=i);
            res.append(Operation('say',es,to=child));
        }
    }
    return res;
}

bad_type NPCMind::think()
{
    if (const.debug_thinking) {
        log.thinking("think: "+str(this));
    }
    output=NPCMind::fulfill_goals(NPCMind::time);
    if (output and const.debug_thinking) {
        log.thinking(str(this)+" result at "+str(NPCMind::time)+": "+output[-1][0].description);
    }
    return output;
}

bad_type NPCMind::send(bad_type op)
{
    if (not NPCMind::message_queue) {
        NPCMind::message_queue=Message(op);
    }
    else {
        NPCMind::message_queue.append(op);
}
