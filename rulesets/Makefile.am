SUBDIRS = astronomy skills

noinst_LIBRARIES = librulesetbase.a librulesetmind.a librulesetentity.a \
                   libscriptpython.a

librulesetbase_a_SOURCES = Entity.cpp Entity.h \
			   Script.cpp Script.h

librulesetmind_a_SOURCES = BaseMind.cpp BaseMind.h \
			   ExternalMind.cpp ExternalMind.h \
			   MindFactory.cpp MindFactory.h \
			   MemMap.cpp MemMap.h MemMap_methods.h

librulesetentity_a_SOURCES = Thing.cpp Thing.h \
			     World.cpp World.h \
			     Character.cpp Character.h \
			     Creator.cpp Creator.h \
			     Plant.cpp Plant.h \
			     Food.cpp Food.h \
			     Stackable.cpp Stackable.h \
			     Movement.cpp Movement.h \
			     Pedestrian.cpp Pedestrian.h \
			     EntityFactory.cpp EntityFactory.h \
			     ThingFactory.cpp ThingFactory.h \
			     CharacterFactory.cpp CharacterFactory.h \
			     CreatorFactory.cpp CreatorFactory.h \
			     PlantFactory.cpp PlantFactory.h \
			     FoodFactory.cpp FoodFactory.h \
			     StackFactory.cpp StackFactory.h \
			     StructFactory.cpp StructFactory.h

libscriptpython_a_SOURCES = Py_Object.cpp Py_Object.h \
			    Py_Operation.cpp Py_Operation.h \
			    Py_Oplist.cpp Py_Oplist.h \
			    Py_Optime.cpp Py_Optime.h \
			    Py_Thing.cpp Py_Thing.h \
			    Py_Mind.cpp Py_Mind.h \
			    Py_Map.cpp Py_Map.h \
			    Py_Vector3D.cpp Py_Vector3D.h \
			    Py_WorldTime.cpp Py_WorldTime.h \
			    Py_World.cpp Py_World.h \
			    Py_Location.cpp Py_Location.h \
			    Python_API.cpp Python_API.h \
			    PythonScript.cpp PythonScript.h \
			    PythonThingScript.cpp PythonThingScript.h \
			    PythonMindScript.cpp PythonMindScript.h

dist-hook:
	tar cf - `find basic -name \*.py` | (cd $(distdir); tar xf -)
	tar cf - `find acorn -name \*.py` | (cd $(distdir); tar xf -)

install-data-local:
	mkdirhier $(datadir)/cyphesis/rulesets
	tar cf - `find basic -name \*.py` | (cd $(datadir)/cyphesis/rulesets; tar xf -)
	tar cf - `find acorn -name \*.py` | (cd $(datadir)/cyphesis/rulesets; tar xf -)
	cat basic/hooks/ruleset_import_hooks.py | sed 's%PREFIX%$(datadir)/cyphesis%' > $(datadir)/cyphesis/rulesets/basic/hooks/ruleset_import_hooks.py
