# To Do

## Open Questions

 * How to properly initialise and manage TrafficLightSignaler.

   Currently: Completely managed by LowLevelStreet, both are initialised
   together by ModelSyncer. Where do parameters for the traffic light car come
   from?

   **Status**: Pending

 * What happens when there is no next / previous car?

   The problem is trivially solvable when using iterators through the use of
   end() (past the end) iterators. Dereferencing the end iterator is undefined
   behaviour. By defining that the iterators supplied to and returned by
   getNextCar() / getPrevCar() are iterators of the allIterable() iterable,
   the implementation could check equality to the end() iterator.

   Alternatively, an additional iterator state "no car" could be defined. An
   iterator with this state is returned by getNextCar() / getPrevCar() when
   there is no next / previous car. An iterator of this state can be
   constructed by third-party types using an additional method on the
   allIterable() iterable.

   **Status**: Practical Solution Available

 * Restructuring iterables of RfbStructure.

   Currently, there are two iterables retrievable via method calls, one for
   all vehicles and one for vehicle beyond the end of the street. The iterator
   methods of the "all" iterable could be moved to the RfbStructure class to
   simplify the most common access operations.

   This would result in the "which end() iterator to use?" problem described
   in the previous point to be more logically consistent.

   **Status**: Practical Solution Available

 * How to return the traffic light car?

   The TrafficLightSignaler proxies access to the RfbStructure and returns the
   TrafficLightCar when applicable. Returning an iterator is problematic, as
   the TrafficLightCar is not tracked by the RfbStructure.

   Instead, a pointer to a car could be returned. The iterator returned by the
   RfbStructure is dereferenced and returned; if applicable a pointer to the
   traffic light car is returned. If there is no next or previous car the
   nilptr is returned (checked by comparing to end()).

   This poses a problem for the computation routines: In many cases it is
   required to retrieve the next / previous car of the returned car. A
   solution: The RfbStructure interface is extended by adding an
   "out-of-street" state to the iterator. This state may be initialised by
   third-party types with a reference to an arbitrary car (returned upon
   dereferencing). An iterator in the "out-of-street" state is invalid to be
   passed to getNextCar() / getPrevCar().

   **Status**: Practical Solution Available

 * Traffic signal car: Only one per street or one per lane?

   There is only a single traffic signal car per street with lane = 0 in the
   current design. Is this sufficient or is it required for the traffic signal
   car to have the correct lane set? In this case multiple traffic signal cars
   per street are required.

   Multiple cars per street could be implemented using an internal array with
   a fixed number of fields. There is an upper bound on the number of lanes
   per street (3). This would be used as the size of the array.

   **Status**: To Be Discussed

 * Computation Routines in Simulator: When are instances initialised?

   Instances may be initialised upon construction of the Simulator instance.
   Alternatively, instances may be initialised when first required and stored
   in members (operator=) or only temporary (stack) during computeStep().
   Does this help with avoiding circular template definitions?

   The most obvious solution is to solve circular template definitions by
   extracting base types and initialising computation routines upon
   construction of the Simulator instance.

   **Status**: Practical Solution Available

 * Naming of applyUpdates methods on RfbStructure and LowLevelCar.

   **Status**: Pending

 * RfbStructure::incorporateInsertedCars(): Is applyUpdates called?

   Does this method call applyUpdates() on the cars? This would result in a a
   more consistent API, all writers only need to update the next* fields of
   cars. There seems to be no particular advantages for either alternative.

   **Status**: To Be Discussed

 * Cache Line Optimisations

   Do cache line optimisations on contiguous data arrays provide significant
   performance gains?

   On what structure should optimisations be performed? For example
   LowLevelCar will be wrapped in further structures by different RfbStructure
   implementations. Alignment is important as well.

   Optimisations can be applied manually by adding padding fields or
   automatically by some compilers by specifying additional keywords.

   **Status**: Pending

 * Inline short methods?

   Short methods should be inline-defined to enable advanced optimisations by
   the compiler. Especially getters will be optimised and will not result in a
   function call.

   **Status**: Practical Solution Available

 * Resolve template dependencies related to Simulator and computation routines

   **Status**: Pending
