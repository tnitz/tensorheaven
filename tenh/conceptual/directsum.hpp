// ///////////////////////////////////////////////////////////////////////////
// tenh/conceptual/directsum.hpp by Ted Nitz, created 2013/10/24
// Copyright Leap Motion Inc.
// ///////////////////////////////////////////////////////////////////////////

#ifndef TENH_CONCEPTUAL_DIRECTSUM_HPP_
#define TENH_CONCEPTUAL_DIRECTSUM_HPP_

#include "tenh/core.hpp"

#include "tenh/conceptual/concept.hpp"
#include "tenh/conceptual/basis.hpp"
#include "tenh/conceptual/dual.hpp"
#include "tenh/conceptual/vectorspace.hpp"
#include "tenh/meta/typelist.hpp"

namespace Tenh {

// ///////////////////////////////////////////////////////////////////////////
// DirectSum_c
// ///////////////////////////////////////////////////////////////////////////

// generic direct sum of formal symbols (e.g. identifiers, builtin C++ types, etc)
template <typename SummandTypeList_>
struct DirectSum_c
{
private:
    enum { STATIC_ASSERT_IN_ENUM(IsTypeList_f<SummandTypeList_>::V, MUST_BE_TYPELIST) };

public:
    typedef EmptyTypeList ParentTypeList;

    typedef SummandTypeList_ SummandTypeList;
    static Uint32 const SUMMAND_COUNT = SummandTypeList::LENGTH;

    static std::string type_as_string ()
    {
        return "DirectSum_c<" + type_string_of<SummandTypeList_>() + '>';
    }
};

template <typename SummandTypeList_>
struct IsConcept_f<DirectSum_c<SummandTypeList_> >
{
    static bool const V = true;
private:
    IsConcept_f();
};

template <typename T> struct IsDirectSum_f
{
    static bool const V = false;
private:
    IsDirectSum_f();
};
template <typename SummandTypeList_> struct IsDirectSum_f<DirectSum_c<SummandTypeList_> >
{
    static bool const V = true;
private:
    IsDirectSum_f();
};

DEFINE_CONCEPTUAL_STRUCTURE_METAFUNCTIONS(DirectSum);
// special convenience macros
#define IS_DIRECT_SUM_UNIQUELY(Concept) HasUniqueDirectSumStructure_f<Concept>::V
#define AS_DIRECT_SUM(Concept) UniqueDirectSumStructureOf_f<Concept>::T

template <typename SummandTypeList_>
struct DualOf_f<DirectSum_c<SummandTypeList_> >
{
    typedef DirectSum_c<typename DualOf_f<SummandTypeList_>::T> T;
private:
    DualOf_f();
};

// property IDs

struct SummandCount; // could this be a forward declaration? Looks like yes.
struct SummandTypeList; // could this be a forward declaration? Looks like yes.

// BaseProperty_f accessors

template <typename SummandTypeList_>
struct BaseProperty_f<DirectSum_c<SummandTypeList_>,SummandCount>
{
    typedef Value_t<Uint32,SummandTypeList_::LENGTH> T;
private:
    BaseProperty_f();
};

template <typename SummandTypeList_>
struct BaseProperty_f<DirectSum_c<SummandTypeList_>,SummandTypeList>
{
    typedef SummandTypeList_ T;
private:
    BaseProperty_f();
};

// this function is only well-defined if SummandTypeList_ is uniform and has at least one element
// disabled due to name problems
// template <typename SummandTypeList_>
// struct BaseProperty_f<DirectSum_c<SummandTypeList_>,TensorPowerFactor>
// {
// private:
//     static bool const THERE_IS_A_UNIQUE_FACTOR = SummandTypeList_::LENGTH >= 1 &&
//                                                  TypeListIsUniform_t<SummandTypeList_>::V;
//     BaseProperty_f();
// public:
//     typedef typename If_f<THERE_IS_A_UNIQUE_FACTOR,
//                           typename SummandTypeList_::HeadType,
//                           NullValue>::T T;
// };

// named property accessors

template <typename Concept_> struct SummandCountOf_f
{
    static Uint32 const V = PropertyValue_f<Concept_,SummandCount>::V;
private:
    OrderOf_f();
};
template <typename Concept_> struct SummandTypeListOf_f
{
    typedef typename Property_f<Concept_,SummandTypeList>::T T;
private:
    SummandTypeListOf_f();
};
// template <typename Concept_> struct TensorPowerFactorOf_f
// {
//     typedef typename Property_f<Concept_,TensorPowerFactor>::T T;
// private:
//     TensorPowerFactorOf_f();
// };

// ///////////////////////////////////////////////////////////////////////////
// DirectSumOfVectorSpaces_c
// ///////////////////////////////////////////////////////////////////////////

// TODO: Replace with predicate-based thing
template <typename SummandTypeList_>
struct AllFactorsAreVectorSpaces_f
{
    static bool const V = HasVectorSpaceStructure_f<typename SummandTypeList_::HeadType>::V &&
                          AllFactorsAreVectorSpaces_f<typename SummandTypeList_::BodyTypeList>::V;
private:
    AllFactorsAreVectorSpaces_f();
};

template <>
struct AllFactorsAreVectorSpaces_f<EmptyTypeList>
{
    static bool const V = true;
private:
    AllFactorsAreVectorSpaces_f();
};

template <typename SummandTypeList_>
struct AllFactorsAreBases_f
{
    static bool const V = HasBasisStructure_f<typename SummandTypeList_::HeadType>::V &&
                          AllFactorsAreBases_f<typename SummandTypeList_::BodyTypeList>::V;
private:
    AllFactorsAreBases_f();
};

template <>
struct AllFactorsAreBases_f<EmptyTypeList>
{
    static bool const V = true;
private:
    AllFactorsAreBases_f();
};

template <typename SummandTypeList_>
struct AllFactorsHaveTheSameField_f
{
private:
    typedef typename SummandTypeList_::HeadType HeadType;
    typedef typename SummandTypeList_::BodyTypeList BodyTypeList;
    AllFactorsHaveTheSameField_f();
public:
    static bool const V = TypesAreEqual_f<typename ScalarFieldOf_f<HeadType>::T,
                                          typename ScalarFieldOf_f<typename BodyTypeList::HeadType>::T>::V &&
                          AllFactorsHaveTheSameField_f<BodyTypeList>::V;
};

template <typename HeadType>
struct AllFactorsHaveTheSameField_f<TypeList_t<HeadType> >
{
    static bool const V = true;
private:
    AllFactorsHaveTheSameField_f();
};

template <>
struct AllFactorsHaveTheSameField_f<EmptyTypeList>
{
    static bool const V = true;
private:
    AllFactorsHaveTheSameField_f();
};

template <typename SummandTypeList_>
struct SumOfDimensions_t
{
private:
    enum { STATIC_ASSERT_IN_ENUM(IS_VECTOR_SPACE_UNIQUELY(typename SummandTypeList_::HeadType), MUST_BE_VECTOR_SPACE) };
    ProductOfDimensions_t();
public:
    static Uint32 const V = DimensionOf_f<typename SummandTypeList_::HeadType>::V +
                            ProductOfDimensions_t<typename SummandTypeList_::BodyTypeList>::V;
};

template <>
struct SumOfDimensions_t<EmptyTypeList>
{
    static Uint32 const V = 0;
private:
    ProductOfDimensions_t();
};

template <typename TypeList>
struct IdsOfTypeList_t
{
    typedef TypeList_t<typename TypeList::HeadType::Id,
                       typename IdsOfTypeList_t<typename TypeList::BodyTypeList>::T> T;
private:
    IdsOfTypeList_t();
};

template <>
struct IdsOfTypeList_t<EmptyTypeList>
{
    typedef EmptyTypeList T;
private:
    IdsOfTypeList_t();
};


// SummandTypeList_ must be a TypeList_t of VectorSpace_c types
template <typename SummandTypeList_>
struct DirectSumOfVectorSpaces_c
{
private:
    enum
    {
        STATIC_ASSERT_IN_ENUM(IsTypeList_f<SummandTypeList_>::V, MUST_BE_TYPELIST),
        STATIC_ASSERT_IN_ENUM(AllFactorsAreVectorSpaces_f<SummandTypeList_>::V, ALL_FACTORS_MUST_BE_VECTOR_SPACES),
        STATIC_ASSERT_IN_ENUM(AllFactorsHaveTheSameField_f<SummandTypeList_>::V, ALL_FACTORS_MUST_HAVE_SAME_FIELD)
    };

    typedef DirectSum_c<SummandTypeList_> As_DirectSum;
    typedef VectorSpace_c<typename ScalarFieldOf_f<typename SummandTypeList_::HeadType>::T,
                          SumOfDimensions_t<SummandTypeList_>::V,
                          DirectSum_c<typename IdsOfTypeList_t<SummandTypeList_>::T> > As_VectorSpace;
    typedef EmbeddableInDirectSumOfVectorSpaces_c<DirectSumOfVectorSpaces_c> As_EmbeddableInDirectSumOfVectorSpaces;
public:
    typedef TypeList_t<As_DirectSum,
            TypeList_t<As_VectorSpace,
            TypeList_t<As_EmbeddableInDirectSumOfVectorSpaces> > > ParentTypeList;

    typedef typename As_VectorSpace::Id Id;

    static std::string type_as_string ()
    {
        return "DirectSumOfVectorSpaces_c<" + type_string_of<SummandTypeList_>() + '>';
    }
};

template <typename SummandTypeList_>
struct IsConcept_f<DirectSumOfVectorSpaces_c<SummandTypeList_> >
{
    static bool const V = true;
private:
    IsConcept_f();
};

template <typename T> struct IsDirectSumOfVectorSpaces_f
{
    static bool const V = false;
private:
    IsDirectSumOfVectorSpaces_f();
};
template <typename SummandTypeList_> struct IsDirectSumOfVectorSpaces_f<DirectSumOfVectorSpaces_c<SummandTypeList_> >
{
    static bool const V = true;
private:
    IsDirectSumOfVectorSpaces_f();
};

DEFINE_CONCEPTUAL_STRUCTURE_METAFUNCTIONS(DirectSumOfVectorSpaces);
// special convenience macros
#define IS_DIRECT_SUM_OF_VECTOR_SPACES_UNIQUELY(Concept) HasUniqueDirectSumOfVectorSpacesStructure_f<Concept>::V
#define AS_DIRECT_SUM_OF_VECTOR_SPACES(Concept) UniqueDirectSumOfVectorSpacesStructureOf_f<Concept>::T

template <typename SummandTypeList_>
struct DualOf_f<DirectSumOfVectorSpaces_c<SummandTypeList_> >
{
    typedef DirectSumOfVectorSpaces_c<typename DualOf_f<SummandTypeList_>::T> T;
private:
    DualOf_f();
};

// ///////////////////////////////////////////////////////////////////////////
// DirectSumOfBases_c
// ///////////////////////////////////////////////////////////////////////////

// SummandTypeList_ must be a TypeList_t of Basis_c types
template <typename SummandTypeList_>
struct DirectSumOfBases_c
{
private:
    enum
    {
        STATIC_ASSERT_IN_ENUM(IsTypeList_f<SummandTypeList_>::V, MUST_BE_TYPELIST),
        STATIC_ASSERT_IN_ENUM(AllFactorsAreBases_f<SummandTypeList_>::V, ALL_FACTORS_MUST_BE_BASES)
    };
    typedef DirectSum_c<SummandTypeList_> As_DirectSum;
    typedef Basis_c<DirectSum_c<typename IdsOfTypeList_t<SummandTypeList_>::T> > As_Basis;
public:
    typedef TypeList_t<As_DirectSum, TypeList_t<As_Basis> > ParentTypeList;

    typedef typename As_Basis::Id Id;

    static std::string type_as_string ()
    {
        return "DirectSumOfBases_c<" + type_string_of<SummandTypeList_>() + '>';
    }
};

template <typename SummandTypeList_>
struct IsConcept_f<DirectSumOfBases_c<SummandTypeList_> >
{
    static bool const V = true;
private:
    IsConcept_f();
};

template <typename T> struct IsDirectSumOfBases_f
{
    static bool const V = false;
private:
    IsDirectSumOfBases_f();
};
template <typename SummandTypeList_> struct IsDirectSumOfBases_f<DirectSumOfBases_c<SummandTypeList_> >
{
    static bool const V = true;
private:
    IsDirectSumOfBases_f();
};

DEFINE_CONCEPTUAL_STRUCTURE_METAFUNCTIONS(DirectSumOfBases);
// special convenience macros
#define IS_DIRECT_SUM_OF_BASES_UNIQUELY(Concept) HasUniqueDirectSumOfBasesStructure_f<Concept>::V
#define AS_DIRECT_SUM_OF_BASES(Concept) UniqueDirectSumOfBasesStructureOf_f<Concept>::T

template <typename SummandTypeList_>
struct DualOf_f<DirectSumOfBases_c<SummandTypeList_> >
{
    typedef DirectSumOfBases_c<typename DualOf_f<SummandTypeList_>::T> T;
private:
    DualOf_f();
};

// ///////////////////////////////////////////////////////////////////////////
// BasedDirectSumOfVectorSpaces_c
// ///////////////////////////////////////////////////////////////////////////

// TODO: this should be an EmbeddableInDirectSumOfVectorSpaces
template <typename DirectSumOfVectorSpaces_, typename Basis_>
struct BasedDirectSumOfVectorSpaces_c
{
private:
    enum
    {
        STATIC_ASSERT_IN_ENUM(IS_DIRECT_SUM_OF_VECTOR_SPACES_UNIQUELY(DirectSumOfVectorSpaces_), MUST_BE_DIRECT_SUM_OF_VECTOR_SPACES),
        STATIC_ASSERT_IN_ENUM(IS_BASIS_UNIQUELY(Basis_), MUST_BE_BASIS),
    };

    typedef DirectSumOfVectorSpaces_ As_DirectSumOfVectorSpaces;
    typedef BasedVectorSpace_c<DirectSumOfVectorSpaces_,Basis_> As_BasedVectorSpace;
public:
    typedef TypeList_t<As_DirectSumOfVectorSpaces,
            TypeList_t<As_BasedVectorSpace> > ParentTypeList;

    typedef typename As_BasedVectorSpace::Id Id;

    static std::string type_as_string ()
    {
        return "BasedDirectSumOfVectorSpaces_c<" + type_string_of<DirectSumOfVectorSpaces_>() + ','
                                                     + type_string_of<Basis_>() + '>';
    }
};

template <typename DirectSumOfVectorSpaces_, typename Basis_>
struct IsConcept_f<BasedDirectSumOfVectorSpaces_c<DirectSumOfVectorSpaces_, Basis_> >
{
    static bool const V = true;
private:
    IsConcept_f();
};

template <typename T> struct IsBasedDirectSumOfVectorSpaces_f
{
    static bool const V = false;
private:
    IsBasedDirectSumOfVectorSpaces_f();
};
template <typename DirectSumOfVectorSpaces_, typename Basis_> struct IsBasedDirectSumOfVectorSpaces_f<BasedDirectSumOfVectorSpaces_c<DirectSumOfVectorSpaces_,Basis_> >
{
    static bool const V = true;
private:
    IsBasedDirectSumOfVectorSpaces_f();
};

DEFINE_CONCEPTUAL_STRUCTURE_METAFUNCTIONS(BasedDirectSumOfVectorSpaces);
// special convenience macros
#define IS_BASED_DIRECT_SUM_OF_VECTOR_SPACES_UNIQUELY(Concept) HasUniqueBasedDirectSumOfVectorSpacesStructure_f<Concept>::V
#define AS_BASED_DIRECT_SUM_OF_VECTOR_SPACES(Concept) UniqueBasedDirectSumOfVectorSpacesStructureOf_f<Concept>::T

template <typename DirectSumOfVectorSpaces, typename Basis>
struct DualOf_f<BasedDirectSumOfVectorSpaces_c<DirectSumOfVectorSpaces,Basis> >
{
    typedef BasedDirectSumOfVectorSpaces_c<typename DualOf_f<DirectSumOfVectorSpaces>::T,typename DualOf_f<Basis>::T> T;
private:
    DualOf_f();
};

// ///////////////////////////////////////////////////////////////////////////
// DirectSumOfBasedVectorSpaces_c
// ///////////////////////////////////////////////////////////////////////////

// TODO: Predicate thingy
template <typename SummandTypeList_>
struct AllFactorsAreBasedVectorSpaces_f
{
    static bool const V = HasBasedVectorSpaceStructure_f<typename SummandTypeList_::HeadType>::V &&
                          AllFactorsAreBasedVectorSpaces_f<typename SummandTypeList_::BodyTypeList>::V;
private:
    AllFactorsAreBasedVectorSpaces_f();
};

template <>
struct AllFactorsAreBasedVectorSpaces_f<EmptyTypeList>
{
    static bool const V = true;
private:
    AllFactorsAreBasedVectorSpaces_f();
};

// SummandTypeList_ must be a TypeList_t of BasedVectorSpace_c types
template <typename SummandTypeList_>
struct DirectSumOfBasedVectorSpaces_c
{
private:
    enum
    {
        STATIC_ASSERT_IN_ENUM(IsTypeList_f<SummandTypeList_>::V, MUST_BE_TYPELIST),
        STATIC_ASSERT_IN_ENUM(AllFactorsAreBasedVectorSpaces_f<SummandTypeList_>::V, ALL_FACTORS_MUST_BE_BASED_VECTOR_SPACES),
    };

    typedef typename PropertyOfEachInTypeList_f<SummandTypeList_,Basis>::T BasisTypeList;
    typedef BasedDirectSumOfVectorSpaces_c<DirectSumOfVectorSpaces_c<SummandTypeList_>,
                                               DirectSumOfBases_c<BasisTypeList> > As_BasedDirectSumOfVectorSpaces;
    typedef EmbeddableInDirectSumOfBasedVectorSpaces_c<DirectSumOfBasedVectorSpaces_c,DirectSumOfVectorSpaces_c<SummandTypeList_> > As_EmbeddableInDirectSumOfBasedVectorSpaces;
public:
    typedef TypeList_t<As_BasedDirectSumOfVectorSpaces,
            TypeList_t<As_EmbeddableInDirectSumOfBasedVectorSpaces> > ParentTypeList;

    typedef typename As_BasedDirectSumOfVectorSpaces::Id Id;

    static std::string type_as_string ()
    {
        return "DirectSumOfBasedVectorSpaces_c<" + type_string_of<SummandTypeList_>() + '>';
    }
};

template <typename SummandTypeList_>
struct IsConcept_f<DirectSumOfBasedVectorSpaces_c<SummandTypeList_> >
{
    static bool const V = true;
private:
    IsConcept_f();
};

template <typename T> struct IsDirectSumOfBasedVectorSpaces_f
{
    static bool const V = false;
private:
    IsDirectSumOfBasedVectorSpaces_f();
};
template <typename SummandTypeList_> struct IsDirectSumOfBasedVectorSpaces_f<DirectSumOfBasedVectorSpaces_c<SummandTypeList_> >
{
    static bool const V = true;
private:
    IsDirectSumOfBasedVectorSpaces_f();
};

DEFINE_CONCEPTUAL_STRUCTURE_METAFUNCTIONS(DirectSumOfBasedVectorSpaces);
// special convenience macros
#define IS_DIRECT_SUM_OF_BASED_VECTOR_SPACES_UNIQUELY(Concept) HasUniqueDirectSumOfBasedVectorSpacesStructure_f<Concept>::V
#define AS_DIRECT_SUM_OF_BASED_VECTOR_SPACES(Concept) UniqueDirectSumOfBasedVectorSpacesStructureOf_f<Concept>::T

// there is a natural identification of the dual of a direct sum of vector spaces with the
// direct sum of the duals of the vector spaces
template <typename SummandTypeList_>
struct DualOf_f<DirectSumOfBasedVectorSpaces_c<SummandTypeList_> >
{
    typedef DirectSumOfBasedVectorSpaces_c<typename DualOf_f<SummandTypeList_>::T> T;
private:
    DualOf_f();
};

// ///////////////////////////////////////////////////////////////////////////
// meta-functions for taking direct sum powers of stuff
// disabled due to lack of a good name
// ///////////////////////////////////////////////////////////////////////////

// template <Uint32 ORDER_, typename Factor_>
// struct TensorPower_f
// {
//     typedef DirectSum_c<typename UniformTypeListOfLength_t<Factor_,ORDER_>::T> T;
// private:
//     TensorPower_f();
// };
//
// template <Uint32 ORDER_, typename Factor_>
// struct TensorPowerOfVectorSpace_f
// {
// private:
//     enum { STATIC_ASSERT_IN_ENUM(IS_VECTOR_SPACE_UNIQUELY(Factor_), MUST_BE_VECTOR_SPACE) };
//     TensorPowerOfVectorSpace_f();
// public:
//     typedef DirectSumOfVectorSpaces_c<typename UniformTypeListOfLength_t<Factor_,ORDER_>::T> T;
// };
//
// template <Uint32 ORDER_, typename Factor_>
// struct TensorPowerOfBasedVectorSpace_f
// {
// private:
//     enum { STATIC_ASSERT_IN_ENUM(IS_BASED_VECTOR_SPACE_UNIQUELY(Factor_), MUST_BE_BASED_VECTOR_SPACE) };
//     TensorPowerOfBasedVectorSpace_f();
// public:
//     typedef DirectSumOfBasedVectorSpaces_c<typename UniformTypeListOfLength_t<Factor_,ORDER_>::T> T;
// };

} // end of namespace Tenh

#endif // TENH_CONCEPTUAL_DIRECTSUM_HPP_
