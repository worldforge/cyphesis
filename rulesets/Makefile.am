# SUBDIRS = astronomy skills

AM_CPPFLAGS = -I$(top_srcdir) -I${top_builddir}

RULESETS = basic/mind mason minimal mars deeds

moduledir = $(datadir)/cyphesis/scripts/cyphesis
dist_module_DATA = basic/world/objects/Thing.py \
                   basic/world/probability.py \
                   basic/world/objects/__init__.py \
                   basic/editor.py

noinst_LIBRARIES = librulesetbase.a librulesetmind.a librulesetentity.a \
                   libscriptpython.a libentityfilter.a

librulesetbase_a_SOURCES = LocatedEntity.cpp LocatedEntity.h \
			   EntityProperties.cpp \
			   AtlasProperties.cpp AtlasProperties.h \
			   Container.cpp Container.h \
			   Script.cpp Script.h 

librulesetentity_a_SOURCES = \
			     Entity.cpp Entity.h \
			     Thing.cpp Thing.h \
			     World.cpp World.h \
			     Character.cpp Character.h \
			     Creator.cpp Creator.h \
			     Plant.cpp Plant.h \
			     Stackable.cpp Stackable.h \
			     Motion.cpp Motion.h \
			     Domain.cpp Domain.h \
			     BulletDomain.cpp BulletDomain.h \
			     ExternalMind.cpp ExternalMind.h \
			     Movement.cpp Movement.h \
			     Pedestrian.cpp Pedestrian.h \
			     EntityProperty.cpp EntityProperty.h \
			     OutfitProperty.cpp OutfitProperty.h \
			     LineProperty.cpp LineProperty.h \
			     AreaProperty.cpp AreaProperty.h \
			     TerrainProperty.cpp TerrainProperty.h \
			     TerrainEffectorProperty.cpp \
                             TerrainEffectorProperty.h \
			     TerrainModProperty.cpp TerrainModProperty.h \
			     TerrainModTranslator.cpp TerrainModTranslator.h \
			     CalendarProperty.cpp CalendarProperty.h \
			     SolidProperty.cpp SolidProperty.h \
			     StatusProperty.cpp StatusProperty.h \
			     BBoxProperty.cpp BBoxProperty.h \
			     TransientProperty.cpp TransientProperty.h \
			     InternalProperties.cpp InternalProperties.h \
			     StatisticsProperty.cpp StatisticsProperty.h \
			     TasksProperty.cpp TasksProperty.h \
			     SpawnProperty.cpp SpawnProperty.h \
			     VisibilityProperty.cpp VisibilityProperty.h \
			     ExternalProperty.cpp ExternalProperty.h \
			     BiomassProperty.cpp BiomassProperty.h \
			     BurnSpeedProperty.cpp BurnSpeedProperty.h \
			     DecaysProperty.cpp DecaysProperty.h \
			     Task.cpp Task.h \
			     ArithmeticScript.cpp ArithmeticScript.h \
			     ArithmeticFactory.cpp ArithmeticFactory.h \
			     SuspendedProperty.cpp SuspendedProperty.h \
			     SpawnerProperty.cpp SpawnerProperty.h \
			     ImmortalProperty.cpp ImmortalProperty.h \
			     RespawningProperty.cpp RespawningProperty.h \
			     DefaultLocationProperty.cpp DefaultLocationProperty.h \
			     DomainProperty.cpp DomainProperty.h \
			     LimboProperty.cpp LimboProperty.h \
			     PhysicalDomain.cpp PhysicalDomain.h \
			     VoidDomain.cpp VoidDomain.h

libentityfilter_a_SOURCES = entityfilter/Filter.cpp entityfilter/Filter.h \
			     entityfilter/ParserDefinitions.h \
			     entityfilter/Providers.cpp entityfilter/Providers.h \
			     entityfilter/Predicates.cpp entityfilter/Predicates.h

librulesetmind_a_SOURCES = BaseMind.cpp BaseMind.h \
			   MindFactory.cpp MindFactory.h \
			   MindProperty.cpp MindProperty.h \
			   MemEntity.cpp MemEntity.h \
			   MemMap.cpp MemMap.h

libscriptpython_a_SOURCES = Py_Message.cpp Py_Message.h \
			    Py_Operation.cpp Py_Operation.h \
			    Py_RootEntity.cpp Py_RootEntity.h \
			    Py_Oplist.cpp Py_Oplist.h \
			    Py_Vector3D.cpp Py_Vector3D.h \
			    Py_Point3D.cpp Py_Point3D.h \
			    Py_Thing.cpp Py_Thing.h \
			    Py_EntityWrapper.h \
			    Py_Filter.cpp Py_Filter.h \
			    Py_Map.cpp Py_Map.h \
			    Py_Quaternion.cpp Py_Quaternion.h \
			    Py_BBox.cpp Py_BBox.h \
			    Py_WorldTime.cpp Py_WorldTime.h \
			    Py_World.cpp Py_World.h \
			    Py_Location.cpp Py_Location.h \
			    Py_Task.cpp Py_Task.h \
			    Py_Shape.cpp Py_Shape.h \
			    Py_Property.cpp Py_Property.h \
			    Py_TerrainModProperty.cpp \
			    Py_TerrainProperty.cpp \
			    Python_API.cpp Python_API.h \
			    Python_Script_Utils.h \
			    PythonClass.cpp PythonClass.h \
			    PythonContext.cpp PythonContext.h \
			    PythonWrapper.cpp PythonWrapper.h \
			    PythonEntityScript.cpp PythonEntityScript.h \
			    PythonArithmeticScript.cpp PythonArithmeticScript.h \
			    PythonArithmeticFactory.cpp \
                            PythonArithmeticFactory.h \
                            PythonScriptFactory.cpp PythonScriptFactory.h \
                            PythonScriptFactory_impl.h

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
