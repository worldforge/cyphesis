#include <stdio.h>
#include <unistd.h>

#include <Python.h>

#include "Python_API.h"
#include "Thing.h"

#include <modules/Location.h>
#include <server/WorldTime.h>
#include <common/const.h>

static void Function_dealloc(FunctionObject * self)
{
    PyMem_DEL(self);
}

static PyObject * log_debug(PyObject * self, PyObject * args, PyObject * kwds)
{
    printf("LOG.DEBUG\n");
    Py_INCREF(Py_None);
    return Py_None;
}

PyTypeObject log_debug_type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,
	"Function",
	sizeof(FunctionObject),
	0,
	/* methods */
	(destructor)Function_dealloc,
	0,		/* tp_print */
	0,		/* tp_getattr */
	0,		/* tp_setattr */
	0,		/* tp_compare */
	0,		/* tp_repr */
	0,		/* tp_as_number */
	0,		/* tp_as_sequence */
	0,		/* tp_as_mapping */
	0,		/* tp_hash */
	log_debug,	/* tp_call */
};

static PyObject * dictlist_add_value(PyObject * self, PyObject * args, PyObject * kwds)
{
    PyObject * dict, * item;
    char * key;
    if (!PyArg_ParseTuple(args, "OsO", &dict, &key, &item)) {
        return NULL;
    }
    if (!PyDict_Check(dict)) {
        PyErr_SetString(PyExc_TypeError, "Dict is not a dictionary");
        return NULL;
    }
    PyObject * list = PyDict_GetItemString(dict, key);
    if (list != NULL) {
        if (!PyList_Check(list)) {
            PyErr_SetString(PyExc_TypeError, "Dict does not contain a list");
            return NULL;
        }
        int i, size = PyList_Size(list);
        for(i = 0; i < size; i++) {
            if (PyList_GetItem(list, i) == item) {
                goto present;
            }
        }
        PyList_Append(list, item);
    } else {
        list = PyList_New(1);
        Py_INCREF(item);
        PyList_SetItem(list, 1, item);
        PyDict_SetItemString(dict, key, list);
        Py_DECREF(list);
    }
present:
    Py_INCREF(Py_None);
    return Py_None;
}

PyTypeObject dictlist_add_value_type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,
	"Function",
	sizeof(FunctionObject),
	0,
	/* methods */
	(destructor)Function_dealloc,
	0,		/* tp_print */
	0,		/* tp_getattr */
	0,		/* tp_setattr */
	0,		/* tp_compare */
	0,		/* tp_repr */
	0,		/* tp_as_number */
	0,		/* tp_as_sequence */
	0,		/* tp_as_mapping */
	0,		/* tp_hash */
	dictlist_add_value,	/* tp_call */
};

void Create_PyThing(Thing * thing, const string & package, const string & _type)
{
    string type = _type;
    type[0] = toupper(type[0]);
    PyObject * mod_dict;
    if ((mod_dict = PyImport_ImportModule((char *)package.c_str()))==NULL) {
        cerr << "Cld no find python module " << package << endl << flush;
            PyErr_Print();
        return;
    } else {
        cout << "Got python module " << package << endl << flush;
    }
    PyObject * my_class = PyObject_GetAttrString(mod_dict, (char *)type.c_str());
    if (my_class == NULL) {
        cerr << "Cld no find class in module " << package << endl << flush;
            PyErr_Print();
        return;
    } else {
        cout << "Got python class " << type << " in " << package << endl << flush;
    }
    if (PyCallable_Check(my_class) == 0) {
            cout << "It does not seem to be a class at all" << endl << flush;
        return;
    }
    ThingObject * pyThing = newThingObject(NULL);
    pyThing->m_thing = thing;
    if (thing->set_object(PyEval_CallFunction(my_class,"(O)", (PyObject *)pyThing)) == -1) {
        if (PyErr_Occurred() == NULL) {
            cerr << "Could not get python obj" << endl << flush;
        } else {
            cout << "Reporting python error for " << type << endl << flush;
            PyErr_Print();
        }
    }
}

static PyObject * location_new(PyObject * self, PyObject * args)
{
	LocationObject *o;
	// We need to deal with actual args here
	if (!PyArg_ParseTuple(args, "")) {
		return NULL;
	}
	o = newLocationObject(args);
	if ( o == NULL ) {
		return NULL;
	}
	o->location = new Location;
	return (PyObject *)o;
}

static PyObject * vector3d_new(PyObject * self, PyObject * args)
{
	Vector3DObject *o;
        Vector3D val;
	// We need to deal with actual args here
        PyObject * clist;
        double x,y,z;
        if ( (PyArg_ParseTuple(args, "O", &clist)) &&
                    (PyList_Check(clist)) &&
                    (PyList_Size(clist) == 3) ) {
            PyObject * X = PyList_GetItem(clist, 0);
            PyObject * Y = PyList_GetItem(clist, 1);
            PyObject * Z = PyList_GetItem(clist, 2);
            if (PyFloat_Check(X) && PyFloat_Check(Y) && PyFloat_Check(Z)) {
                val = Vector3D(PyFloat_AsDouble(X),
                               PyFloat_AsDouble(Y),
                               PyFloat_AsDouble(Z));
            } else if (PyInt_Check(X) && PyInt_Check(Y) && PyInt_Check(Z)) {
                val = Vector3D(double(PyInt_AsLong(X)),
                               double(PyInt_AsLong(Y)),
                               double(PyInt_AsLong(Z)));
            } else {
                PyErr_SetString(PyExc_TypeError, "Vector3D must take list of floats, or ints");
                return NULL;
            }
        } else if (PyArg_ParseTuple(args, "ddd", &x, &y, &z)) {
            val = Vector3D(x,y,z);
        } else if (!PyArg_ParseTuple(args, "")) {
            return NULL;
        }
            
	o = newVector3DObject(args);
	if ( o == NULL ) {
		return NULL;
	}
        o->coords = val;
	return (PyObject *)o;
}

static PyObject * worldtime_new(PyObject * self, PyObject * args)
{
	WorldTimeObject *o;
        	
        int seconds;
	if (!PyArg_ParseTuple(args, "i", &seconds)) {
		return NULL;
	}
	o = newWorldTimeObject(args);
	if ( o == NULL ) {
		return NULL;
	}
	o->time = new WorldTime(seconds);
	return (PyObject *)o;
}

static PyObject * oplist_new(PyObject * self, PyObject * args)
{
	OplistObject *o;
	
        PyObject * op1 = NULL, * op2 = NULL;
	if (!PyArg_ParseTuple(args, "|OO", &op1, &op2)) {
		return NULL;
	}
	o = newOplistObject(args);
	if ( o == NULL ) {
		return NULL;
	}
	o->ops = new oplist();
        if (op1 != NULL) {
           if ((PyTypeObject *)PyObject_Type(op1) == &RootOperation_Type) {
               o->ops->push_back( ((RootOperationObject*)op1)->operation );
           } else if (op1 != Py_None) {
               PyErr_SetString(PyExc_TypeError, "Argument must be an op");
               return NULL;
           }
        }
        if (op2 != NULL) {
           if ((PyTypeObject *)PyObject_Type(op2) == &RootOperation_Type) {
               o->ops->push_back( ((RootOperationObject*)op2)->operation );
           } else if (op1 != Py_None) {
               PyErr_SetString(PyExc_TypeError, "Argument must be an op");
               return NULL;
           }
        }
	return (PyObject *)o;
}

static PyObject * object_new(PyObject * self, PyObject * args)
{
	AtlasObject *o;
	
	if (!PyArg_ParseTuple(args, "")) {
		return NULL;
	}
	o = newAtlasObject(args);
	if ( o == NULL ) {
		return NULL;
	}
	o->m_obj = new Object;
	return (PyObject *)o;
}

static PyObject * entity_new(PyObject * self, PyObject * args, PyObject * kwds)
{
	AtlasObject *o;
        char * id = NULL;
	
	if (!PyArg_ParseTuple(args, "|s", id)) {
		return NULL;
	}
        Object::MapType _omap;
        Object obj(_omap);
        Object::MapType & omap = obj.AsMap();
        if (id != NULL) {
            omap["id"] = string(id);
        }
        PyObject * keys = PyMapping_Keys(kwds);
        PyObject * vals = PyMapping_Values(kwds);
        if ((keys == NULL) || (vals == NULL)) {
            PyErr_SetString(PyExc_TypeError, "Error in keywords");
            return NULL;
        }
        int i, size=PyMapping_Length(keys); 
        for(i = 0; i < size; i++) {
            char * key = PyString_AsString(PyList_GetItem(keys, i));
            PyObject * val = PyList_GetItem(vals, i);
            if ((strcmp(key, "location") == 0) &&
                ((PyTypeObject *)PyObject_Type(val) == &Location_Type)) {
                LocationObject * loc = (LocationObject*)val;
                loc->location->addObject(&obj);
            } else {
                Object val_obj = PyObject_asObject(val);
                if (val_obj.GetType() == Object::TYPE_NONE) {
                    fprintf(stderr, "Could not handle %s value in Entity()", key);
                    PyErr_SetString(PyExc_TypeError, "Argument type error to Entity()");
                    return NULL;
                }
                omap[key] = val_obj;
            }
        }
        
	o = newAtlasObject(args);
	if ( o == NULL ) {
		return NULL;
	}
	o->m_obj = new Object(obj);
	return (PyObject *)o;
}

static PyObject * cppthing_new(PyObject * self, PyObject * args)
{
	ThingObject *o;
	
	if (!PyArg_ParseTuple(args, "")) {
		return NULL;
	}
	o = newThingObject(args);
	if ( o == NULL ) {
		return NULL;
	}
	//o->m_thing = new Thing;
	return (PyObject *)o;
}

inline void addToArgs(Object::ListType & args, PyObject * ent)
{
    if (ent == NULL) {
        return;
    }
    if ((PyTypeObject*)PyObject_Type(ent) == &Object_Type) {
        AtlasObject * obj = (AtlasObject*)ent;
        if (obj->m_obj == NULL) {
            fprintf(stderr, "Invalid object in Operation arguments\n");
            return;
        }
        args.push_back(*obj->m_obj);
    } else {
        fprintf(stderr, "Non-entity passed as arg to Operation()\n");
    }
}

static PyObject * operation_new(PyObject * self, PyObject * args, PyObject * kwds)
{
    printf("New Operation\n");
    RootOperationObject * op;

    char * type;
    PyObject * to = NULL;
    PyObject * from = NULL;
    PyObject * arg1 = NULL;
    PyObject * arg2 = NULL;
    PyObject * arg3 = NULL;

    printf("New Operation: parsing args\n");
    if (!PyArg_ParseTuple(args, "s|OOO", &type, &arg1, &arg2, &arg3)) {
        return NULL;
    }
    printf("New Operation: creating operation\n");
    op = newAtlasRootOperation(args);
    if (op == NULL) {
        return NULL;
    }
    op->operation = new RootOperation;
    if (strcmp(type, "tick") == 0) {
        *op->operation = Tick::Instantiate();
    } else if (strcmp(type, "create") == 0) {
        *op->operation = Create::Instantiate();
    } else if (strcmp(type, "setup") == 0) {
        *op->operation = Setup::Instantiate();
    } else if (strcmp(type, "look") == 0) {
        *op->operation = Look::Instantiate();
    } else if (strcmp(type, "move") == 0) {
        *op->operation = Move::Instantiate();
    } else if (strcmp(type, "talk") == 0) {
        *op->operation = Talk::Instantiate();
    } else {
        fprintf(stderr, "ERROR: PYTHON CREATING AN UNHANDLED %s OPERATION\n", type);
        *op->operation = RootOperation::Instantiate();
    }
    if (PyMapping_HasKeyString(kwds, "to")) {
        to = PyMapping_GetItemString(kwds, "to");
        printf("Operation creation sets to\n");
        PyObject * to_id;
        if ((to_id = PyObject_GetAttrString(to, "id")) == NULL) {
            fprintf(stderr, "To was not really an entity, as it had no id\n");
            return NULL;
        }
        if (!PyString_Check(to_id)) {
            fprintf(stderr, "To id is not a string\n");
            return NULL;
        }
        op->operation->SetTo(PyString_AsString(to_id));
    }
    if (PyMapping_HasKeyString(kwds, "from_")) {
        from = PyMapping_GetItemString(kwds, "from_");
        printf("Operation creation sets from\n");
        PyObject * from_id;
        if ((from_id = PyObject_GetAttrString(from, "id")) == NULL) {
            fprintf(stderr, "From was not really an entity, as it had no id\n");
            return NULL;
        }
        if (!PyString_Check(from_id)) {
            fprintf(stderr, "From id is not a string\n");
            return NULL;
        }
        op->operation->SetFrom(PyString_AsString(from_id));
        // FIXME I think I need to actually do something with said value now
    }
    Object::ListType args_list;
    addToArgs(args_list, arg1);
    addToArgs(args_list, arg2);
    addToArgs(args_list, arg3);
    op->operation->SetArgs(args_list);
    return (PyObject *)op;
}

static PyObject * set_kw(PyObject * meth_self, PyObject * args)
{
    // Takes self, kw, name, default=None
    PyObject * self;
    PyObject * kw;
    char * name;
    PyObject * def = NULL;

    if (!PyArg_ParseTuple(args, "OOs|O", &self, &kw, &name, &def)) {
        return NULL;
    }
    printf("SET_KW: %s: Got args\n", name);
    PyObject * attr = PyObject_GetAttrString(self, "attributes");
    if (attr == NULL) {
        PyErr_SetString(PyExc_TypeError, "SET_KW: No attributes list");
        return NULL;
    }
    int i = PyList_Size(attr);
    char * entry;
    PyObject * item;
    for(i= 0; i < PyList_Size(attr); i++) {
        item = PyList_GetItem(attr, i);
        if (!PyString_Check(item)) {
            continue;
        }
        entry = PyString_AsString(item);
        if (strcmp(entry, name) == 0) {
            goto list_contains_it;
        }
        // Should I free entry at this point?
    }
    {
      printf("SET_KW: Adding it to attributes\n");
      PyObject * namestr = PyString_FromString(name);
      PyList_Append(attr, namestr);
      Py_DECREF(namestr);
    }
list_contains_it:
    if (!PyDict_Check(kw)) {
        PyErr_SetString(PyExc_TypeError, "SET_KW: kw not a dict");
        return NULL;
    }
    PyObject * value = NULL;
    if ((value = PyDict_GetItemString(kw, name)) == NULL) {
        PyObject * copy = PyDict_GetItemString(kw, "copy");
        if ((copy != NULL) && (PyObject_HasAttrString(copy, name))) {
            value = PyObject_GetAttrString(copy, name);
            printf("SET_KW: Getting it from \"copy\"\n");
        } else {
            value = def;
            printf("SET_KW: Setting to default\n");
        }
    }
    if (value == NULL) {
        Py_INCREF(Py_None);
        value = Py_None;
    }
    PyObject_SetAttrString(self, name, value);
    
    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef atlas_methods[] = {
    /* {"system",	spam_system, METH_VARARGS}, */
    {"Operation",  (PyCFunction)operation_new,	METH_VARARGS|METH_KEYWORDS},
    {"Location",   location_new,		METH_VARARGS},
    {"Object",     object_new,			METH_VARARGS},
    {"Entity",     (PyCFunction)entity_new,	METH_VARARGS|METH_KEYWORDS},
    {"Message",    oplist_new,			METH_VARARGS},
    {"cppThing",   cppthing_new,		METH_VARARGS},
    {NULL,		NULL}				/* Sentinel */
};

static PyMethodDef Vector3D_methods[] = {
	{"Vector3D",	vector3d_new,	METH_VARARGS},
	{NULL,		NULL}				/* Sentinel */
};

static PyMethodDef server_methods[] = {
	{"WorldTime",	worldtime_new,	METH_VARARGS},
	{NULL,		NULL}				/* Sentinel */
};

static PyMethodDef common_methods[] = {
	//{"null",	null_new,	METH_VARARGS},
	{NULL,		NULL}				/* Sentinel */
};

static PyMethodDef misc_methods[] = {
	{"set_kw",	set_kw,		METH_VARARGS},
	{NULL,		NULL}				/* Sentinel */
};

void init_python_api()
{
	char * cwd;

	if ((cwd = getcwd(NULL, 0)) != NULL) {
                size_t len = strlen(cwd) * 2 + 60;
                char * pypath = (char *)malloc(len);
                strcpy(pypath, cwd);
                strcat(pypath, "/rulesets/basic:");
                // This should eventually pull in a ruleset name from
                // the commandline args.
                // basic ruleset should always be left on the end
                strcat(pypath, cwd);
                strcat(pypath, "/rulesets/acorn");
		setenv("PYTHONPATH", pypath, 1);
	}

	Py_Initialize();

        PyRun_SimpleString("from hooks import ruleset_import_hooks\n");
        PyRun_SimpleString("ruleset_import_hooks.install(['acorn','basic'])\n");

	if (Py_InitModule("atlas", atlas_methods) == NULL) {
		printf("Failed to Create atlas thing\n");
		return;
	}

	if (Py_InitModule("Vector3D", Vector3D_methods) == NULL) {
		printf("Failed to Create Vector3D thing\n");
		return;
	}

        PyObject * misc;
        if ((misc = Py_InitModule("misc", misc_methods)) == NULL) {
		printf("Failed to Create misc thing\n");
		return;
	}

	PyObject * common;
	PyObject * dict;
	if ((common = Py_InitModule("common", common_methods)) == NULL) {
		printf("Failed to Create common thing\n");
		return;
	}
	PyObject * _const = PyModule_New("const");
	PyObject * log = PyModule_New("log");
	dict = PyModule_GetDict(common);
	PyDict_SetItemString(dict, "const", _const);
	PyDict_SetItemString(dict, "log", log);
        PyObject * debug = (PyObject *)PyObject_NEW(FunctionObject, &log_debug_type);
	PyObject_SetAttrString(log, "debug", debug);
	//PyDict_SetItemString(dict, "misc", misc);
	PyObject_SetAttrString(_const, "server_python", PyInt_FromLong(0));
	PyObject_SetAttrString(_const, "debug_level",
			PyInt_FromLong(consts::debug_level));
	PyObject_SetAttrString(_const, "debug_thinking",
			PyInt_FromLong(consts::debug_thinking));

	PyObject_SetAttrString(_const, "time_multiplier",
			PyFloat_FromDouble(consts::time_multiplier));
	PyObject_SetAttrString(_const, "base_velocity_coefficient",
			PyFloat_FromDouble(consts::base_velocity_coefficient));
	PyObject_SetAttrString(_const, "base_velocity",
			PyFloat_FromDouble(consts::base_velocity));

	PyObject_SetAttrString(_const, "basic_tick",
			PyFloat_FromDouble(consts::basic_tick));
	PyObject_SetAttrString(_const, "day_in_seconds",
			PyInt_FromLong(consts::day_in_seconds));

	PyObject_SetAttrString(_const, "sight_range",
			PyFloat_FromDouble(consts::sight_range));
	PyObject_SetAttrString(_const, "hearing_range",
			PyFloat_FromDouble(consts::hearing_range));
	PyObject_SetAttrString(_const, "collision_range",
			PyFloat_FromDouble(consts::collision_range));
	PyObject_SetAttrString(_const, "enable_ranges",
			PyInt_FromLong(consts::enable_ranges));

	PyObject * server;
	if ((server = Py_InitModule("server", server_methods)) == NULL) {
		printf("Failed to Create server thing\n");
		return;
	}
	dict = PyModule_GetDict(server);
	PyObject * dictlist = PyModule_New("dictlist");
        PyObject * add_value = (PyObject *)PyObject_NEW(FunctionObject, &dictlist_add_value_type);
	PyObject_SetAttrString(dictlist, "add_value", add_value);
	PyDict_SetItemString(dict, "dictlist", dictlist);
}
