// ///////////////////////////////////////////////////////////////////////////
// tenh/conceptual/concept.hpp by Ted Nitz, created 2013/08/21
// Copyright Leap Motion Inc.
// ///////////////////////////////////////////////////////////////////////////

#ifndef TENH_CONCEPTUAL_CONCEPT_HPP_
#define TENH_CONCEPTUAL_CONCEPT_HPP_

#include "tenh/core.hpp"

#include "tenh/meta/typelist.hpp"
#include "tenh/meta/typelist_utility.hpp"
#include "tenh/meta/typestringof.hpp"

// This header defines helper meta functions fo defining and using conceptual inheritence in mathematical code.
// Here is an example object template for the concept Concept Example, which has a Parent Concept structure.
/*
template <typename Parameter_>
struct ConceptExample_c
{
private:
    typedef ParentConcept_c<Parameter_> As_ParentConcept;
public:
    typedef TypeList_t<As_ParentConcept> ParentTypeList;

    // ...
};

template <typename Parameter_>
struct IsConcept_f<ConceptExample_c<Parameter_> >
{ static bool const V = true; };

template <typename T> struct IsConceptExample_f { static bool const V = false; };

template <typename Parameter_>
struct IsConceptExample_f<ConceptExample_c<Parameter_> >
{ static bool const V = true; };

DEFINE_CONCEPTUAL_STRUCTURE_METAFUNCTIONS(ConceptExample);
// special convenience macros
#define IS_CONCEPT_EXAMPLE_UNIQUELY(Concept) HasUniqueConceptExampleStructure_f<Concept>::V
#define AS_CONCEPT_EXAMPLE(Concept) UniqueConceptExampleStructureOf_f<Concept>::T

*/


// TODO: Integrate disambiguation maps
namespace Tenh {

// a Concept is a struct that has a ParentTypeList (which is a TypeList_t)
template <typename T> struct IsConcept_f
{
    static bool const V = false;
private:
    IsConcept_f();
};

// default definition for non-concepts -- no parents
template <typename Concept_, bool ACTUALLY_IS_CONCEPT_ = IsConcept_f<Concept_>::V>
struct ParentTypeListOfConcept_f;

// template specialization for non-concepts -- EmptyTypeList
template <typename Concept_>
struct ParentTypeListOfConcept_f<Concept_,false>
{
    typedef EmptyTypeList T;
};

// template specialization for concepts -- uses Concept_::ParentTypeList
template <typename Concept_>
struct ParentTypeListOfConcept_f<Concept_,true>
{
    typedef typename Concept_::ParentTypeList T;
};

// parent-traversing metafunctions

template <typename ParentTypeList>
struct AncestorsOf_Recursive_f;

// "ancestors" of Concept include Concept as a trivial ancestor -- if Concept
// isn't actually a concept, T will be EmptyTypeList
template <typename Concept>
struct AncestorsOf_f
{
private:
    AncestorsOf_f();
public:
    typedef TypeList_t<Concept,
                       typename AncestorsOf_Recursive_f<typename ParentTypeListOfConcept_f<Concept>::T>::T> T;
};

template <typename ParentTypeList>
struct AncestorsOf_Recursive_f
{
    // depth-first traversal of the ancestor tree
    typedef typename ConcatenationOfTypeLists_t<typename AncestorsOf_f<typename ParentTypeList::HeadType>::T,
                                                typename AncestorsOf_Recursive_f<typename ParentTypeList::BodyTypeList>::T>::T T;
private:
    AncestorsOf_Recursive_f();
};

template <typename HeadType>
struct AncestorsOf_Recursive_f<TypeList_t<HeadType> >
{
    // depth-first traversal of the ancestor tree
    typedef typename AncestorsOf_f<HeadType>::T T;
private:
    AncestorsOf_Recursive_f();
};

template <>
struct AncestorsOf_Recursive_f<EmptyTypeList>
{
    // depth-first traversal of the ancestor tree
    typedef EmptyTypeList T;
private:
    AncestorsOf_Recursive_f();
};

template <typename Concept, typename Predicate>
struct AncestorsSatisfyingPredicate_f
{
    typedef typename ElementsOfTypeListSatisfyingPredicate_t<typename AncestorsOf_f<Concept>::T,Predicate>::T T;
private:
    AncestorsSatisfyingPredicate_f();
};

// for recursively retrieving various conceptual structures

// note that if Concept isn't actually a concept, then T will be EmptyTypeList.
template <typename Concept, typename ConceptualStructurePredicate>
struct ConceptualStructuresOf_f
{
private:
    ConceptualStructuresOf_f();
public:
    typedef typename UniqueTypesIn_t<typename AncestorsSatisfyingPredicate_f<Concept,ConceptualStructurePredicate>::T>::T T;
};

template <typename Concept, typename ConceptualStructurePredicate>
struct HasConceptualStructure_f
{
    static bool const V = ConceptualStructuresOf_f<Concept,ConceptualStructurePredicate>::T::LENGTH > 0;
private:
    HasConceptualStructure_f();
};

template <typename Concept, typename ConceptualStructurePredicate>
struct HasUniqueConceptualStructure_f
{
    static bool const V = ConceptualStructuresOf_f<Concept,ConceptualStructurePredicate>::T::LENGTH == 1;
private:
    HasUniqueConceptualStructure_f();
};

template <typename Concept, typename ConceptualStructurePredicate>
struct UniqueConceptualStructureOf_f
{
private:
    enum { STATIC_ASSERT_IN_ENUM((HasUniqueConceptualStructure_f<Concept,ConceptualStructurePredicate>::V), MUST_HAVE_UNIQUE_CONCEPTUAL_STRUCTURE) };
    UniqueConceptualStructureOf_f();
public:
    typedef typename ConceptualStructuresOf_f<Concept,ConceptualStructurePredicate>::T::HeadType T;
};

// easy way to provide Concept-specific structure metafunctions
#define DEFINE_CONCEPTUAL_STRUCTURE_METAFUNCTIONS(ConceptName) \
struct Is##ConceptName##_p \
{ \
    template <typename T> \
    struct Eval_t \
    { \
        static bool const V = Is##ConceptName##_f<T>::V; \
    private: \
        Eval_t(); \
    }; \
private: \
    Is##ConceptName##_p(); \
}; \
\
template <typename Concept> struct ConceptName##StructuresOf_f \
{ \
    typedef typename ConceptualStructuresOf_f<Concept,Is##ConceptName##_p>::T T; \
private: \
    ConceptName##StructuresOf_f(); \
}; \
template <typename Concept> struct Has##ConceptName##Structure_f \
{ \
    static bool const V = HasConceptualStructure_f<Concept,Is##ConceptName##_p>::V; \
private: \
    Has##ConceptName##Structure_f(); \
}; \
template <typename Concept> struct HasUnique##ConceptName##Structure_f \
{ \
    static bool const V = HasUniqueConceptualStructure_f<Concept,Is##ConceptName##_p>::V; \
private: \
    HasUnique##ConceptName##Structure_f(); \
}; \
template <typename Concept> struct Unique##ConceptName##StructureOf_f \
{ \
private: \
    enum { STATIC_ASSERT_IN_ENUM(HasUnique##ConceptName##Structure_f<Concept>::V, MUST_HAVE_UNIQUE_CONCEPTUAL_STRUCTURE) }; \
    Unique##ConceptName##StructureOf_f(); \
public: \
    typedef typename UniqueConceptualStructureOf_f<Concept,Is##ConceptName##_p>::T T; \
}

// ///////////////////////////////////////////////////////////////////////////
// conceptual property accessor machinery
// ///////////////////////////////////////////////////////////////////////////

// default definition of the given property of exactly the given concept (but
// NOT its ancestors).  particular concepts should specialize this to provide
// definitions.  if the property is a value (e.g. Uint32), then the type should
// be Value_t<...>
template <typename Concept_, typename PropertyId_>
struct BaseProperty_f
{
    typedef NullValue T;
private:
    BaseProperty_f();
};

template <typename TypeList_, typename PropertyId_>
struct BasePropertyOfEachInTypeList_f
{
    typedef TypeList_t<typename BaseProperty_f<typename TypeList_::HeadType,PropertyId_>::T,
                       typename BasePropertyOfEachInTypeList_f<typename TypeList_::BodyTypeList,PropertyId_>::T> T;
private:
    BasePropertyOfEachInTypeList_f();
};

template <typename PropertyId_>
struct BasePropertyOfEachInTypeList_f<EmptyTypeList,PropertyId_>
{
    typedef EmptyTypeList T;
private:
    BasePropertyOfEachInTypeList_f();
};

// gives a list of the unique values of the given property, taken from all ancestors.
template <typename Concept_, typename PropertyId_>
struct MultiProperty_f
{
private:
    typedef typename BasePropertyOfEachInTypeList_f<typename AncestorsOf_f<Concept_>::T,PropertyId_>::T PropertyOfEach;
    typedef typename UniqueTypesIn_t<PropertyOfEach>::T UniquePropertyTypeList;
    MultiProperty_f();
public:
    typedef typename SetSubtraction_t<UniquePropertyTypeList,TypeList_t<NullValue> >::T T;
};

// if MultiProperty_f has exactly one element, returns that.
template <typename Concept_, typename PropertyId_>
struct Property_f
{
private:
    typedef typename MultiProperty_f<Concept_,PropertyId_>::T MultiProperty;
    enum { STATIC_ASSERT_IN_ENUM(MultiProperty::LENGTH == 1, PROPERTY_IS_NOT_WELL_DEFINED) };
    Property_f();
public:
    typedef typename MultiProperty::HeadType T;
};

// if Property_f is a Value_t, returns the value.
template <typename Concept_, typename PropertyId_>
struct PropertyValue_f
{
private:
    typedef typename Property_f<Concept_,PropertyId_>::T ValueType;
    PropertyValue_f();
public:
    static typename ValueType::T const V = ValueType::V;
};

// constructs a TypeList_t whose elements are the results of Property_f on each element
template <typename TypeList_, typename PropertyId_>
struct PropertyOfEachInTypeList_f
{
    typedef TypeList_t<typename Property_f<typename TypeList_::HeadType,PropertyId_>::T,
                       typename PropertyOfEachInTypeList_f<typename TypeList_::BodyTypeList,PropertyId_>::T> T;
private:
    PropertyOfEachInTypeList_f();
};

// base case
template <typename PropertyId_>
struct PropertyOfEachInTypeList_f<EmptyTypeList,PropertyId_>
{
    typedef EmptyTypeList T;
private:
    PropertyOfEachInTypeList_f();
};

} // end of namespace Tenh

#endif // TENH_CONCEPTUAL_CONCEPT_HPP_