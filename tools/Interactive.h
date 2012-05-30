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

// $Id$

#ifndef TOOLS_INTERACTIVE_H
#define TOOLS_INTERACTIVE_H

#include "AdminClient.h"

#include "common/OperationRouter.h"

#include <sigc++/trackable.h>

#include <boost/enable_shared_from_this.hpp>

class ObjectContext : public boost::enable_shared_from_this<ObjectContext>
{
  public:
    virtual bool accept(const Atlas::Objects::Operation::RootOperation&) const = 0;
    virtual int dispatch(const Atlas::Objects::Operation::RootOperation&) = 0;
    virtual std::string repr() const = 0;

};

class AccountContext : public ObjectContext
{
  protected:
    const std::string m_username;
    std::string m_id;
    long m_refNo;
  public:
    AccountContext(const std::string & u);
    virtual bool accept(const Atlas::Objects::Operation::RootOperation&) const;
    virtual int dispatch(const Atlas::Objects::Operation::RootOperation&);
    virtual std::string repr() const;
};

typedef std::map<std::string, boost::shared_ptr<ObjectContext> > ContextMap;

/// \brief Class template for clients used to connect to and administrate
/// a cyphesis server.
class Interactive : public AdminClient,
                    virtual public sigc::trackable
{
  private:
    bool m_avatar_flag, m_server_flag, m_juncture_flag;
    std::string m_agentId;
    std::string m_juncture_id;
    std::string m_serverName;
    std::string m_systemType;
    std::string m_prompt;
    bool m_exit_flag;

    /// \brief Map of context with which we can interact with the server
    ContextMap m_contexts;

  protected:
    virtual void operation(const Operation &);

    virtual void appearanceArrived(const Operation &);
    virtual void disappearanceArrived(const Operation &);
    virtual void infoArrived(const Operation &);
    virtual void errorArrived(const Operation &);
    virtual void sightArrived(const Operation &);
    virtual void soundArrived(const Operation &);

    virtual void loginSuccess(const Atlas::Objects::Root & arg);

  public:
    Interactive();
    ~Interactive();

    int setup();
    void exec(const std::string & cmd, const std::string & arg);
    void loop();
    void select(bool rewrite_prompt = true);
    void updatePrompt();
    void runCommand(char *);
    void switchContext(int, int);

    static void gotCommand(char *);

    int commandUnknown(const std::string &, const std::string &);
};

#endif // TOOLS_INTERACTIVE_H
