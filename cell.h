//===----- cell.h --------------- Static Collection -------------*- C++ -*-===//
//
//                       Base Construction Library (BCL)
//
//===----------------------------------------------------------------------===//
//
// This file defines static collection of cells with different types of stored
// data. Each cell has unique key, so this collection can be treated as a map.
//
//===----------------------------------------------------------------------===//

#ifndef CELL_H
#define CELL_H

#ifdef BCL_LEGACY
// TODO (kaniandr@gmail.com) : Remove it when all dependent files will be
// rewritten.
#include "cell_legacy.h"
#endif//BCL_LEGACY


#include <cassert>
#include <type_traits>

namespace bcl {
/// \brief This is a static map which consists of cells.
///
/// This class lets to construct a static map which consists of cells. For each
/// cell a key and a type of stored data must be specified. Inheritance is
/// used to insert a new cell to the beginning of the map. This cell inherits
/// the next cell in the map. The last cell inherits an empty map.
/// \attention Key of a cell is a structure within the ValueType type must be
/// defined. This type specifies a type of data stored in the cell.
/// \tparam Args Each parameter is a key of a cell. This key is used to access
/// an appropriate cell.
/// \sa \ref cell_test
template<class... Args> class StaticMap;

/// This represents empty static map.
template<> class StaticMap<> {
public:
  /// \brief Returns true if the map is empty, otherwise returns false.
  /// 
  /// It is also possible to use std::is_empty to check whether the map
  /// is empty. 
  static constexpr bool empty() { return true; }  
};

/// \brief This adds a new cell of data to the beginning of a static map.
template<class Head, class... Tail> class StaticMap<Head, Tail...> :
  public StaticMap<Tail...> {
public:
  
  /// Key of a cell which is used to access it.
  typedef Head CellKey;

  /// The type of a value which is stored in the cell.
  typedef typename CellKey::ValueType ValueType;

  /// The next cell in the map.
  typedef StaticMap<Tail...> CellNext;

  /// The reference to a value.
  typedef ValueType & Reference;

  /// The constant reference to a value.
  typedef const ValueType & ReferenceC;

  /// \brief Returns true if the map is empty, otherwise returns false.
  /// 
  /// It is also possible to use std::is_empty to check whether the map
  /// is empty. 
  static constexpr bool empty() { return false; }
  
  /// \brief Applies a specified function to a definition of each key in the
  /// map.
  ///
  /// The function will be applied to the current key. If this is not the last
  /// key in the map then the following key will be visited.
  /// \pre The `template<class CellTy> operator()()` method must be defined in
  /// the \c Function class.
  /// TODO (kaniandr@gmail.com): Override and make it possible to specify a list
  /// of functions.
  /// TODO (kaniandr@gmail.com): Remove __GNUC__.
  /// \sa \ref map_for_each_key "Example"
  template<class Function> static void for_each_key(Function &F) {
    check_requirements();
#ifdef __GNUC__
    F.template operator( )<StaticMap>( );
#else
    F.operator( )<StaticMap>();
#endif
    for_each_key<Function>(F, std::is_empty<CellNext>());
  }

  /// \brief Returns a value from the specified cell.
  ///
  /// \tparam What Ket of the accessed cell.
  template<class What> typename What::ValueType & value() {
    check_requirements();
    return const_cast<typename What::ValueType &>(accessValue<What>());
  }

  /// \brief Returns a value from the specified cell.
  ///
  /// \tparam What Ket of the accessed cell.
  template<class What> const typename What::ValueType & value() const {
    check_requirements();
    return accessValue<What>();
  }

  /// \brief Returns a value from the specified cell.
  ///
  /// \tparam What Key of the accessed cell.
  template<class What> typename What::ValueType & operator[](What) {
    check_requirements();
    return value<What>();
  }

  /// \brief Returns a value from the specified cell.
  ///
  /// \tparam What Key of the accessed cell.
  template<class What> const typename What::ValueType & operator[](What) const {
    check_requirements();
    return value<What>();
  }
  
  /// \brief Applies a specified function to each cell in the map.
  ///
  /// The function will be applied to the current cell. If this is not the last
  /// cell in the map then the following cell will be visited.
  /// \pre The `template<class CellTy> operator()(Cell *C)` method must be
  /// defined in the \c Function class.
  /// TODO (kaniandr@gmail.com): Override and make it possible to specify a list
  /// of functions.
  /// \sa \ref cell_for_each "Example"
  template<class Function> void for_each(Function &F) {
    check_requirements();
    F(this);
    for_each<Function>(F, std::is_empty<CellNext>());
  }

  /// Applies a specified function to each cell in the map.  
  template<class Function> void for_each(Function &F) const {
    check_requirements();
    F(this);
    for_each<Function>(F, std::is_empty<CellNext>());
  }

protected:
  /// \brief Returns a value from the specified cell.
  ///
  /// Cells treated consistently from the beginning of the collection until the
  /// desired cell is found.
  /// \tparam What Key of the accessed cell.
  template<class What> const typename What::ValueType & accessValue() const {
    return accessValue<What>(std::is_same<What, CellKey>());
  }

private:
  /// \brief Returns a value from the specified cell.
  template<class What>
  const typename What::ValueType & accessValue(std::true_type) const {
    return mValue;
  }

  /// This is invoked to visit the next cell in the map.
  template<class What>
  const typename What::ValueType & accessValue(std::false_type) const {
    return CellNext::template accessValue<What>();
  }

  /// This is invoked to visit the next cell in the map.
  template<class Function> void for_each(Function &F, std::false_type) {
    CellNext::template for_each<Function>(F);
  }

  /// This is invoked when all cells will be visited.
  template<class Function> void for_each(Function &, std::true_type) { }

  /// This is invoked to visit the next cell in the map.
  template<class Function> void for_each(Function &F, std::false_type) const {
    CellNext::template for_each<Function>(F);
  }

  /// This is invoked when all cells will be visited.
  template<class Function> void for_each(Function &, std::true_type) const { }

  /// This is invoked to visit the next cell in the map.
  template<class Function>
  static void for_each_key(Function &F, std::false_type) {
    CellNext::template for_each_key<Function>(F);
  }

  /// This is invoked when all definitions of cells will be visited.
  template<class Function>
  static void for_each_key(Function &, std::true_type) { }

  /// Checks requirements for static map usage.
  constexpr static void check_requirements();

  ValueType mValue;
};

/// \brief Determines whether the cell exists in the collection.
///
/// If there is no cell with the specified key in the collection this
/// class inherits std::false_type. In this case it provides definition of type
/// Cell equal to an empty map. Otherwise this class inherits std::true_type
/// and Cell is equal to a type of found cell.
/// \tparam Collection A collection that should be traversed.
/// \tparam What An ket of the desired cell.
template<class Collection, class What> struct IsCellExist;

namespace detail {
/// \brief Implements search of cells in the collection.
///
/// Cells treated consistently from the beginning of the collection until the
/// desired cell is found.
/// \tparam Current The cell is currently being processed.
/// \tparam What An key of the desired cell.
/// \tparam Test Indicates whether the current cell is desired.
template<class Current, class What, bool Test> struct IsCellExistImp;

/// This is a specialization in the case when the current cell is desired.
template<class Current, class What>
struct IsCellExistImp<Current, What, true> : public std::true_type {
  typedef Current Cell;
};

/// This is a specialization in the case when the desired cell is not found yet.
template<class Current, class What>
struct IsCellExistImp<Current, What, false> :
  public IsCellExist<typename Current::CellNext, What> {};
  
/// \brief Implements construction of the reversed collection.
///
/// \tparam CurrentId This cell must be inserted to the beginning of the new
/// collection which is under construction.
/// \tparam Tail The rest of the original collection.
/// \tparam Reverse This is a list of already traversed keys. When all keys 
/// are traversed a merged collection will be constructed.
template<class CurrentId, class Tail, class... Reverse> struct ReverseCellImp {
  typedef typename ReverseCellImp<typename Tail::CellKey,
    typename Tail::CellNext, CurrentId, Reverse...>::Result Result;
};

template<class CurrentId, class... Reverse>
struct ReverseCellImp<CurrentId, StaticMap<>, Reverse...> {
  typedef StaticMap<CurrentId, Reverse...> Result;
};

/// \brief Implements merging of two collections.
///
/// \tparam CurrentId This cell must be inserted to the beginning of the new
/// collection which is under construction.
/// \tparam Tail The rest of the original collection.
/// \tparam Merged This is a list of already traversed keys. When all keys 
/// are traversed a merged collection will be constructed.
template<class CurrentId, class Tail, class... Merged> struct MergeCellImp {
  typedef typename MergeCellImp<typename Tail::CellKey,
    typename Tail::CellNext, CurrentId, Merged...>::Result Result;
};

template< class CurrentId, class... Merged>
struct MergeCellImp<CurrentId, StaticMap<>, Merged...> {
  typedef StaticMap<CurrentId, Merged...> Result;
};
}

/// Implements search of cells in the collection.
template<class Collection, class What> struct IsCellExist :
  public detail::IsCellExistImp<Collection, What,
    std::is_same<typename Collection::CellKey, What>::value> { };
    
/// This is a specialization in the case when the desired cell is not found yet
/// and all cells are visited.
template<class What> struct IsCellExist<StaticMap<>, What> :
  public std::false_type {
    typedef StaticMap<> Cell;
};

template<class Head, class... Tail> 
constexpr inline void StaticMap<Head, Tail...>::check_requirements() {
  typedef typename StaticMap<Head, Tail...>::CellKey CellKey;
  typedef typename StaticMap<Head, Tail...>::CellNext CellNext;
  static_assert(!IsCellExist<CellNext, CellKey>::value,
    "Each cell must be presented only once in the map!");
}

/// \brief Reverses the order of the cells in the collection.
///
/// This class provides definition of the Result type which is equal to a type
/// of the reversed collection.
/// \tparam Collection A collection that should be reversed.
template<class Collection> struct ReverseCell {    
  typedef typename detail::ReverseCellImp<typename Collection::CellKey,
    typename Collection::CellNext>::Result Result;
};

template<> struct ReverseCell<StaticMap<> > {
    typedef StaticMap<> Result;
};

/// \brief Merges two collections.
///
/// This class provides definition of the Result type which is equal to a type
/// of the merged collection.
template<class Left, class Right> struct MergeCell {
  typedef typename MergeCell<
    typename detail::MergeCellImp<
      typename ReverseCell<Left>::Result::CellKey,
      typename ReverseCell<Left>::Result::CellNext,
      typename Right::CellKey>::Result,
    typename Right::CellNext>::Result Result;
};

template<class Left> struct MergeCell<Left, StaticMap<> > {
    typedef Left Result;
};

template<class Right> struct MergeCell<StaticMap<>, Right> {
  typedef Right Result;
};

/// Deletes memory allocated for a value which is stored in a cell if it has
/// a pointer type.
struct ClearCellFunctor {
  template<class CellTy> inline void operator()(CellTy *C) {
      typedef typename CellTy::CellKey CellKey;
      typedef typename CellKey::ValueType ValueType;
      clear(C, std::is_pointer<ValueType>());
  }

private:
  template<class CellTy> inline void clear(CellTy *C, std::true_type) {
    typedef typename CellTy::CellKey CellKey;
    if (C->template value<CellKey>())
      delete C->template value<CellKey>();
  }

  template<class CellTy> inline void clear(CellTy *, std::false_type) {}
};

/// Stores nullptr in a value which is stored in a cell if it has a pointer
struct InitCellFunctor {
  template<class CellTy> inline void operator()(CellTy *C) {
    typedef typename CellTy::CellKey CellKey;
    typedef typename CellKey::ValueType ValueType;
    init(C, std::is_pointer<ValueType>());
  }

private:
  template<class CellTy> inline void init( CellTy *C, std::true_type) {
      typedef typename CellTy::CellKey CellKey;
      C->template value< CellKey >( ) = nullptr;
  }

  template<class CellTy> inline void init(CellTy *, std::false_type) {}
};

/// \brief Copies data from one cell to another.
///
/// This can be used to copy data from one collection to another. If some cells
/// does not exist in the target (source) collection these cells are not copied.
/// \tparam From_ This is a collection which contains source data.
template<class From_> struct CopyToCellFunctor {
  typedef From_ From;

  CopyToCellFunctor(const From *F) : mFrom(F) {
    assert(F && "The source of data must not be null!");
  }

  template<class CellTy> inline void operator()(CellTy *C) {
    typedef typename CellTy::CellKey CellKey;
    copyTo(C, IsCellExist<From, CellKey>());
  }

private:
  template<class CellTy> inline void copyTo(CellTy *C, std::true_type) {
    typedef typename CellTy::CellKey CellKey;
    C->template value<CellKey>() = mFrom->template value<CellKey>();
  }

  template<class CellTy> inline void copyTo(CellTy *, std::false_type) {}

  const From *mFrom;
};


/// \brief Copies data from one cell to another.
///
/// This can be used to copy data from one collection to another. If some cells
/// does not exist in the target (source) collection these cells are not copied.
/// \tparam To_ This is a collection which contains target data.
template<class To_> struct CopyFromCellFunctor {
  typedef To_ To;

  CopyFromCellFunctor(const To *T) : mTo(T) {
    assert(T && "A target collection must not be null!");
  }

  template< class CellTy > inline void operator()(CellTy *C) {
    typedef typename CellTy::CellKey CellKey;
    copyFrom(C, IsCellExist<To, CellKey>());
  }

private:
  template<class CellTy> inline void copyFrom(CellTy *C, std::true_type) {
    typedef typename CellTy::CellKey CellKey;
    mTo->template value<CellKey>() = C->template value<CellKey>();
  }

  template<class CellTy> inline void copyFrom(CellTy *, std::false_type) {}

  const To *mTo;
};

/// \brief Moves data from one cell to another.
///
/// This can be used to move data from one collection to another. If some cells
/// does not exist in the target (source) collection these cells are not moved.
/// \tparam From_ This is a collection which contains source data.
template<class From_> struct MoveToCellFunctor {
  typedef From_ From;

  MoveToCellFunctor(const From *F) : mFrom(F) {
    assert(F && "The source of data must not be null!");
  }

  template<class CellTy> inline void operator()(CellTy *C) {
    typedef typename CellTy::CellKey CellKey;
    moveTo(C, IsCellExist<From, CellKey>());
  }

private:
  template<class CellTy> inline void moveTo(CellTy *C, std::true_type) {
    typedef typename CellTy::CellKey CellKey;
    C->template value<CellKey>() = std::move(mFrom->template value<CellKey>());
  }

  template<class CellTy> inline void moveTo(CellTy *, std::false_type) {}

  const From *mFrom;
};

/// \brief Moves data from one cell to another.
///
/// This can be used to move data from one collection to another. If some cells
/// does not exist in the target (source) collection these cells are not moved.
/// \tparam To_ This is a collection which contains target data.
template<class To_> struct MoveFromCellFunctor {
  typedef To_ To;

  MoveFromCellFunctor(const To *T) : mTo(T) {
    assert(T && "A target collection must not be null!");
  }

  template< class CellTy > inline void operator()(CellTy *C) {
    typedef typename CellTy::CellKey CellKey;
    moveFrom(C, IsCellExist<To, CellKey>());
  }

private:
  template<class CellTy> inline void moveFrom(CellTy *C, std::true_type) {
    typedef typename CellTy::CellKey CellKey;
    mTo->template value<CellKey>() = std::move(C->template value<CellKey>());
  }

  template<class CellTy> inline void moveFrom(CellTy *, std::false_type) {}

  const To *mTo;
};
}

/// \page cell_test	Example of bcl::StaticMap usage. Workers and salary
/// Let us create a record for a table reflecting the salary of workers:
///	Name | Salary
///	---- | --------
///	Smit | 300.50
/// Each record must be possible to print.
/// With each worker the following information will be associated:
/// - name will be stored in a cell with key Name;
/// - salary will be stored in a cell with key Salary. 
///	The source code of the example is available from \a \b cell_test.cpp
///
///	\dontinclude cell_test.cpp
///	1. Include necessary C++ headers:
///	\until iostream
///	2. Specify that all names from bcl namespace must be visible:
///	\skipline using
///	3. Define keys for each cell in the map:
///	\n Name of a worker:
///	\skip Name
///	\until };
///	Salary of a worker:
///	\skip Salary
///	\until };
///	4. Define static map which comprises two cells mentioned above:
///	\skipline typedef
///	5. \anchor cell_for_each_key
///	Create a functor to print structure of a static map
/// (bcl::StaticMap::for_each_key() will be used for this purpose):
///	\skip Functor
///	\until };
///	6. \anchor cell_for_each
///	Create a functor to print worker's name and salary
/// (bcl::StaticMap::for_each() will be used for this purpose):
///	\skip Functor
///	\until };
///	7. Create an empty record, initialize it with some values and print:
///	\skip main
///	\until }
///	The example displays the following output:
///	\n `Structure of salary information collection:`
///	\n `Name is <value>`
///	\n `Salary is <value>`
/// \n `Worker's salary is:`
///	\n `Name is Smit`
///	\n `Salary is 300.5`
#endif//СELL_H
