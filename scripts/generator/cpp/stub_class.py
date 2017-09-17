#!/usr/bin/env python
#
# Copyright 2008 Google Inc.  All Rights Reserved.
# Copyright 2017 Erik Ogenvik
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

"""Create Stub implementation files from headers.

Use this tool to create stub file used for testing. A ".h" header file is supplied
and an additional "*_custom.h" file is created, if it does not exists yet.
This allows "custom" behaviour to be added to the "*_custom.h" file, since it won't
be touched when regenerating the stubs.
"""

__author__ = 'nnorwitz@google.com (Neal Norwitz)'


import os
import re
import sys
import errno

from cpp import ast
from cpp import utils

# Preserve compatibility with Python 2.3.
try:
    _dummy = set
except NameError:
    import sets
    set = sets.Set

_VERSION = (1, 0, 1)  # The version of this script.
# How many spaces to indent.  Can set me with the INDENT environment variable.
_INDENT = 2

def buildTemplatedArgs(node):
    template_args = [buildTemplatedArgs(arg) for arg in node.templated_types]
    arg = ""
    if template_args:
        arg = node.name + '<' + ', '.join(template_args) + '>'
    else:
        arg = node.name
    if node.pointer:
        arg += "*"
    return arg


def _GenerateMethods(output_lines, source, class_node):
    ctor_or_dtor = ast.FUNCTION_CTOR | ast.FUNCTION_DTOR
    deleted_or_defaulted = ast.FUNCTION_DEFAULTED | ast.FUNCTION_DELETED
    indent = ' ' * _INDENT

    #Create Ctor instantiation list

    ctor_inits = []
    for node in class_node.body:
        if (isinstance(node, ast.VariableDeclaration)):
            if node.type.pointer and not 'static' in node.type.modifiers:
                ctor_inits.append("%s(nullptr)" % node.name)




    for node in class_node.body:

        if (isinstance(node, ast.Function)
            and not node.IsDefinition()
            and not node.modifiers & deleted_or_defaulted):
            # Pick out all the elements we need from the original function.
            const = ''
            methodName = node.name
            if node.modifiers & ast.FUNCTION_CONST:
                const = ' const'
            return_type = 'void'
            if node.modifiers & ctor_or_dtor:
                return_type = ''
            if node.modifiers & ast.FUNCTION_DTOR:
                methodName = "~" + methodName
            return_statement = ''
            if node.return_type:
                # Add modifiers like 'const'.
                modifiers = ''
                if node.return_type.modifiers:
                    # Ignore 'static'
                    modifiers = ' '.join([i for i in node.return_type.modifiers if i != 'static']) + ' '
                raw_return_type = buildTemplatedArgs(node.return_type)
                return_type = modifiers + raw_return_type
                if node.return_type.pointer:
                    return_statement = 'return nullptr;'
                elif node.return_type.name != 'void':
                    if node.return_type.name in ['int', 'unsigned int', 'long', 'unsigned long', 'size_t', 'float', 'double', 'short', 'unsigned short']:
                        return_statement = 'return 0;'
                    elif node.return_type.name == 'bool':
                        return_statement = 'return false;'
                    elif node.return_type.name in ['std::string', 'char*', 'const char*']:
                        if node.return_type.reference:
                            return_statement = 'static %s instance; return instance;' % raw_return_type
                        else:
                            return_statement = 'return "";'
                    elif node.return_type.name in ['std::vector', 'std::set', 'std::list', 'std::map']:
                        if node.return_type.reference:
                            return_statement = 'static %s instance; return instance;' % raw_return_type
                        else:
                            return_statement = 'return %s();' % raw_return_type
                    else:
                        return_statement = 'return *static_cast<%s*>(nullptr);' % (return_type)
                if node.return_type.reference:
                    return_type += '&'
                num_parameters = len(node.parameters)
                if len(node.parameters) == 1:
                    first_param = node.parameters[0]
                    if source[first_param.start:first_param.end].strip() == 'void':
                        # We must treat T(void) as a function with no parameters.
                        num_parameters = 0
            tmpl = ''
            if class_node.templated_types:
                tmpl = '_T'

            args = ''
            if node.parameters:
                # Remove any default values
                args = ', '.join('%s' % source[param.start:param.end].split('=')[0] for param in node.parameters)

            guard = 'STUB_%s_%s' % (class_node.name, node.name.replace('==', '_EQUALS').replace('[]', '_INDEX'))
            if node.modifiers & ast.FUNCTION_DTOR:
                guard += "_DTOR"

            # Create the mock method definition.
            output_lines.extend(['#ifndef %s' % guard])
            # Add a commented out line to allow for easily copying to the "custom" file if needed.
            output_lines.extend(['//#define %s' % guard])
            template_specifier = ""
            if class_node.templated_types and len(class_node.templated_types) > 0:
                output_lines.extend(['%stemplate <%s>' % (indent, ','.join('typename %s' % key for key in class_node.templated_types.keys()))])
                template_specifier = "<%s>" % ','.join('%s' % key for key in class_node.templated_types.keys())

            if node.modifiers & ast.FUNCTION_CTOR:
                output_lines.extend(['%s%s %s%s::%s(%s)%s' % (indent, return_type, class_node.name, template_specifier, methodName, args, const)])
                if class_node.bases is not None and len(class_node.bases) > 0:
                    output_lines.extend(['%s: %s(%s)' % (indent * 2, class_node.bases[0].name, ', '.join(param.name for param in node.parameters))])
                    if len(ctor_inits) != 0:
                        output_lines.extend(["%s, %s" % (indent * 2, ','.join(ctor_inits))])
                else:
                    if len(ctor_inits) != 0:
                        output_lines.extend(["%s: %s" % (indent * 2, ','.join(ctor_inits))])
                output_lines.extend(['%s{' % (indent), (indent * 2) + return_statement, '%s}' % (indent)])
            else:
                output_lines.extend(['%s%s %s%s::%s(%s)%s' % (indent, return_type, class_node.name, template_specifier, methodName, args, const), '%s{' % (indent), (indent * 2) + return_statement, '%s}' % (indent)])
            output_lines.extend(['#endif //%s' % guard, ''])


def _GenerateStubs(filename, source, ast_list):
    processed_class_names = set()
    lines = []

    filenameSegments = filename.split("/")

    customFileName = 'stub' + ''.join(filenameSegments[-1].split('.')[0:-1]) + '_custom.h'

    guard = "STUB_" + filename.upper().replace('.', '_').replace('/', '_')

    lines.append("// AUTOGENERATED file, created by the tool generate_stub.py, don't edit!")
    lines.append("// If you want to add your own functionality, instead edit the %s file." % customFileName)
    lines.append("")
    lines.append("#include \"%s\"" % (filename))
    lines.append("#include \"%s\"" % (customFileName))
    lines.append("")
    lines.append("#ifndef %s" % guard)
    lines.append("#define %s" % guard)
    lines.append("")

    for node in ast_list:
        if (isinstance(node, ast.Class) and node.body):
            class_name = node.name
            parent_name = class_name
            processed_class_names.add(class_name)
            class_node = node
            # Add namespace before the class.
            if class_node.namespace:
                lines.extend(['namespace %s {' % n for n in class_node.namespace])  # }
                lines.append('')

            # Add all the methods.
            _GenerateMethods(lines, source, class_node)

            # Close the class.
            if lines:
                # Only close the class if there really is a class.
                lines.append('')  # Add an extra newline.

            # Close the namespace.
            if class_node.namespace:
                for i in range(len(class_node.namespace)-1, -1, -1):
                    lines.append('}  // namespace %s' % class_node.namespace[i])
                lines.append('')  # Add an extra newline.

    if not processed_class_names:
        sys.stderr.write('No class found in %s\n' % filename)

    lines.append("#endif")

    return lines


def main(argv=sys.argv):
    if len(argv) < 3:
        sys.stderr.write('Cyphesis Stub Class Generator v%s\n\n' %
                         '.'.join(map(str, _VERSION)))
        sys.stderr.write(__doc__)
        return 1


    global _INDENT
    try:
        _INDENT = int(os.environ['INDENT'])
    except KeyError:
        pass
    except:
        sys.stderr.write('Unable to use indent of %s\n' % os.environ.get('INDENT'))

    filename = argv[1]
    stubdirectory = argv[2]
    source = utils.ReadFile(filename)
    if source is None:
        return 1

    builder = ast.BuilderFromSource(source, filename)
    try:
        entire_ast = filter(None, builder.Generate())
    except KeyboardInterrupt:
        return
    except:
        # An error message was already printed since we couldn't parse.
        sys.exit(1)
    else:

        filenameSegments = filename.split("/")
        customFileName = 'stub' + ''.join(filenameSegments[-1].split('.')[0:-1]) + '_custom.h'

        if len(filenameSegments) > 1:
            stubpath = stubdirectory + '/' + '/'.join(filenameSegments[0:-1]) + "/stub" + filenameSegments[-1]
            stubCustomPath = stubdirectory + '/' + '/'.join(filenameSegments[0:-1]) + "/" + customFileName
        else:
            stubpath = stubdirectory + "/stub" + filenameSegments[-1]
            stubCustomPath = stubdirectory + "/" + customFileName

        try:
            os.makedirs(os.path.dirname(stubpath))
        except OSError as exc:  # Python >2.5
            if exc.errno == errno.EEXIST and os.path.isdir(os.path.dirname(stubpath)):
                pass
            else:
                raise


        lines = _GenerateStubs(filename, source, entire_ast)
        with open(stubpath, 'w') as stubFile:
            stubFile.write('\n'.join(lines))
        sys.stdout.write("Wrote to %s\n" % stubpath)

        if not os.path.isfile(stubCustomPath):
            with open(stubCustomPath, 'w') as stubCustomFile:
                stubCustomFile.write('//Add custom implementations of stubbed functions here; this file won\'t be rewritten when re-generating stubs.\n')
            sys.stdout.write("Also created empty custom stub file at %s\n" % stubCustomPath)


#        sys.stdout.write('\n'.join(lines))


if __name__ == '__main__':
    main(sys.argv)
