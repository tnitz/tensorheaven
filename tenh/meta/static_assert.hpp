// ///////////////////////////////////////////////////////////////////////////
// tenh/meta/static_assert.hpp by Ted Nitz, created 2013/04/12
// Copyright Leap Motion Inc.
// ///////////////////////////////////////////////////////////////////////////

#ifndef TENH_META_STATIC_ASSERT_HPP_
#define TENH_META_STATIC_ASSERT_HPP_

#include "tenh/meta/lvd.hpp"

namespace Tenh {

struct EmptyTypeList;

}

#define STATIC_ASSERT(CONDITION,MESSAGE) do { if(Tenh::StaticAssert<bool(CONDITION)>::MESSAGE) {} } while (false)
#define STATIC_ASSERT_AS_RVALUE(CONDITION,MESSAGE) Tenh::StaticAssert<bool(CONDITION)>::MESSAGE
#define STATIC_ASSERT_IN_ENUM(CONDITION,MESSAGE) MESSAGE = Tenh::StaticAssert<bool(CONDITION)>::MESSAGE
#define STATIC_ASSERT_TYPELIST_IS_EMPTY(TYPELIST) STATIC_ASSERT((Lvd::Meta::TypesAreEqual<TYPELIST,Tenh::EmptyTypeList>::v), TYPELIST_MUST_BE_EMPTY)

namespace Tenh {

template <bool condition>
struct StaticAssert {};

template <>
struct StaticAssert<true>
{
    enum
    {
        ALL_FACTOR_TYPE_SCALARS_ARE_EQUAL,
        ARGUMENT_LENGTH_MUST_EQUAL_FACTOR_LENGTH,
        ATTEMPTED_ACCESS_PAST_LIST_END,
        BASIS_IS_STANDARD_EUCLIDEAN,
        BUNDLE_AND_RESULTING_MUST_BE_DISTINCT,
        BUNDLE_INDICES_MUST_BE_FREE,
        CAN_ONLY_BUNDLE_TWO_INDICES,
        CANT_CONTRACT_WITH_EUCLIDEANLY_EMBEDDED_TENSOR,
        DEGREE_MUST_BE_TWO,
        DERIVED_MUST_NOT_BE_NULL_TYPE,
        DOMAIN_INDICES_MUST_NOT_CONTAIN_DUPLICATES,
        DIMENSION_DOES_NOT_MATCH_ARGUMENT_COUNT,
        DIMENSION_MUST_BE_POSITIVE,
        FACTOR_DIMENSIONS_MUST_BE_EQUAL,
        FACTOR_SCALAR_TYPES_ARE_EQUAL,
        FACTOR_TYPE_LIST_IS_NONEMPTY,
        FACTOR1_AND_FACTOR2_MUST_BE_IDENTICAL,
        FREE_INDICES_DONT_COLLIDE_WITH_USED,
        FREE_INDICES_HAVE_NO_DUPLICATES,
        TYPE_MUST_BE_A_TENSOR_I,
        LEFT_OPERAND_HAS_NO_DUPLICATE_FREE_INDICES,
        LEFT_OPERAND_IS_EXPRESSION_TEMPLATE,
        LENGTH_DOES_NOT_MATCH_ARGUMENT_COUNT,
        LENGTH_MUST_BE_POSITIVE,
        ONLY_ONE_DIMENSIONAL_VECTORS_CAN_BE_CONVERTED_TO_SCALARS,
        OPERAND_HAS_THE_SAME_FREE_INDICES,
        OPERAND_IS_EXPRESSION_TEMPLATE,
        OPERAND_SCALAR_MATCHES_SCALAR,
        OPERAND_SCALAR_TYPES_ARE_EQUAL,
        OPERANDS_HAVE_SAME_FREE_INDICES,
        OPERATOR_IS_PLUS_OR_MINUS,
        OPERATOR_IS_VALID,
        RIGHT_OPERAND_HAS_NO_DUPLICATE_FREE_INDICES,
        RIGHT_OPERAND_IS_EXPRESSION_TEMPLATE,
        SOURCE_AND_SPLIT_MUST_BE_DISTINCT,
        SOURCE_INDEX_MUST_BE_FREE,
        START_INDEX_MUST_NOT_EXCEED_END_INDEX,
        TYPEDINDEX_SYMBOL_MUST_NOT_BE_NULL,
        TYPE_MUST_APPEAR_IN_TYPELIST,
        TYPE_MUST_BE_TENSOR2SYMMETRIC_T,
        TYPELIST_MUST_BE_EMPTY
    };
};

} // end of namespace Tenh

#endif // TENH_META_STATIC_ASSERT_HPP_
