noinst_LIBRARIES = librulesets.a

librulesets_a_SOURCES = Thing.cpp Thing.h \
			ThingFactory.cpp ThingFactory.h \
			BaseMind.cpp BaseMind.h \
			MemMap.cpp MemMap.h MemMap_methods.h \
			ExternalMind.cpp ExternalMind.h \
			Character.cpp Character.h \
			Creator.cpp Creator.h \
			Py_Object.cpp Py_Object.h \
			Py_Operation.cpp Py_Operation.h \
			Py_Oplist.cpp Py_Oplist.h \
			Py_Optime.cpp Py_Optime.h \
			Py_Thing.cpp Py_Thing.h \
			Py_Map.cpp Py_Map.h \
			Py_Vector3D.cpp Py_Vector3D.h \
			Py_WorldTime.cpp Py_WorldTime.h \
			Py_World.cpp Py_World.h \
			Py_Location.cpp Py_Location.h \
			Python_API.cpp Python_API.h

dist-hook:
	tar cf - `find basic -name \*.py` | (cd $(distdir); tar xf -)
	tar cf - `find acorn -name \*.py` | (cd $(distdir); tar xf -)

install-data-local:
	mkdirhier $(datadir)/cyphesis/rulesets
	tar cf - `find basic -name \*.py` | (cd $(datadir)/cyphesis/rulesets; tar xf -)
	tar cf - `find acorn -name \*.py` | (cd $(datadir)/cyphesis/rulesets; tar xf -)
	cat basic/hooks/ruleset_import_hooks.py | sed 's%PREFIX%$(datadir)/cyphesis%' > $(datadir)/cyphesis/rulesets/basic/hooks/ruleset_import_hooks.py
