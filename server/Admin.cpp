#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Operation/Login.h>

#include "Admin.h"

//File::File()
//{
    //File::str="";
//}
//
//bad_type File::write(bad_type str)
//{
    //File::str=File::str+str;
//}
//

RootOperation * Admin::Operation(const Save & op)
{
    //filename = op[0].id;
    //if (Admin::server.id_dict.has_key(op.to.id)) {
        //Admin::server.id_dict[op.to.id].save(filename);
    //}
    return(NULL);
}

RootOperation * Admin::Operation(const Load & op)
{
    //filename = op[0].id;
    //if (Admin::server.id_dict.has_key(op.to.id)) {
        //Admin::server.id_dict[op.to.id].load(filename);
    //}
    return(NULL);
}

#if 0 
// If this ever works, I will be more surprised than a very surprised thing
bad_type Admin::code_operation(bad_type op)
{
    ent=op[0];
    try {
        if (ent.code_type=='eval') {
            res=`Admin::server.world.eval_code(ent.code)`;
        }
        else {
            Admin::server.world.execute_code(ent.code);
            res='';
        }
    }
    catch () {
        f=File();
        traceback.print_exc(None,f);
        res=f.str;
    }
    res=string.replace(res,"\n","[newline]");
    return Operation("info",Entity(code=ent.code,result=res),refno=op.no);
}
#endif
