#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Operation/Login.h>

#include <server/Admin.h>

#include "persistance.h"

#include <fstream.h>

namespace Persistance {

BaseEntity * load_admin_account()
{
    // Eventually this should actually load the account. For now it just
    // creates it.
    Admin * adm = new Admin(NULL, "admin", "test");
    ofstream adm_file("common/admin.xml", ios::out, 0600);
    adm_file << "<atlas>" << endl << "<map>" << endl;
    adm_file << "    <string name=\"password\">" << adm->password << "</string>" << endl;
    adm_file << "    <string name=\"id\">" << adm->fullid << "</string>" << endl;
    adm_file << "    <list name=\"parents\">" << endl;
    adm_file << "    <string>admin</string>" << endl;
    adm_file << "    </list>" << endl;
    adm_file << "</map>" << endl << "</atlas>" << endl << flush;
    adm_file.close();
    return(adm);
}

}
