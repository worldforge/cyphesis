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

#include <memory>

class ObjectContext;

typedef std::set<std::shared_ptr<ObjectContext> > ContextMap;

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
    std::weak_ptr<ObjectContext> m_currentContext;

  protected:
    void operation(const Operation &) override;
    void appearanceArrived(const Operation &) override;
    void disappearanceArrived(const Operation &) override;
    void infoArrived(const Operation &) override;
    void errorArrived(const Operation &) override;
    void soundArrived(const Operation &) override;
    void loginSuccess(const Atlas::Objects::Root & arg) override;

  public:
    explicit Interactive(Atlas::Objects::Factories& factories, boost::asio::io_context& io_context);
    ~Interactive() override;

    int setup();
    void exec(const std::string & cmd, const std::string & arg);
    void loop();
    int select(bool rewrite_prompt = true);
    void updatePrompt();
    void runCommand(char *);
    void switchContext(int, int);
    const std::shared_ptr<ObjectContext> & addContext(
          const std::shared_ptr<ObjectContext> &);
    void addCurrentContext(const std::shared_ptr<ObjectContext> &);

    static void gotCommand(char *);

    int commandUnknown(struct command *, const std::string &);
};

#endif // TOOLS_INTERACTIVE_H
