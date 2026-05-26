#ifndef H_FACTDB_INFRA_RANGES_HPP
#define H_FACTDB_INFRA_RANGES_HPP

#if __APPLE__
#define USE_RANGES_V3
#endif

#ifdef USE_RANGES_V3
#include <range/v3/view/enumerate.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/reverse.hpp>
#include <range/v3/view/take.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/zip.hpp>
#else
#include <ranges>
#endif

namespace factDB::views {

// Helper for solving issues with c++ ranges library.
// Remove once clang(-tidy) implements ranges correctly.

#ifdef USE_RANGES_V3

using ::ranges::views::enumerate;
using ::ranges::views::filter;
using ::ranges::views::iota;
using ::ranges::views::reverse;
using ::ranges::views::take;
using ::ranges::views::transform;
using ::ranges::views::zip;

#else

using std::views::enumerate;
using std::views::filter;
using std::views::iota;
using std::views::reverse;
using std::views::take;
using std::views::transform;
using std::views::zip;

#endif

} // namespace factDB::views

#endif // H_FACTDB_INFRA_RANGES_HPP
