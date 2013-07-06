// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2009 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA


#ifndef TOOLS_INTERACTIVE_H
#define TOOLS_INTERACTIVE_H

#include "AdminClient.h"

#include "common/OperationRouter.h"

#include <sigc++/trackable.h>

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

class ObjectContext;

typedef std::set<boost::shared_ptr<ObjectContext> > ContextMap;

/// \brief Class template for clients used to connect to and administrate
/// a cyphesis server.
class Interactive : public AdminClient,
                    virtual public sigc::trackable
{
  private:
    bool m_server_flag;
    std::string m_serverName;
    std::string m_systemType;
    std::string m_prompt;

    /// \brief Map of context with which we can interact with the server
    ContextMap m_contexts;
    boost::weak_ptr<ObjectContext> m_currentContext;

  protected:
    virtual void operation(const Operation &);

    virtual void appearanceArrived(const Operation &);
    virtual void disappearanceArrived(const Operation &);
    virtual void infoArrived(const Operation &);
    virtual void errorArrived(const Operation &);
    virtual void soundArrived(const Operation &);

    virtual void loginSuccess(const Atlas::Objects::Root & arg);

  public:
    Interactive();
    ~Interactive();

    int setup();
    void exec(const std::string & cmd, const std::string & arg);
    void loop();
    int select(bool rewrite_prompt = true);
    void updatePrompt();
    void runCommand(char *);
    void switchContext(int, int);
    const boost::shared_ptr<ObjectContext> & addContext(
          const boost::shared_ptr<ObjectContext> &);
    void addCurrentContext(const boost::shared_ptr<ObjectContext> &);

    static void gotCommand(char *);

    int commandUnknown(struct command *, const std::string &);
};

#endif // TOOLS_INTERACTIVE_H
