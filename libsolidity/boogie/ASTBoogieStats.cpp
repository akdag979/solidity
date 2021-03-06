#include <boost/algorithm/string/predicate.hpp>
#include <libsolidity/boogie/ASTBoogieStats.h>
#include <libsolidity/boogie/ASTBoogieUtils.h>

using namespace dev;
using namespace dev::solidity;

namespace dev
{
namespace solidity
{

bool ASTBoogieStats::hasDocTag(DocumentedAnnotation const& _annot, std::string _tag) const
{
	for (auto docTag: _annot.docTags)
		if (docTag.first == "notice" && boost::starts_with(docTag.second.content, _tag)) // Find expressions with the given tag
			return true;

	return false;
}

bool ASTBoogieStats::visit(ContractDefinition const& _node)
{
	m_allContracts.push_back(&_node);
	return true;
}

bool ASTBoogieStats::visit(FunctionDefinition const& _node)
{
	if (!m_hasModifiesSpecs)
	{
		m_hasModifiesSpecs = m_hasModifiesSpecs || hasDocTag(_node.annotation(), ASTBoogieUtils::DOCTAG_MODIFIES);
		m_hasModifiesSpecs = m_hasModifiesSpecs || hasDocTag(_node.annotation(), ASTBoogieUtils::DOCTAG_MODIFIES_ALL);
	}
	return true;
}

}

}
