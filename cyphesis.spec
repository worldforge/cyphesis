%define prefix /usr

Summary: A simple personal server for the WorldForge project
Name: cyphesis
Version: 0.0.2
Release: 1
Copyright: GPL
Group: Amusements/Games
Source: http://ftp.worldforge.org/pub/WorldForge/cyphesis/source/cyphesis-0.0.11.tar.gz
BuildRoot: /var/tmp/%{name}-buildroot

%description
Cyphesis is a very simple world simulator. NPCs that do things according to
rules. They have minds with simple input and output. They can
use/move/make things and have simple discussion. They can deduce
simple things (like where I can get these things and where I should
be). They have simple memory and use it too. They can have goals (like
build home for me or go to dinner).

%prep
%setup -q

%build

if [ -x ./configure ]; then
  CFLAGS=$RPM_OPT_FLAGS ./configure --prefix=%{prefix}
else 
  CFLAGS=$RPM_OPT_FLAGS ./autogen.sh --prefix=%{prefix}
fi
make


%install
rm -rf $RPM_BUILD_ROOT
make prefix=$RPM_BUILD_ROOT%{prefix} install

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%{prefix}/bin/cyphesis
%{prefix}/share/cyphesis/basic.vconf
%{prefix}/share/cyphesis/acorn.vconf
%{prefix}/share/cyphesis/cyphesis.vconf
%{prefix}/share/cyphesis/rulesets/acorn/*.py
%{prefix}/share/cyphesis/rulesets/acorn/mind/*.py
%{prefix}/share/cyphesis/rulesets/acorn/world/*.py
%{prefix}/share/cyphesis/rulesets/acorn/world/objects/*.py
%{prefix}/share/cyphesis/rulesets/acorn/world/objects/tools/*.py
%{prefix}/share/cyphesis/rulesets/acorn/world/objects/plants/*.py
%{prefix}/share/cyphesis/rulesets/acorn/world/objects/plants/seeds/*.py
%{prefix}/share/cyphesis/rulesets/acorn/world/objects/undead/*.py
%{prefix}/share/cyphesis/rulesets/acorn/world/objects/animals/*.py
%{prefix}/share/cyphesis/rulesets/acorn/world/objects/outdoor/*.py
%{prefix}/share/cyphesis/rulesets/acorn/world/objects/buildings/*.py
%{prefix}/share/cyphesis/rulesets/basic/*.py
%{prefix}/share/cyphesis/rulesets/basic/mind/*.py
%{prefix}/share/cyphesis/rulesets/basic/mind/goals/*.py
%{prefix}/share/cyphesis/rulesets/basic/mind/goals/animal/*.py
%{prefix}/share/cyphesis/rulesets/basic/mind/goals/common/*.py
%{prefix}/share/cyphesis/rulesets/basic/mind/goals/dynamic/*.py
%{prefix}/share/cyphesis/rulesets/basic/mind/goals/humanoid/*.py
%{prefix}/share/cyphesis/rulesets/basic/mind/panlingua/*.py
%{prefix}/share/cyphesis/rulesets/basic/hooks/*.py
%{prefix}/share/cyphesis/rulesets/basic/world/*.py
%{prefix}/share/cyphesis/rulesets/basic/world/objects/*.py
%{prefix}/share/cyphesis/rulesets/basic/world/objects/food/*.py
%{prefix}/share/cyphesis/rulesets/basic/world/objects/tools/*.py
%{prefix}/share/cyphesis/rulesets/basic/world/objects/plants/*.py
%{prefix}/share/cyphesis/rulesets/basic/world/objects/plants/seeds/*.py
%{prefix}/share/cyphesis/rulesets/basic/world/objects/undead/*.py
%{prefix}/share/cyphesis/rulesets/basic/world/objects/animals/*.py
%{prefix}/share/cyphesis/rulesets/basic/world/objects/elements/*.py
%{prefix}/share/cyphesis/rulesets/basic/world/objects/buildings/*.py
%{prefix}/share/cyphesis/rulesets/basic/world/objects/materials/*.py
%{prefix}/share/cyphesis/rulesets/basic/world/objects/characters/*.py
