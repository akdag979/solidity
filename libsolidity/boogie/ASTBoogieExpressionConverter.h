#pragma once

#include <libsolidity/ast/ASTVisitor.h>
#include <libsolidity/boogie/BoogieAst.h>
#include <libsolidity/boogie/BoogieContext.h>

namespace dev
{
namespace solidity
{

/**
 * Converter from Solidity expressions to Boogie expressions.
 */
class ASTBoogieExpressionConverter : private ASTConstVisitor
{
private:

	BoogieContext& m_context;
	bool m_insideSpec; // Indicates if a specification annotation is being parsed

	// Helper variables to pass information between the visit methods
	boogie::Expr::Ref m_currentExpr;
	boogie::Expr::Ref m_currentAddress;
	boogie::Expr::Ref m_currentMsgValue;
	bool m_isGetter;
	TypePointer m_getterVarType;
	bool m_isLibraryCall;
	bool m_isLibraryCallStatic;

	// As a side effect, converting an expression can yield extra stuff
	// due to the differences between Solidity and Boogie expressions
	std::vector<boogie::Stmt::Ref> m_newStatements; // New statements
	std::list<boogie::Decl::Ref> m_newDecls; // New declarations
	std::list<boogie::Expr::Ref> m_tccs; // Type checking conditions
	std::list<boogie::Expr::Ref> m_ocs; // Overflow conditions

	/**
	 * Helper method to add a type checking condition for an expression with a given type.
	 * @param expr Boogie expression
	 * @param tp Type of the expression
	 */
	void addTCC(boogie::Expr::Ref expr, TypePointer tp);

	/** Helper method to add a side effect (statement) */
	void addSideEffect(boogie::Stmt::Ref stmt);

	/** Helper method to add side effects (statements) */
	void addSideEffects(std::vector<boogie::Stmt::Ref> const& stmts) { for (auto stmt: stmts) addSideEffect(stmt); }

	// Helper methods for the different scenarios for function calls
	void functionCallConversion(FunctionCall const& _node);
	void functionCallNewStruct(StructDefinition const* structDef, std::vector<boogie::Expr::Ref> const& args);
	void functionCallReduceBalance(boogie::Expr::Ref msgValue);
	void functionCallRevertBalance(boogie::Expr::Ref msgValue);
	void functionCallSum(FunctionCall const& _node, boogie::Expr::Ref arg);
	void functionCallOld(FunctionCall const& _node, std::vector<boogie::Expr::Ref> const& args);
	void functionCallEq(FunctionCall const& _node, std::vector<boogie::Expr::Ref> const& args);
	void functionCallNewArray(FunctionCall const& _node);
	void functionCallPushPop(MemberAccess const* memAccExpr, ArrayType const* arrType, FunctionCall const& _node);

public:

	/**
	 * Result of converting a Solidity expression to a Boogie expression.
	 * Due to differences between Solidity and Boogie, the result might
	 * introduce new statements and declarations as well.
	 */
	struct Result
	{
		boogie::Expr::Ref expr;
		std::vector<boogie::Stmt::Ref> newStatements;
		std::list<boogie::Decl::Ref> newDecls;
		std::list<boogie::Expr::Ref> tccs; // Type checking conditions
		std::list<boogie::Expr::Ref> ocs;  // Overflow conditions

		Result(boogie::Expr::Ref expr,
				std::vector<boogie::Stmt::Ref> const& newStatements,
				std::list<boogie::Decl::Ref> const& newDecls,
				std::list<boogie::Expr::Ref> const& tccs,
				std::list<boogie::Expr::Ref> const& ocs)
			:expr(expr), newStatements(newStatements), newDecls(newDecls), tccs(tccs), ocs(ocs) {}
	};

	/**
	 * Create a new instance with a given context.
	 */
	ASTBoogieExpressionConverter(BoogieContext& context);

	/**
	 * Convert a Solidity Expression into a Boogie expression.
	 * @param specification Indicates if a specification annotation is converted
	 * @returns Converted expression and side effects
	 */
	Result convert(Expression const& _node, bool isSpecification);

	// Only need to handle expressions
	bool visit(Conditional const& _node) override;
	bool visit(Assignment const& _node) override;
	bool visit(TupleExpression const& _node) override;
	bool visit(UnaryOperation const& _node) override;
	bool visit(BinaryOperation const& _node) override;
	bool visit(FunctionCall const& _node) override;
	bool visit(NewExpression const& _node) override;
	bool visit(MemberAccess const& _node) override;
	bool visit(IndexAccess const& _node) override;
	bool visit(Identifier const& _node) override;
	bool visit(ElementaryTypeNameExpression const& _node) override;
	bool visit(Literal const& _node) override;

	bool visitNode(ASTNode const&) override;

};

}
}
