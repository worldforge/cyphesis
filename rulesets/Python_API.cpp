#include <stdio.h>
#include <unistd.h>

#include <Python.h>

#include "Python_API.h"
#include "Thing.h"

#include <Atlas/Objects/Operation/Sight.h>

#include <modules/Location.h>
#include <server/WorldTime.h>
#include <server/server.h>
#include <common/const.h>

static void Function_dealloc(FunctionObject * self)
{
    PyMem_DEL(self);
}

static PyObject * log_debug(PyObject * self, PyObject * args, PyObject * kwds)
{
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

static PyObject * dictlist_remove_value(PyObject * self, PyObject * args, PyObject * kwds)
{
    PyObject * dict, * item;
    long remove_empty_key = 1;
    if (!PyArg_ParseTuple(args, "OO|i", &dict, &item, &remove_empty_key)) {
        return NULL;
    }
    int flag=0;
    if (!PyDict_Check(dict)) {
        PyErr_SetString(PyExc_TypeError, "Trying to set item in not dictlist");
        return NULL;
    }
    PyObject * keys = PyDict_Keys(dict);
    PyObject * values = PyDict_Values(dict);

    if ((keys == NULL) || (values == NULL)) {
        PyErr_SetString(PyExc_TypeError, "Error getting keys from dictlist");
        return NULL;
    }
    int i, size = PyList_Size(keys);
    for(i = 0; i < size; i++) {
        PyObject * value = PyList_GetItem(values, i);
        PyObject * key = PyList_GetItem(keys, i);
        int j, lsize = PyList_Size(value);
        for(j = 0; j < lsize; j++) {
            if (PyList_GetItem(value, j) == item) {
                flag = 1;
                PyList_SetSlice(value, j, j+1, NULL);
                if ((remove_empty_key !=0) && (PyList_Size(value) == 0)) {
                    PyDict_DelItem(dict, key);
                }
            }
        }
    }
    Py_DECREF(keys);
    Py_DECREF(values);
    return PyInt_FromLong(flag);
}

PyTypeObject dictlist_remove_value_type = {
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
	dictlist_remove_value,	/* tp_call */
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
        list = PyList_New(0);
        //Py_INCREF(item);
        PyList_Append(list, item);
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
    PyObject * package_name = PyString_FromString((char *)package.c_str());
    if ((mod_dict = PyImport_Import(package_name))==NULL) {
        cerr << "Cld no find python module " << package << endl << flush;
            PyErr_Print();
        return;
    }
    PyObject * my_class = PyObject_GetAttrString(mod_dict, (char *)type.c_str());
    if (my_class == NULL) {
        cerr << "Cld not find class " << type << " in module " << package
             << endl << flush;
        PyErr_Print();
        return;
    }
    if (PyCallable_Check(my_class) == 0) {
        cerr << "It does not seem to be a class at all" << endl << flush;
        return;
    }
    ThingObject * pyThing = newThingObject(NULL);
    pyThing->m_thing = thing;
    if (thing->set_object(PyEval_CallFunction(my_class,"(O)", (PyObject *)pyThing)) == -1) {
        if (PyErr_Occurred() == NULL) {
            cerr << "Could not get python obj" << endl << flush;
        } else {
            cerr << "Reporting python error for " << type << endl << flush;
            PyErr_Print();
        }
    }
    Py_DECREF(my_class);
    Py_DECREF(mod_dict);
}

static PyObject * is_location(PyObject * self, PyObject * args)
{
    PyObject * loc;
    if (!PyArg_ParseTuple(args, "O", &loc)) {
        return NULL;
    }
    if ((PyTypeObject*)PyObject_Type(loc) == &Location_Type) {
        Py_INCREF(Py_True);
        return Py_True;
    }
    Py_INCREF(Py_False);
    return Py_False;
}

static PyObject * location_new(PyObject * self, PyObject * args)
{
    LocationObject *o;
    // We need to deal with actual args here
    PyObject * parentO, * coordsO = NULL;
    if (PyArg_ParseTuple(args, "O|O", &parentO, &coordsO)) {
        if (((PyTypeObject *)PyObject_Type(parentO) != &Thing_Type) &&
            ((PyTypeObject *)PyObject_Type(parentO) != &World_Type)) {
            if (PyObject_HasAttrString(parentO, "cppthing")) {
                parentO = PyObject_GetAttrString(parentO, "cppthing");
            }
            if ((PyTypeObject *)PyObject_Type(parentO) != &Thing_Type) {
                PyErr_SetString(PyExc_TypeError, "Arg parent required");
                return NULL;
            }
        }
        if ((coordsO != NULL) &&
            ((PyTypeObject *)PyObject_Type(coordsO) != &Vector3D_Type)) {
            PyErr_SetString(PyExc_TypeError, "Arg coords required");
            return NULL;
        }

        BaseEntity * parent_ent;
        if ((PyTypeObject *)PyObject_Type(parentO) == &World_Type) {
            WorldObject * parent = (WorldObject*)parentO;
            if (parent->world == NULL) {
                PyErr_SetString(PyExc_TypeError, "Parent world is invalid");
                return NULL;
            }
            parent_ent = parent->world;
        } else {
            ThingObject * parent = (ThingObject*)parentO;
            if (parent->m_thing == NULL) {
                PyErr_SetString(PyExc_TypeError, "Parent thing is invalid");
                return NULL;
            }
            parent_ent = parent->m_thing;
        }
        Vector3DObject * coords = (Vector3DObject*)coordsO;
        o = newLocationObject(NULL);
        if ( o == NULL ) {
            return NULL;
        }
        if (coords == NULL) {
            o->location = new Location(parent_ent, Vector3D());
        } else {
            o->location = new Location(parent_ent, coords->coords);
        }
        o->own = 1;
    } else if (PyArg_ParseTuple(args, "")) {
        o = newLocationObject(NULL);
        if ( o == NULL ) {
            return NULL;
        }
        o->location = new Location;
        o->own = 1;
    } else {
        return NULL;
    }
    return (PyObject *)o;
}

static PyObject * vector3d_new(PyObject * self, PyObject * args)
{
	Vector3DObject *o;
        Vector3D val;
	// We need to deal with actual args here
        PyObject * clist;
        double x,y,z;
        long ix,iy,iz;
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
        } else if (PyArg_ParseTuple(args, "iii", &ix, &iy, &iz)) {
            val = Vector3D(ix,iy,iz);
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

inline void addToOplist(PyObject * op, OplistObject * o)
{
    if (op != NULL) {
       if ((PyTypeObject *)PyObject_Type(op) == &RootOperation_Type) {
           o->ops->push_back( ((RootOperationObject*)op)->operation );
           ((RootOperationObject*)op)->own = 0;
       } else if (op != Py_None) {
           PyErr_SetString(PyExc_TypeError, "Argument must be an op");
           return;
       }
    }
}


static PyObject * oplist_new(PyObject * self, PyObject * args)
{
	OplistObject *o;
	
        PyObject * op1 = NULL, * op2 = NULL, * op3 = NULL, * op4 = NULL;
	if (!PyArg_ParseTuple(args, "|OOOO", &op1, &op2, &op3, &op4)) {
		return NULL;
	}
	o = newOplistObject(args);
	if ( o == NULL ) {
		return NULL;
	}
	o->ops = new oplist();
        addToOplist(op1, o);
        addToOplist(op2, o);
        addToOplist(op3, o);
        addToOplist(op4, o);
#if 0
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
#endif
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
	
	if (!PyArg_ParseTuple(args, "|s", &id)) {
            return NULL;
	}
        Object::MapType _omap;
        Object obj(_omap);
        Object::MapType & omap = obj.AsMap();
        if (id != NULL) {
            omap["id"] = string(id);
        }
        PyObject * keys = PyDict_Keys(kwds);
        PyObject * vals = PyDict_Values(kwds);
        if ((keys == NULL) || (vals == NULL)) {
            PyErr_SetString(PyExc_TypeError, "Error in keywords");
            return NULL;
        }
        int i, size=PyList_Size(keys); 
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
                    Py_DECREF(keys);
                    Py_DECREF(vals);
                    return NULL;
                }
                omap[key] = val_obj;
            }
        }
        Py_DECREF(keys);
        Py_DECREF(vals);
        
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
    } else if ((PyTypeObject*)PyObject_Type(ent) == &RootOperation_Type) {
        RootOperationObject * op = (RootOperationObject*)ent;
        if (op->operation == NULL) {
            fprintf(stderr, "Invalid operation in Operation arguments\n");
            return;
        }
        args.push_back(op->operation->AsObject());
    } else {
        fprintf(stderr, "Non-entity passed as arg to Operation()\n");
    }
}

static PyObject * operation_new(PyObject * self, PyObject * args, PyObject * kwds)
{
    RootOperationObject * op;

    char * type;
    PyObject * to = NULL;
    PyObject * from = NULL;
    PyObject * arg1 = NULL;
    PyObject * arg2 = NULL;
    PyObject * arg3 = NULL;

    if (!PyArg_ParseTuple(args, "s|OOO", &type, &arg1, &arg2, &arg3)) {
        return NULL;
    }
    op = newAtlasRootOperation(args);
    if (op == NULL) {
        return NULL;
    }
    if (strcmp(type, "tick") == 0) {
        op->operation = new Tick;
        *op->operation = Tick::Instantiate();
    } else if (strcmp(type, "sight") == 0) {
        op->operation = new Sight;
        *op->operation = Sight::Instantiate();
    } else if (strcmp(type, "set") == 0) {
        op->operation = new Set;
        *op->operation = Set::Instantiate();
    } else if (strcmp(type, "fire") == 0) {
        op->operation = new Fire;
        *op->operation = Fire::Instantiate();
    } else if (strcmp(type, "create") == 0) {
        op->operation = new Create;
        *op->operation = Create::Instantiate();
    } else if (strcmp(type, "setup") == 0) {
        op->operation = new Setup;
        *op->operation = Setup::Instantiate();
    } else if (strcmp(type, "look") == 0) {
        op->operation = new Look;
        *op->operation = Look::Instantiate();
    } else if (strcmp(type, "move") == 0) {
        op->operation = new Move;
        *op->operation = Move::Instantiate();
    } else if (strcmp(type, "talk") == 0) {
        op->operation = new Talk;
        *op->operation = Talk::Instantiate();
    } else if ((strcmp(type,"thought")==0) || (strcmp(type,"goal_info")==0)) {
        Py_DECREF(op);
        Py_INCREF(Py_None);
        return Py_None;
    } else {
        fprintf(stderr, "ERROR: PYTHON CREATING AN UNHANDLED %s OPERATION\n", type);
        //*op->operation = RootOperation::Instantiate();
        Py_DECREF(op);
        Py_INCREF(Py_None);
        return Py_None;
    }
    op->own = 1;
    if (PyMapping_HasKeyString(kwds, "to")) {
        to = PyMapping_GetItemString(kwds, "to");
        PyObject * to_id;
        if (PyString_Check(to)) {
            to_id = to;
        } else if ((to_id = PyObject_GetAttrString(to, "id")) == NULL) {
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
        PyObject * from_id;
        if (PyString_Check(from)) {
            from_id = from;
        } else if ((from_id = PyObject_GetAttrString(from, "id")) == NULL) {
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
        } else {
            value = def;
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
    {"isLocation", is_location,			METH_VARARGS},
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
    string pypath("");
    list<string>::const_iterator I;
    for(I = rulesets.begin(); I != rulesets.end(); I++) {
        pypath = pypath + install_directory + "/share/cyphesis/rulesets/" +
                 *I + ":";
    }
    setenv("PYTHONPATH", pypath.c_str(), 1);

    Py_Initialize();

    PyRun_SimpleString("from hooks import ruleset_import_hooks\n");
    PyRun_SimpleString("ruleset_import_hooks.install([\"acorn\",\"basic\"])\n");

    if (Py_InitModule("atlas", atlas_methods) == NULL) {
        fprintf(stderr, "Failed to Create atlas module\n");
        return;
    }

    if (Py_InitModule("Vector3D", Vector3D_methods) == NULL) {
        fprintf(stderr, "Failed to Create Vector3D module\n");
        return;
    }

    PyObject * misc;
    if ((misc = Py_InitModule("misc", misc_methods)) == NULL) {
        fprintf(stderr, "Failed to Create misc module\n");
        return;
    }

    PyObject * common;
    PyObject * dict;
    if ((common = Py_InitModule("common", common_methods)) == NULL) {
        fprintf(stderr, "Failed to Create common module\n");
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
        fprintf(stderr, "Failed to Create server thing\n");
        return;
    }
    dict = PyModule_GetDict(server);
    PyObject * dictlist = PyModule_New("dictlist");
    PyObject * add_value = (PyObject *)PyObject_NEW(FunctionObject, &dictlist_add_value_type);
    PyObject_SetAttrString(dictlist, "add_value", add_value);
    PyObject * remove_value = (PyObject *)PyObject_NEW(FunctionObject, &dictlist_remove_value_type);
    PyObject_SetAttrString(dictlist, "remove_value", remove_value);
    PyDict_SetItemString(dict, "dictlist", dictlist);
}
