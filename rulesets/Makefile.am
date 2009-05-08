# SUBDIRS = astronomy skills

INCLUDES = -I$(top_srcdir)

RULESETS = basic/mind mason

moduledir = $(datadir)/cyphesis/scripts/cyphesis
dist_module_DATA = basic/world/objects/Thing.py \
                   basic/world/probability.py \
                   basic/world/objects/__init__.py \
                   basic/editor.py

noinst_LIBRARIES = librulesetbase.a librulesetmind.a librulesetentity.a \
                   libscriptpython.a

librulesetbase_a_SOURCES = LocatedEntity.cpp LocatedEntity.h \
			   Entity.cpp Entity.h \
			   EntityProperties.cpp \
			   AtlasProperties.cpp AtlasProperties.h \
			   ActivePropertyFactory.cpp \
			   ActivePropertyFactory_impl.h \
			   ActivePropertyFactory.h \
			   Container.cpp Container.h \
			   Script.cpp Script.h 

librulesetentity_a_SOURCES = Thing.cpp Thing.h \
			     World.cpp World.h \
			     Character.cpp Character.h \
			     Creator.cpp Creator.h \
			     Plant.cpp Plant.h \
			     Stackable.cpp Stackable.h \
			     Motion.cpp Motion.h \
			     Domain.cpp Domain.h \
			     Movement.cpp Movement.h \
			     Pedestrian.cpp Pedestrian.h \
			     EntityProperty.cpp EntityProperty.h \
			     OutfitProperty.cpp OutfitProperty.h \
			     LineProperty.cpp LineProperty.h \
			     AreaProperty.cpp AreaProperty.h \
			     TerrainProperty.cpp TerrainProperty.h \
			     TerrainModProperty.cpp TerrainModProperty.h \
			     TerrainMod_impl.h \
			     CalendarProperty.cpp CalendarProperty.h \
			     SolidProperty.cpp SolidProperty.h \
			     StatusProperty.cpp StatusProperty.h \
			     HandlerProperty.cpp HandlerProperty.h \
			     HandlerProperty_impl.h \
			     MultiHandlerProperty.cpp MultiHandlerProperty.h \
			     MultiHandlerProperty_impl.h \
			     BBoxProperty.cpp BBoxProperty.h \
			     TransientProperty.cpp TransientProperty.h \
			     MindProperty.cpp MindProperty.h \
			     InternalProperties.cpp InternalProperties.h \
			     StatisticsProperty.cpp StatisticsProperty.h \
			     Task.cpp Task.h \
			     TaskScript.cpp TaskScript.h \
			     ArithmeticScript.cpp ArithmeticScript.h \
			     ArithmeticFactory.cpp ArithmeticFactory.h


librulesetmind_a_SOURCES = BaseMind.cpp BaseMind.h \
			   MindFactory.cpp MindFactory.h \
			   MemEntity.cpp MemEntity.h \
			   MemMap.cpp MemMap.h

libscriptpython_a_SOURCES = Py_Object.cpp Py_Object.h \
			    Py_Operation.cpp Py_Operation.h \
			    Py_RootEntity.cpp Py_RootEntity.h \
			    Py_Oplist.cpp Py_Oplist.h \
			    Py_Vector3D.cpp Py_Vector3D.h \
			    Py_Point3D.cpp Py_Point3D.h \
			    Py_Thing.cpp Py_Thing.h \
			    Py_EntityWrapper.h \
			    Py_Mind.cpp Py_Mind.h \
			    Py_Map.cpp Py_Map.h \
			    Py_Quaternion.cpp Py_Quaternion.h \
			    Py_BBox.cpp Py_BBox.h \
			    Py_WorldTime.cpp Py_WorldTime.h \
			    Py_World.cpp Py_World.h \
			    Py_Location.cpp Py_Location.h \
			    Py_Task.cpp Py_Task.h \
			    Py_Property.cpp Py_Property.h \
			    Py_TerrainProperty.cpp \
			    Python_API.cpp Python_API.h \
			    Python_Script_Utils.h \
			    PythonContext.cpp PythonContext.h \
			    PythonWrapper.cpp PythonWrapper.h \
			    PythonScript.cpp PythonScript.h \
			    PythonEntityScript.cpp PythonEntityScript.h \
			    PythonArithmeticScript.cpp PythonArithmeticScript.h

dist-hook:
	(cd $(top_srcdir)/rulesets && tar cf - `find $(RULESETS) -name \*.py`) | (cd $(distdir) && tar xf -)

install-data-local:
	(cd $(top_srcdir)/rulesets && find $(RULESETS) -type d ! -name CVS | xargs -I PYRDIR $(install_sh) -d $(DESTDIR)$(datadir)/cyphesis/rulesets/PYRDIR)
	(cd $(top_srcdir)/rulesets && tar cf - $(TAR_PERM_FLAGS) `find $(RULESETS) -name \*.py`) | (cd $(DESTDIR)$(datadir)/cyphesis/rulesets && tar xpf -)
	rm -f $(DESTDIR)$(datadir)/cyphesis/rulesets/basic/world/__init__.py
	rm -f $(DESTDIR)$(datadir)/cyphesis/rulesets/basic/world/__init__.py?

uninstall-local:
	(cd $(top_srcdir)/rulesets && find $(RULESETS) -name \*.py | xargs -I PYRFILE rm -f $(DESTDIR)$(datadir)/cyphesis/rulesets/PYRFILE)
	find $(DESTDIR)$(datadir)/cyphesis -type d -delete
