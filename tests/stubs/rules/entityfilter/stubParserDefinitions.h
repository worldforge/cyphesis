// AUTOGENERATED file, created by the tool generate_stub.py, don't edit!
// If you want to add your own functionality, instead edit the stubParserDefinitions_custom.h file.

#ifndef STUB_RULES_ENTITYFILTER_PARSERDEFINITIONS_H
#define STUB_RULES_ENTITYFILTER_PARSERDEFINITIONS_H

#include "rules/entityfilter/ParserDefinitions.h"
#include "stubParserDefinitions_custom.h"

namespace EntityFilter {
namespace parser {


}  // namespace parser
}  // namespace EntityFilter

namespace EntityFilter {
namespace parser {

#ifndef STUB_query_parser_query_parser
//#define STUB_query_parser_query_parser
  template <typename Iterator>
   query_parser<Iterator>::query_parser(const ProviderFactory& factory)
    : qi::grammar(factory)
  {
    
  }
#endif //STUB_query_parser_query_parser


}  // namespace parser
}  // namespace EntityFilter

#endif