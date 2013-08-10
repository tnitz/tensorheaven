// ///////////////////////////////////////////////////////////////////////////
// tenh/expression_templates.hpp by Victor Dods, created 2013/03/13
// Copyright Leap Motion Inc.
// ///////////////////////////////////////////////////////////////////////////

#ifndef TENH_EXPRESSION_TEMPLATES_HPP_
#define TENH_EXPRESSION_TEMPLATES_HPP_

#include <stdexcept>

#include "tenh/core.hpp"

#include "tenh/expression_templates_utility.hpp"
#include "tenh/interface/expressiontemplate.hpp"

namespace Tenh {

// ////////////////////////////////////////////////////////////////////////////
// expression-template-generation (making ETs from vectors/tensors)
// ////////////////////////////////////////////////////////////////////////////

static bool const FORCE_CONST = true;
static bool const DONT_FORCE_CONST = false;

static bool const CHECK_FOR_ALIASING = true;
static bool const DONT_CHECK_FOR_ALIASING = false;

// this is the "const" version of an indexed tensor expression (it has summed indices, so it doesn't make sense to assign to it)
template <typename Object,
          typename FactorTypeList_, // this is necessary because the factor type depends on if the thing is being indexed as a vector or tensor
          typename DimIndexTypeList,
          typename SummedDimIndexTypeList_,
          bool FORCE_CONST_,
          bool CHECK_FOR_ALIASING_,
          typename Derived_ = NullType>
struct ExpressionTemplate_IndexedObject_t
    :
    public ExpressionTemplate_i<typename DerivedType_t<Derived_,
                                                       ExpressionTemplate_IndexedObject_t<Object,
                                                                                          FactorTypeList_,
                                                                                          DimIndexTypeList,
                                                                                          SummedDimIndexTypeList_,
                                                                                          FORCE_CONST_,
                                                                                          CHECK_FOR_ALIASING_,
                                                                                          Derived_> >::T,
                                typename Object::Scalar,
                                FactorTypeList_,
                                typename FreeIndexTypeList_t<DimIndexTypeList>::T,
                                SummedDimIndexTypeList_>
{
    enum
    {
        // TODO: assert that FactorTypeList is a TypeList of BasedVectorSpace_c types.
        STATIC_ASSERT_IN_ENUM__UNIQUE(EachTypeIsADimIndex_t<DimIndexTypeList>::V, MUST_BE_TYPELIST_OF_DIM_INDEX_TYPES, DIMINDEXTYPELIST),
        STATIC_ASSERT_IN_ENUM__UNIQUE(EachTypeIsADimIndex_t<SummedDimIndexTypeList_>::V, MUST_BE_TYPELIST_OF_DIM_INDEX_TYPES, SUMMEDDIMINDEXTYPELIST)
    };

    typedef ExpressionTemplate_i<typename DerivedType_t<Derived_,
                                                        ExpressionTemplate_IndexedObject_t<Object,
                                                                                           FactorTypeList_,
                                                                                           DimIndexTypeList,
                                                                                           SummedDimIndexTypeList_,
                                                                                           FORCE_CONST_,
                                                                                           CHECK_FOR_ALIASING_,
                                                                                           Derived_> >::T,
                                 typename Object::Scalar,
                                 FactorTypeList_,
                                 typename FreeIndexTypeList_t<DimIndexTypeList>::T,
                                 SummedDimIndexTypeList_> Parent;
    typedef typename Parent::Derived Derived;
    typedef typename Parent::Scalar Scalar;
    typedef typename Parent::FactorTypeList FactorTypeList;
    typedef typename Parent::FreeDimIndexTypeList FreeDimIndexTypeList;
    typedef typename Parent::UsedDimIndexTypeList UsedDimIndexTypeList;
    typedef typename Parent::MultiIndex MultiIndex;
    using Parent::IS_EXPRESSION_TEMPLATE_I; // TODO: deprecate
    typedef SummedDimIndexTypeList_ SummedDimIndexTypeList;

    ExpressionTemplate_IndexedObject_t (Object const &object) : m_object(object) { }

    operator Scalar () const
    {
        STATIC_ASSERT_TYPELIST_IS_EMPTY(FreeDimIndexTypeList);
        return operator[](MultiIndex());
    }

    // read-only, because it doesn't make sense to assign to an expression which is a summation.
    Scalar operator [] (MultiIndex const &m) const
    {
        return UnarySummation_t<Object,DimIndexTypeList,SummedDimIndexTypeList>::eval(m_object, m);
    }

    template <typename OtherTensor>
    bool uses_tensor (OtherTensor const &t) const
    {
        // the reinterpret_cast is safe because we're dealing with POD types and there
        // is an explicit type-check at compiletime (TypesAreEqual)
        return Lvd::Meta::TypesAreEqual<OtherTensor,Object>::v && reinterpret_cast<Object const *>(&t) == &m_object;
    }

private:

    Object const &m_object;
};

// this is the "non-const" version of an indexed tensor expression (it has no summed indices, so it makes sense to assign to it)
template <typename Object, 
          typename FactorTypeList_, 
          typename DimIndexTypeList, 
          bool CHECK_FOR_ALIASING_, 
          typename Derived_>
struct ExpressionTemplate_IndexedObject_t<Object,FactorTypeList_,DimIndexTypeList,EmptyTypeList,DONT_FORCE_CONST,CHECK_FOR_ALIASING_,Derived_>
    :
    public ExpressionTemplate_i<ExpressionTemplate_IndexedObject_t<Object,FactorTypeList_,DimIndexTypeList,EmptyTypeList,DONT_FORCE_CONST,CHECK_FOR_ALIASING_,Derived_>,
                                typename Object::Scalar,
                                FactorTypeList_,
                                typename FreeIndexTypeList_t<DimIndexTypeList>::T,
                                EmptyTypeList>
{
    typedef ExpressionTemplate_i<ExpressionTemplate_IndexedObject_t<Object,FactorTypeList_,DimIndexTypeList,EmptyTypeList,DONT_FORCE_CONST,CHECK_FOR_ALIASING_,Derived_>,
                                 typename Object::Scalar,
                                 FactorTypeList_,
                                 typename FreeIndexTypeList_t<DimIndexTypeList>::T,
                                 EmptyTypeList> Parent;
    typedef typename Parent::Derived Derived;
    typedef typename Parent::Scalar Scalar;
    typedef typename Parent::FactorTypeList FactorTypeList;
    typedef typename Parent::FreeDimIndexTypeList FreeDimIndexTypeList;
    typedef typename Parent::UsedDimIndexTypeList UsedDimIndexTypeList;
    typedef typename Parent::MultiIndex MultiIndex;
    using Parent::IS_EXPRESSION_TEMPLATE_I;

    ExpressionTemplate_IndexedObject_t (Object &object) : m_object(object) { }

    // call this on the left-hand side (LHS) of an indexed assignment to avoid the run-time
    // aliasing check.  this should only be done when the human can guarantee that there is
    // no memory aliasing in the assignment (where the same memory location is being referenced
    // on both the LHS and RHS of the assignment, therefore causing the non-atomically
    // evaluated result to be implementation-dependent and incorrect).
    ExpressionTemplate_IndexedObject_t<Object,FactorTypeList,DimIndexTypeList,EmptyTypeList,DONT_FORCE_CONST,DONT_CHECK_FOR_ALIASING,Derived_> no_alias ()
    {
        return ExpressionTemplate_IndexedObject_t<Object,FactorTypeList,DimIndexTypeList,EmptyTypeList,DONT_FORCE_CONST,DONT_CHECK_FOR_ALIASING,Derived_>(m_object);
    }

    operator Scalar () const
    {
        STATIC_ASSERT_TYPELIST_IS_EMPTY(FreeDimIndexTypeList);
        return operator[](MultiIndex());
    }

    // read-only, because it doesn't necessarily make sense to assign to an expression
    // template -- the expression may be a product or some such, where each component
    // is not an L-value.
    Scalar operator [] (MultiIndex const &m) const { return m_object[m]; }

    // for some dumb reason, the compiler needed a non-templatized assignment operator for the exact matching type
    void operator = (ExpressionTemplate_IndexedObject_t const &right_operand)
    {
        // if right and left operands' m_tensor references are the same, this is a no-op
        if (&right_operand.m_object == &m_object)
            return;

        // TODO: replace with memcpy? (this would require that Scalar is a POD type)
        for (typename Object::ComponentIndex i; i.is_not_at_end(); ++i)
            m_object[i] = right_operand[i];
    }
    template <typename RightOperand>
    void operator = (RightOperand const &right_operand)
    {
        enum
        {
            STATIC_ASSERT_IN_ENUM(RightOperand::IS_EXPRESSION_TEMPLATE_I, RIGHT_OPERAND_IS_EXPRESSION_TEMPLATE),
            STATIC_ASSERT_IN_ENUM((Lvd::Meta::TypesAreEqual<Scalar,typename RightOperand::Scalar>::v), OPERAND_SCALAR_TYPES_ARE_EQUAL),
            STATIC_ASSERT_IN_ENUM((AreEqualAsSets_t<FreeDimIndexTypeList,typename RightOperand::FreeDimIndexTypeList>::V),OPERANDS_HAVE_SAME_FREE_INDICES),
            STATIC_ASSERT_IN_ENUM((!ContainsDuplicates_t<FreeDimIndexTypeList>::V), LEFT_OPERAND_HAS_NO_DUPLICATE_FREE_INDICES),
            STATIC_ASSERT_IN_ENUM((!ContainsDuplicates_t<typename RightOperand::FreeDimIndexTypeList>::V), RIGHT_OPERAND_HAS_NO_DUPLICATE_FREE_INDICES)
        };

        // check for aliasing (where source and destination memory overlap)
        if (CHECK_FOR_ALIASING_ && right_operand.uses_tensor(m_object))
            throw std::invalid_argument("aliased tensor assignment (source and destination memory overlap) -- "
                                        "see evaluse eval() on RHS or noalias() on LHS of assignment");

        typedef MultiIndexMap_t<FreeDimIndexTypeList,typename RightOperand::FreeDimIndexTypeList> RightOperandIndexMap;
        typename RightOperandIndexMap::EvalMapType right_operand_index_map = RightOperandIndexMap::eval;

        // component-wise assignment via the free index type.
        for (MultiIndex m; m.is_not_at_end(); ++m)
            m_object[m] = right_operand[right_operand_index_map(m)];
    }

    template <typename OtherTensor>
    bool uses_tensor (OtherTensor const &t) const
    {
        // the reinterpret_cast is safe because we're dealing with POD types and there
        // is an explicit type-check at compiletime (TypesAreEqual)
        return Lvd::Meta::TypesAreEqual<OtherTensor,Object>::v && reinterpret_cast<Object const *>(&t) == &m_object;
    }

private:

    Object &m_object;
};

// ////////////////////////////////////////////////////////////////////////////
// addition of expression templates
// ////////////////////////////////////////////////////////////////////////////

// NOTE: if this is ever subclassed, then it will be necessary to change the inheritance to pass in the Derived type
template <typename LeftOperand, typename RightOperand, char OPERATOR>
struct ExpressionTemplate_Addition_t
    :
    public ExpressionTemplate_i<ExpressionTemplate_Addition_t<LeftOperand,RightOperand,OPERATOR>,
                                typename LeftOperand::Scalar,
                                typename LeftOperand::FactorTypeList,
                                typename LeftOperand::FreeDimIndexTypeList,
                                EmptyTypeList>
{
    typedef ExpressionTemplate_i<ExpressionTemplate_Addition_t<LeftOperand,RightOperand,OPERATOR>,
                                 typename LeftOperand::Scalar,
                                 typename LeftOperand::FactorTypeList,
                                 typename LeftOperand::FreeDimIndexTypeList,
                                 EmptyTypeList> Parent;
    typedef typename Parent::Derived Derived;
    typedef typename Parent::Scalar Scalar;
    typedef typename Parent::FactorTypeList FactorTypeList;
    typedef typename Parent::FreeDimIndexTypeList FreeDimIndexTypeList;
    typedef typename Parent::UsedDimIndexTypeList UsedDimIndexTypeList;
    typedef typename Parent::MultiIndex MultiIndex;
    using Parent::IS_EXPRESSION_TEMPLATE_I;

    // TODO: check that the summed indices from each operand have no indices in common
    // though technically this is unnecessary, because the summed indices are "private"
    // to each contraction, so this is really for the human's benefit, not getting
    // confused by multiple repeated indices that have nothing to do with each other.
    // NOTE: technically this check is already done inside MultiIndex_t, but it would
    // be good to do the check here so that an error will be more obvious.
    enum
    {
        STATIC_ASSERT_IN_ENUM(LeftOperand::IS_EXPRESSION_TEMPLATE_I, LEFT_OPERAND_IS_EXPRESSION_TEMPLATE),
        STATIC_ASSERT_IN_ENUM(RightOperand::IS_EXPRESSION_TEMPLATE_I, RIGHT_OPERAND_IS_EXPRESSION_TEMPLATE),
        STATIC_ASSERT_IN_ENUM((Lvd::Meta::TypesAreEqual<typename LeftOperand::Scalar,typename RightOperand::Scalar>::v), OPERAND_SCALAR_TYPES_ARE_EQUAL),
        STATIC_ASSERT_IN_ENUM((Lvd::Meta::TypesAreEqual<typename LeftOperand::FactorTypeList,typename RightOperand::FactorTypeList>::v), OPERANDS_HAVE_SAME_FACTORS),
        STATIC_ASSERT_IN_ENUM((AreEqualAsSets_t<typename LeftOperand::FreeDimIndexTypeList,typename RightOperand::FreeDimIndexTypeList>::V), OPERANDS_HAVE_SAME_FREE_INDICES),
        STATIC_ASSERT_IN_ENUM(!ContainsDuplicates_t<typename LeftOperand::FreeDimIndexTypeList>::V, LEFT_OPERAND_HAS_NO_DUPLICATE_FREE_INDICES),
        STATIC_ASSERT_IN_ENUM(!ContainsDuplicates_t<typename RightOperand::FreeDimIndexTypeList>::V, RIGHT_OPERAND_HAS_NO_DUPLICATE_FREE_INDICES),
        STATIC_ASSERT_IN_ENUM((OPERATOR == '+' || OPERATOR == '-'), OPERATOR_IS_PLUS_OR_MINUS)
    };

    ExpressionTemplate_Addition_t (LeftOperand const &left_operand, RightOperand const &right_operand)
        :
        m_left_operand(left_operand),
        m_right_operand(right_operand)
    { }

    operator Scalar () const
    {
        STATIC_ASSERT_TYPELIST_IS_EMPTY(FreeDimIndexTypeList);
        if (OPERATOR == '+')
            return m_left_operand.operator Scalar() + m_right_operand.operator Scalar();
        else // OPERATOR == '-'
            return m_left_operand.operator Scalar() - m_right_operand.operator Scalar();
    }

    // read-only, because it doesn't make sense to assign to an expression which is a summation.
    Scalar operator [] (MultiIndex const &m) const
    {
        typedef MultiIndexMap_t<FreeDimIndexTypeList,typename LeftOperand::FreeDimIndexTypeList> LeftOperandIndexMap;
        typedef MultiIndexMap_t<FreeDimIndexTypeList,typename RightOperand::FreeDimIndexTypeList> RightOperandIndexMap;
        typename LeftOperandIndexMap::EvalMapType left_operand_index_map = LeftOperandIndexMap::eval;
        typename RightOperandIndexMap::EvalMapType right_operand_index_map = RightOperandIndexMap::eval;
        if (OPERATOR == '+')
            return m_left_operand[left_operand_index_map(m)] + m_right_operand[right_operand_index_map(m)];
        else // OPERATOR == '-'
            return m_left_operand[left_operand_index_map(m)] - m_right_operand[right_operand_index_map(m)];
    }

    template <typename OtherTensor>
    bool uses_tensor (OtherTensor const &t) const
    {
        return m_left_operand.uses_tensor(t) || m_right_operand.uses_tensor(t);
    }

private:

    LeftOperand const &m_left_operand;
    RightOperand const &m_right_operand;
};

// ////////////////////////////////////////////////////////////////////////////
// scalar multiplication and division of expression templates
// ////////////////////////////////////////////////////////////////////////////

// it is assumed that scalar multiplication is commutative.
// OPERATOR can be '*' or '/'.
// NOTE: if this is ever subclassed, then it will be necessary to change the inheritance to pass in the Derived type
template <typename Operand, typename Scalar_, char OPERATOR>
struct ExpressionTemplate_ScalarMultiplication_t
    :
    public ExpressionTemplate_i<ExpressionTemplate_ScalarMultiplication_t<Operand,Scalar_,OPERATOR>,
                                typename Operand::Scalar,
                                typename Operand::FactorTypeList,
                                typename Operand::FreeDimIndexTypeList,
                                typename Operand::UsedDimIndexTypeList>
{
    typedef ExpressionTemplate_i<ExpressionTemplate_ScalarMultiplication_t<Operand,Scalar_,OPERATOR>,
                                 typename Operand::Scalar,
                                 typename Operand::FactorTypeList,
                                 typename Operand::FreeDimIndexTypeList,
                                 typename Operand::UsedDimIndexTypeList> Parent;
    typedef typename Parent::Derived Derived;
    typedef typename Parent::Scalar Scalar;
    typedef typename Parent::FactorTypeList FactorTypeList;
    typedef typename Parent::FreeDimIndexTypeList FreeDimIndexTypeList;
    typedef typename Parent::UsedDimIndexTypeList UsedDimIndexTypeList;
    typedef typename Parent::MultiIndex MultiIndex;
    using Parent::IS_EXPRESSION_TEMPLATE_I;

    enum
    {
        STATIC_ASSERT_IN_ENUM(Operand::IS_EXPRESSION_TEMPLATE_I, OPERAND_IS_EXPRESSION_TEMPLATE),
        STATIC_ASSERT_IN_ENUM((Lvd::Meta::TypesAreEqual<typename Operand::Scalar,Scalar_>::v), OPERAND_SCALAR_MATCHES_SCALAR),
        STATIC_ASSERT_IN_ENUM((OPERATOR == '*' || OPERATOR == '/'), OPERATOR_IS_VALID)
    };

    ExpressionTemplate_ScalarMultiplication_t (Operand const &operand, Scalar scalar_operand)
        :
        m_operand(operand),
        m_scalar_operand(scalar_operand)
    {
        // TODO: should there be a runtime check here if OPERATOR is '/' and scalar_operand is zero (or close to zero)?
    }

    operator Scalar () const
    {
        STATIC_ASSERT_TYPELIST_IS_EMPTY(FreeDimIndexTypeList);
        return operator[](MultiIndex());
    }

    Scalar operator [] (MultiIndex const &m) const
    {
        if (OPERATOR == '*')
            return m_operand[m] * m_scalar_operand;
        else
            return m_operand[m] / m_scalar_operand;
    }

    template <typename OtherTensor>
    bool uses_tensor (OtherTensor const &t) const
    {
        return m_operand.uses_tensor(t);
    }

private:

    Operand const &m_operand;
    Scalar m_scalar_operand;
};

// ////////////////////////////////////////////////////////////////////////////
// multiplication of expression templates (tensor product and contraction)
// ////////////////////////////////////////////////////////////////////////////

// TODO: there is an issue to think about: while it is totally valid to do
// u(i)*v(j)*w(j) (this is an outer product contracted with a vector), the
// expression v(j)*w(j) can be computed first and factored out of the whole
// thing, instead of needing to be multiplied out for each access of the i index.
// this may be somewhat difficult to do, as it would require searching the
// expression template AST for such contractions and restructuring the AST.
// NOTE: if this is ever subclassed, then it will be necessary to change the inheritance to pass in the Derived type
template <typename LeftOperand, typename RightOperand>
struct ExpressionTemplate_Multiplication_t
    :
    public ExpressionTemplate_i<ExpressionTemplate_Multiplication_t<LeftOperand,RightOperand>,
                                typename LeftOperand::Scalar,
                                typename FactorTypeListOfMultiplication_t<LeftOperand,RightOperand>::T,
                                typename FreeDimIndexTypeListOfMultiplication_t<LeftOperand,RightOperand>::T,
                                typename UsedDimIndexTypeListOfMultiplication_t<LeftOperand,RightOperand>::T>
{
    typedef ExpressionTemplate_i<ExpressionTemplate_Multiplication_t<LeftOperand,RightOperand>,
                                 typename LeftOperand::Scalar,
                                 typename FactorTypeListOfMultiplication_t<LeftOperand,RightOperand>::T,
                                 typename FreeDimIndexTypeListOfMultiplication_t<LeftOperand,RightOperand>::T,
                                 typename UsedDimIndexTypeListOfMultiplication_t<LeftOperand,RightOperand>::T> Parent;
    typedef typename Parent::Derived Derived;
    typedef typename Parent::Scalar Scalar;
    typedef typename Parent::FactorTypeList FactorTypeList;
    typedef typename Parent::FreeDimIndexTypeList FreeDimIndexTypeList;
    typedef typename Parent::UsedDimIndexTypeList UsedDimIndexTypeList;
    typedef typename Parent::MultiIndex MultiIndex;
    using Parent::IS_EXPRESSION_TEMPLATE_I;

    typedef typename SummedDimIndexTypeListOfMultiplication_t<LeftOperand,RightOperand>::T SummedDimIndexTypeList;

    // TODO: check that the summed indices from each operand have no indices in common
    // though technically this is unnecessary, because the summed indices are "private"
    // to each contraction, so this is really for the human's benefit, not getting
    // confused by multiple repeated indices that have nothing to do with each other.
    enum
    {
        STATIC_ASSERT_IN_ENUM(LeftOperand::IS_EXPRESSION_TEMPLATE_I, LEFT_OPERAND_IS_EXPRESSION_TEMPLATE),
        STATIC_ASSERT_IN_ENUM(RightOperand::IS_EXPRESSION_TEMPLATE_I, RIGHT_OPERAND_IS_EXPRESSION_TEMPLATE),
        STATIC_ASSERT_IN_ENUM((Lvd::Meta::TypesAreEqual<typename LeftOperand::Scalar,typename RightOperand::Scalar>::v), OPERAND_SCALAR_TYPES_ARE_EQUAL),
        STATIC_ASSERT_IN_ENUM((!HasNontrivialIntersectionAsSets_t<FreeDimIndexTypeList,UsedDimIndexTypeList>::V), FREE_INDICES_DONT_COLLIDE_WITH_USED)
    };
    // TODO: ensure there are no indices that occur 3+ times (?)

    ExpressionTemplate_Multiplication_t (LeftOperand const &left_operand, RightOperand const &right_operand)
        :
        m_left_operand(left_operand),
        m_right_operand(right_operand)
    { }

    // available ONLY if FreeDimIndexTypeList is EmptyTypeList
    operator Scalar () const
    {
        STATIC_ASSERT_TYPELIST_IS_EMPTY(FreeDimIndexTypeList);
        return operator[](MultiIndex());
    }

    Scalar operator [] (MultiIndex const &m) const
    {
        return BinarySummation_t<LeftOperand,RightOperand,FreeDimIndexTypeList,SummedDimIndexTypeList>::eval(m_left_operand, m_right_operand, m);
    }

    template <typename OtherTensor>
    bool uses_tensor (OtherTensor const &t) const
    {
        return m_left_operand.uses_tensor(t) || m_right_operand.uses_tensor(t);
    }

private:

    LeftOperand const &m_left_operand;
    RightOperand const &m_right_operand;
};

// ////////////////////////////////////////////////////////////////////////////
// bundling multiple separate indices into a single vector index (downcasting)
// ////////////////////////////////////////////////////////////////////////////

template <typename Operand, typename BundleDimIndexTypeList, typename ResultingDimIndexType>
struct ExpressionTemplate_IndexBundle_t
    :
    public ExpressionTemplate_IndexedObject_t<IndexBundle_t<Operand,BundleDimIndexTypeList,ResultingDimIndexType>,
                                              NullType, // TODO: real code
                                              typename IndexBundle_t<Operand,BundleDimIndexTypeList,ResultingDimIndexType>::DimIndexTypeList,
                                              typename SummedIndexTypeList_t<typename IndexBundle_t<Operand,BundleDimIndexTypeList,ResultingDimIndexType>::DimIndexTypeList>::T,
                                              FORCE_CONST,
                                              CHECK_FOR_ALIASING, // irrelevant value
                                              ExpressionTemplate_IndexBundle_t<Operand,BundleDimIndexTypeList,ResultingDimIndexType> >
{
    typedef ExpressionTemplate_IndexedObject_t<IndexBundle_t<Operand,BundleDimIndexTypeList,ResultingDimIndexType>,
                                               NullType, // TODO: real code
                                               typename IndexBundle_t<Operand,BundleDimIndexTypeList,ResultingDimIndexType>::DimIndexTypeList,
                                               typename SummedIndexTypeList_t<typename IndexBundle_t<Operand,BundleDimIndexTypeList,ResultingDimIndexType>::DimIndexTypeList>::T,
                                               FORCE_CONST,
                                               CHECK_FOR_ALIASING, // irrelevant value
                                               ExpressionTemplate_IndexBundle_t<Operand,BundleDimIndexTypeList,ResultingDimIndexType> > Parent;
    typedef typename Parent::Derived Derived;
    typedef typename Parent::Scalar Scalar;
    typedef typename Parent::FreeDimIndexTypeList FreeDimIndexTypeList;
    typedef typename Parent::UsedDimIndexTypeList UsedDimIndexTypeList;
    typedef typename Parent::MultiIndex MultiIndex;
    using Parent::IS_EXPRESSION_TEMPLATE_I;
    typedef typename Parent::SummedDimIndexTypeList SummedDimIndexTypeList;

    typedef IndexBundle_t<Operand,BundleDimIndexTypeList,ResultingDimIndexType> IndexBundle;

    ExpressionTemplate_IndexBundle_t (Operand const &operand) : Parent(m_index_bundle), m_index_bundle(operand) { }

    operator Scalar () const
    {
        STATIC_ASSERT_TYPELIST_IS_EMPTY(FreeDimIndexTypeList);
        return operator[](MultiIndex());
    }

    // read-only, because it doesn't make sense to assign to an index-bundled expression (which is possibly also a summation).
    Scalar operator [] (MultiIndex const &m) const
    {
        return UnarySummation_t<IndexBundle,typename IndexBundle::DimIndexTypeList,SummedDimIndexTypeList>::eval(m_index_bundle, m);
    }

    template <typename OtherTensor>
    bool uses_tensor (OtherTensor const &t) const { return m_index_bundle.uses_tensor(t); }

private:

    IndexBundle m_index_bundle;
};

// ////////////////////////////////////////////////////////////////////////////
// splitting a single vector index into a multiple separate indices (upcasting)
// ////////////////////////////////////////////////////////////////////////////

template <typename Operand, typename SourceDimIndexType, typename SplitDimIndexTypeList>
struct ExpressionTemplate_IndexSplit_t
    :
    public ExpressionTemplate_IndexedObject_t<IndexSplitter_t<Operand,SourceDimIndexType,SplitDimIndexTypeList>,
                                              NullType, // TODO: real code
                                              typename IndexSplitter_t<Operand,SourceDimIndexType,SplitDimIndexTypeList>::DimIndexTypeList,
                                              typename SummedIndexTypeList_t<typename IndexSplitter_t<Operand,SourceDimIndexType,SplitDimIndexTypeList>::DimIndexTypeList>::T,
                                              FORCE_CONST,
                                              CHECK_FOR_ALIASING, // irrelevant value
                                              ExpressionTemplate_IndexSplit_t<Operand,SourceDimIndexType,SplitDimIndexTypeList> >
{
    typedef ExpressionTemplate_IndexedObject_t<IndexSplitter_t<Operand,SourceDimIndexType,SplitDimIndexTypeList>,
                                               NullType, // TODO: real code
                                               typename IndexSplitter_t<Operand,SourceDimIndexType,SplitDimIndexTypeList>::DimIndexTypeList,
                                               typename SummedIndexTypeList_t<typename IndexSplitter_t<Operand,SourceDimIndexType,SplitDimIndexTypeList>::DimIndexTypeList>::T,
                                               FORCE_CONST,
                                               CHECK_FOR_ALIASING, // irrelevant value
                                               ExpressionTemplate_IndexSplit_t<Operand,SourceDimIndexType,SplitDimIndexTypeList> > Parent;
    typedef typename Parent::Derived Derived;
    typedef typename Parent::Scalar Scalar;
    typedef typename Parent::FreeDimIndexTypeList FreeDimIndexTypeList;
    typedef typename Parent::UsedDimIndexTypeList UsedDimIndexTypeList;
    typedef typename Parent::MultiIndex MultiIndex;
    using Parent::IS_EXPRESSION_TEMPLATE_I;
    typedef typename Parent::SummedDimIndexTypeList SummedDimIndexTypeList;

    typedef IndexSplitter_t<Operand,SourceDimIndexType,SplitDimIndexTypeList> IndexSplitter;

    ExpressionTemplate_IndexSplit_t (Operand const &operand) : Parent(m_index_splitter), m_index_splitter(operand) { }

    operator Scalar () const
    {
        STATIC_ASSERT_TYPELIST_IS_EMPTY(FreeDimIndexTypeList);
        return operator[](MultiIndex());
    }

    // read-only, because it doesn't make sense to assign to an index-bundled expression (which is possibly also a summation).
    Scalar operator [] (MultiIndex const &m) const
    {
        return UnarySummation_t<IndexSplitter,typename IndexSplitter::DimIndexTypeList,SummedDimIndexTypeList>::eval(m_index_splitter, m);
    }

    template <typename OtherTensor>
    bool uses_tensor (OtherTensor const &t) const { return m_index_splitter.uses_tensor(t); }

private:

    IndexSplitter m_index_splitter;
};

// ////////////////////////////////////////////////////////////////////////////
// operator overloads for expression templates
// ////////////////////////////////////////////////////////////////////////////

// expression template addition/subtractions

// addition
template <typename LeftDerived, typename LeftFactorTypeList, typename LeftFreeDimIndexTypeList, typename LeftUsedDimIndexTypeList,
          typename RightDerived, typename RightFactorTypeList, typename RightFreeDimIndexTypeList, typename RightUsedDimIndexTypeList>
inline ExpressionTemplate_Addition_t<LeftDerived,RightDerived,'+'>
    operator + (ExpressionTemplate_i<LeftDerived,typename LeftDerived::Scalar,LeftFactorTypeList,LeftFreeDimIndexTypeList,LeftUsedDimIndexTypeList> const &left_operand,
                ExpressionTemplate_i<RightDerived,typename RightDerived::Scalar,RightFactorTypeList,RightFreeDimIndexTypeList,RightUsedDimIndexTypeList> const &right_operand)
{
    return ExpressionTemplate_Addition_t<LeftDerived,RightDerived,'+'>(left_operand.as_derived(), right_operand.as_derived());
}

// subtraction
template <typename LeftDerived, typename LeftFactorTypeList, typename LeftFreeDimIndexTypeList, typename LeftUsedDimIndexTypeList,
          typename RightDerived, typename RightFactorTypeList, typename RightFreeDimIndexTypeList, typename RightUsedDimIndexTypeList>
inline ExpressionTemplate_Addition_t<LeftDerived,RightDerived,'-'>
    operator - (ExpressionTemplate_i<LeftDerived,typename LeftDerived::Scalar,LeftFactorTypeList,LeftFreeDimIndexTypeList,LeftUsedDimIndexTypeList> const &left_operand,
                ExpressionTemplate_i<RightDerived,typename RightDerived::Scalar,RightFactorTypeList,RightFreeDimIndexTypeList,RightUsedDimIndexTypeList> const &right_operand)
{
    return ExpressionTemplate_Addition_t<LeftDerived,RightDerived,'-'>(left_operand.as_derived(), right_operand.as_derived());
}

// scalar multiplication/division, including unary negation (multiplication by -1)

// scalar multiplication on the right
template <typename Derived, typename FactorTypeList, typename FreeDimIndexTypeList, typename UsedDimIndexTypeList>
inline ExpressionTemplate_ScalarMultiplication_t<Derived,typename Derived::Scalar,'*'>
    operator * (ExpressionTemplate_i<Derived,typename Derived::Scalar,FactorTypeList,FreeDimIndexTypeList,UsedDimIndexTypeList> const &operand,
                typename Derived::Scalar scalar_operand)
{
    return ExpressionTemplate_ScalarMultiplication_t<Derived,typename Derived::Scalar,'*'>(operand.as_derived(), scalar_operand);
}

// scalar multiplication on the right -- this overload allows integer literals to be used in scalar multiplications
template <typename Derived, typename FactorTypeList, typename FreeDimIndexTypeList, typename UsedDimIndexTypeList>
inline ExpressionTemplate_ScalarMultiplication_t<Derived,typename Derived::Scalar,'*'>
    operator * (ExpressionTemplate_i<Derived,typename Derived::Scalar,FactorTypeList,FreeDimIndexTypeList,UsedDimIndexTypeList> const &operand,
                int scalar_operand)
{
    return ExpressionTemplate_ScalarMultiplication_t<Derived,typename Derived::Scalar,'*'>(operand.as_derived(), scalar_operand);
}

// scalar multiplication on the left
template <typename Derived, typename FactorTypeList, typename FreeDimIndexTypeList, typename UsedDimIndexTypeList>
inline ExpressionTemplate_ScalarMultiplication_t<Derived,typename Derived::Scalar,'*'>
    operator * (typename Derived::Scalar scalar_operand,
                ExpressionTemplate_i<Derived,typename Derived::Scalar,FactorTypeList,FreeDimIndexTypeList,UsedDimIndexTypeList> const &operand)
{
    return ExpressionTemplate_ScalarMultiplication_t<Derived,typename Derived::Scalar,'*'>(operand.as_derived(), scalar_operand);
}

// scalar multiplication on the left -- this overload allows integer literals to be used in scalar multiplications
template <typename Derived, typename FactorTypeList, typename FreeDimIndexTypeList, typename UsedDimIndexTypeList>
inline ExpressionTemplate_ScalarMultiplication_t<Derived,typename Derived::Scalar,'*'>
    operator * (int scalar_operand,
                ExpressionTemplate_i<Derived,typename Derived::Scalar,FactorTypeList,FreeDimIndexTypeList,UsedDimIndexTypeList> const &operand)
{
    return ExpressionTemplate_ScalarMultiplication_t<Derived,typename Derived::Scalar,'*'>(operand.as_derived(), scalar_operand);
}

// scalar division on the right
template <typename Derived, typename FactorTypeList, typename FreeDimIndexTypeList, typename UsedDimIndexTypeList>
inline ExpressionTemplate_ScalarMultiplication_t<Derived,typename Derived::Scalar,'/'>
    operator / (ExpressionTemplate_i<Derived,typename Derived::Scalar,FactorTypeList,FreeDimIndexTypeList,UsedDimIndexTypeList> const &operand,
                typename Derived::Scalar scalar_operand)
{
    return ExpressionTemplate_ScalarMultiplication_t<Derived,typename Derived::Scalar,'/'>(operand.as_derived(), scalar_operand);
}

// scalar division on the right -- this overload allows integer literals to be used in scalar divisions (it's Scalar division, not integer division)
template <typename Derived, typename FactorTypeList, typename FreeDimIndexTypeList, typename UsedDimIndexTypeList>
inline ExpressionTemplate_ScalarMultiplication_t<Derived,typename Derived::Scalar,'/'>
    operator / (ExpressionTemplate_i<Derived,typename Derived::Scalar,FactorTypeList,FreeDimIndexTypeList,UsedDimIndexTypeList> const &operand,
                int scalar_operand)
{
    return ExpressionTemplate_ScalarMultiplication_t<Derived,typename Derived::Scalar,'/'>(operand.as_derived(), scalar_operand);
}

// unary negation
template <typename Derived, typename FactorTypeList, typename FreeDimIndexTypeList, typename UsedDimIndexTypeList>
inline ExpressionTemplate_ScalarMultiplication_t<Derived,typename Derived::Scalar,'*'>
    operator - (ExpressionTemplate_i<Derived,typename Derived::Scalar,FactorTypeList,FreeDimIndexTypeList,UsedDimIndexTypeList> const &operand)
{
    return ExpressionTemplate_ScalarMultiplication_t<Derived,typename Derived::Scalar,'*'>(operand.as_derived(), -1);
}

// expression template multiplication -- tensor contraction and product

template <typename LeftDerived, typename LeftFactorTypeList, typename LeftFreeDimIndexTypeList, typename LeftUsedDimIndexTypeList,
          typename RightDerived, typename RightFactorTypeList, typename RightFreeDimIndexTypeList, typename RightUsedDimIndexTypeList>
inline ExpressionTemplate_Multiplication_t<LeftDerived,RightDerived>
    operator * (ExpressionTemplate_i<LeftDerived,typename LeftDerived::Scalar,LeftFactorTypeList,LeftFreeDimIndexTypeList,LeftUsedDimIndexTypeList> const &left_operand,
                ExpressionTemplate_i<RightDerived,typename RightDerived::Scalar,RightFactorTypeList,RightFreeDimIndexTypeList,RightUsedDimIndexTypeList> const &right_operand)
{
    return ExpressionTemplate_Multiplication_t<LeftDerived,RightDerived>(left_operand.as_derived(), right_operand.as_derived());
}

} // end of namespace Tenh

#endif // TENH_EXPRESSION_TEMPLATES_HPP_
