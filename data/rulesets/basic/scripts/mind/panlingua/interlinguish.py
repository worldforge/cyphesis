# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 1999 Aloril (See the file COPYING for details).

import re

from mind.panlingua import panlingua

p = panlingua

# semnode ids from WordNet (english word+type+sense no)
# or it's made up (temporary until replaces with interlinguish sentence(s)
# describing goal or word (sense no=0)
word2node = {}


def add_word(words, index):
    name = str.replace(words, " ", "_")
    word2node[words] = (words, index)
    globals()[name] = (words, index)


add_word('help', '#help_verb1')
add_word('welcome', '#welcome_verb1')
add_word('breakfast', '#breakfast_verb1')
add_word('lunch', '#lunch_verb1')
add_word('sup', '#sup_verb1')
add_word('sleep', '#sleep_verb1')
add_word('eat', '#eat_verb2')
add_word('be', '#be_verb1')
add_word('important', '#important_adj1')
add_word('more', '#more_adv1')
add_word('than', '#than_prep0')
add_word('add_extinguish_fire', '#add_extinguish_fire_sentence0')  # CHEAT!
add_word('wander', '#wander_verb1')
add_word('forage', '#forage_verb1')
add_word('browse', '#browse_verb1')
add_word('avoid', '#avoid_verb1')
add_word('flock', '#flock_verb1')
add_word('hunt', '#hunt_verb1')
add_word('transport', '#transport_verb1')
add_word('assemble', '#assemble_verb1')
add_word('market', '#market_verb1')
add_word('peck', '#peck_verb1')

# I would like to buy an axe.
add_word('I', '#I_pron0')
# s/would like/desire/
add_word('desire', '#desire_verb3')
add_word('axe', '#axe_noun1')
add_word('pig', '#pig_noun1')
add_word('lumber', '#lumber_noun1')
add_word('ham', '#ham_noun1')
add_word('coin', '#coin_noun1')
add_word('deed', '#deed_noun1')
add_word('shirt', '#shirt_noun1')
add_word('cloak', '#cloak_noun1')
add_word('pants', '#pants_noun1')
add_word('services', '#services_noun1')
add_word('buy', '#buy_verb1')
add_word('hire', '#hire_verb1')
add_word('trade', '#trade_verb1')
add_word('keep', '#keep_verb1')
add_word('sell', '#sell_verb1')
add_word('patrol', '#patrol_verb1')
add_word('defend', '#defend_verb1')
add_word('herd', '#herd_verb1')
add_word('school', '#school_verb1')
add_word('amble', '#amble_verb1')
add_word('roam', '#roam_verb1')
add_word('vote', '#vote_verb1')
add_word('execute', '#execute_verb1')
add_word('shovel', '#shovel_noun1')
add_word('tinderbox', '#tinderbox_noun1')
add_word('pickaxe', '#pickaxe_noun1')
add_word('bowl', '#bowl_noun1')
add_word('pole', '#pole_noun1')
add_word('hammer', '#hammer_noun1')
add_word('bottle', '#bottle_noun1')
add_word('cleaver', '#cleaver_noun1')
add_word('trowel', '#trowel_noun1')
add_word('scythe', '#scythe_noun1')
add_word('bucksaw', '#bucksaw_noun1')

# CHEAT!: temporary thing
add_word('build home', '#build_home_verb0')
add_word('chop trees', '#chop_trees0')
add_word('create axes', '#create_axes0')
add_word('move lumber', '#move_lumber0')
add_word('buy livestock', '#buy_livestock0')


def importance(sub, cmp, obj):
    a1 = p.atom("is", 1, (be[1], "present tense"), (None, "declarative verb"))
    a2 = p.atom(sub[0], 2, (sub[1], "durative"), (a1, "verb subject"), "up")
    a3 = p.atom("important", 3, (important[1], "default"),
                (a2, "predicate adjective"), "left")
    a4 = p.atom("more", 4, (more[1], "default"),
                (a3, "adjective-modifying adverb"), "up")
    a5 = p.atom("than", 5, (than[1], "default"),
                (a4, "comparing preposition"), "left")
    a6 = p.atom(obj[0], 6, (obj[1], "durative"), (a5, "verb object"), "up")
    return (str.capitalize(sub[0]) + \
            "ing is more important than " + obj[0] + "ing.",
            (a1, a2, a3, a4, a5, a6))


def match_importance(say):
    # A is more important than B.
    a1 = p.atom("", 1)
    a2 = p.atom("", 2, ("?sub", "durative"), (a1, "verb subject"), "up")
    a3 = p.atom("important", 3, ('#important_adj1', "default"),
                (a2, "predicate adjective"), "left")
    a4 = p.atom("more", 4, ('#more_adv1', "default"),
                (a3, "adjective-modifying adverb"), "up")
    a5 = p.atom("than", 5, ('#than_prep0', "default"),
                (a4, "comparing preposition"), "left")
    a6 = p.atom("", 6, ("?obj", "durative"), (a5, "verb object"), "up")
    return p.match_sentence(say, [a1, a2, a3, a4, a5, a6])


# >John loves Mary.
# >loves#1 sib(3), lexlink(present tense: ppt=yes)
# >        synlink(declarative verb)
# >John#2 sib(1), lexlink(default)
# >        synlink(noun subject) up to loves#1
# >Mary#3 sib(1), lexlink(default)
#        synlink(noun object) left to John#2

vso_dict = {}


def add_vso_word(words, index):
    add_word(words, index)
    vso_dict[words] = (words, index)


add_vso_word('own', '#own_verb1')
add_vso_word('learn', '#learn_verb1')
add_vso_word('price', '#price_verb1')
add_vso_word('list', '#list_verb1')

# >This is for verbs like the above which require a predicate
# know pig price 5

vspo_dict = {}


def add_vspo_word(words, index):
    add_word(words, index)
    vspo_dict[words] = (words, index)


add_vspo_word('know', '#know_verb1')
add_vspo_word('tell', '#tell_verb1')


def verb_subject_object(verb, subject, object):
    """this function is partially cheat and not real translation"""
    a1 = p.atom(verb, 1, (word2node[verb][1], "present tense"), (None, "declarative verb"))
    a2 = p.atom(subject, 2, ("#subject", "default"), (a1, "subject"), "up")
    a3 = p.atom(object, 3, ("#object", "default"), (a2, "object"), "left")
    return (verb + " " + subject + " " + object,
            (a1, a2, a3))


def verb_subject_predicate_object(verb, subject, predicate, object):
    """this function is partially cheat and not real translation"""
    a1 = p.atom(verb, 1, (word2node[verb][1], "present tense"), (None, "declarative verb"))
    a2 = p.atom(subject, 2, ("#subject", "default"), (a1, "subject"), "up")
    a3 = p.atom(predicate, 2, ("#predicate", "default"), (a1, "predicate"), "up")
    a4 = p.atom(object, 3, ("#object", "default"), (a2, "object"), "left")
    return (verb + " " + subject + " " + predicate + " " + object,
            (a1, a2, a3, a4))


# <verb> [ the | a ] <subject> [ is | has ] <object>
vso_pattern = re.compile(r"(\w+)\s+(the\s+)?(a\s+)?((\w+)|(\(.+?\)))\s+(the\s+)?(is\s+)?(has\s+)?(.*)")
# <verb> [ the | a ] <subject> [ is | has ] <predicate> [ of ] <object>
vspo_pattern = re.compile(r"(\w+)\s+(the\s+)?(a\s+)?((\w+)|(\(.+?\)))\s+(the\s+)?(is\s+)?(has\s+)?(\w+)\s+(of\s+)?(.*)")


def match_verb_subject_object_string(say):
    m = vso_pattern.match(say)
    if m:
        verb = m.group(1).lower()
        if verb in vso_dict:
            subject, object = m.group(4), m.group(10).rstrip('.!')
            return verb_subject_object(verb, subject, object)
    m = vspo_pattern.match(say)
    if m:
        verb = m.group(1).lower()
        if verb in vspo_dict:
            subject, predicate, object = m.group(4), m.group(10), m.group(12).rstrip('.!')
            return verb_subject_predicate_object(verb, subject, predicate, object)
    return None


sell_pattern = re.compile(r"sell\s+(an?\s+|the\s+|this\s+|that\s+)?(.*)")


def match_sell(say):
    if say and say[-1] == '.': say = say[:-1]
    m = sell_pattern.match(say)
    if not m: return
    verb = "sell"
    verb_id = word2node.get(verb)
    if not verb_id: return
    a1 = p.atom(verb, 1, (verb_id[1], "default"), (None, "verb"))
    object = m.group(2)
    object_id = word2node.get(object)
    if object_id:
        a2 = p.atom(object, 2, (object_id[1], "default"), (a1, "object"), "up")
    else:
        a2 = p.atom(object, 2)
    return (say, (a1, a2))


buy_pattern = re.compile(r"buy\s+(an?\s+|the\s+|this\s+|that\s+)?(.*)")


def match_buy(say):
    if say and say[-1] == '.': say = say[:-1]
    m = buy_pattern.match(say)
    if not m: return
    verb = "buy"
    verb_id = word2node.get(verb)
    if not verb_id: return
    object = m.group(2)
    object_id = word2node.get(object)
    if not object_id: return
    a1 = p.atom(verb, 1, (verb_id[1], "default"), (None, "verb"))
    a2 = p.atom(object, 2, (object_id[1], "default"), (a1, "object"), "up")
    return (say, (a1, a2))


hire_pattern = re.compile(r"hire\s+(\w+\s+)?(.*)")


def match_hire(say):
    if say and say[-1] == '.': say = say[:-1]
    m = hire_pattern.match(say)
    if not m: return
    verb = "hire"
    verb_id = word2node.get(verb)
    if not verb_id: return
    object = m.group(2)
    object_id = word2node.get(object)
    if not object_id: return
    a1 = p.atom(verb, 1, (verb_id[1], "default"), (None, "verb"))
    a2 = p.atom(object, 2, (object_id[1], "default"), (a1, "object"), "up")
    return (say, (a1, a2))


vote_pattern = re.compile(r"vote\s+for\s+(.*)")


def match_vote(say):
    if say and say[-1] == '.': say = say[:-1]
    m = vote_pattern.match(say)
    if not m: return
    verb = "vote"
    verb_id = word2node.get(verb)
    if not verb_id: return
    object = m.group(2)
    object_id = word2node.get(object)
    if not object_id: return
    a1 = p.atom(verb, 1, (verb_id[1], "default"), (None, "verb"))
    a2 = p.atom(object, 2, (object_id[1], "default"), (a1, "object"), "up")
    return (say, (a1, a2))


desire_pattern = re.compile("(.*)\s+(would like|want)\s+to\s+(.*)")


def match_desire(say):
    m = desire_pattern.match(say)
    if not m: return
    verb = "desire"
    verb_id = word2node.get(verb)
    if not verb_id: return
    subject = m.group(1)
    subject_id = word2node.get(subject)
    if not subject_id: return
    object = m.group(3)
    object_sentence = match_buy(object)
    if not object_sentence: object_sentence = match_sell(object)
    if not object_sentence: object_sentence = match_vote(object)
    if not object_sentence: object_sentence = match_hire(object)
    if not object_sentence: return
    a1 = p.atom(verb, 1, (verb_id[1], "default"), (None, "verb"))
    a2 = p.atom(subject, 2, (subject_id[1], "default"), (a1, "subject"), "up")
    a3 = object_sentence[1][0]
    a3.link(a2, "left")
    a3.synlink_type = "object"
    return (say, (a1, a2) + object_sentence[1])


importance_pattern = re.compile("(.*)ing is more important than (.*)ing.")


def convert_english_to_interlinguish(me, say_entity):
    """convert English text to Interlinguish array"""
    try:
        say = say_entity.say
    except AttributeError:
        return None
    if hasattr(me, 'name'):
        name_begin = me.name + ", "
        if say[:len(name_begin)] == name_begin:
            say = say[len(name_begin):]
    match = importance_pattern.match(say)
    if match:
        sub = word2node.get(str.lower(match.group(1)))
        obj = word2node.get(match.group(2))
        if not sub or not obj: return None
        res = importance(sub, '>', obj)
        return res[1]
    res = match_verb_subject_object_string(say)
    if res:
        return res[1]
    res = match_desire(say)
    if res:
        return res[1]
    return None


def get_verb(interlinguish):
    return interlinguish[0].lexlink.id[1:]
